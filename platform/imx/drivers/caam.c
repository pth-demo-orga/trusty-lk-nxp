/*
 * Copyright (C) 2016 Freescale Semiconductor, Inc.
 * Copyright 2017 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <imx-regs.h>
#include <arch/ops.h>
#include <platform/caam.h>
#include <reg.h>
#include <stdio.h>
#include <string.h>

#include "fsl_caam_internal.h"

/* arm v7 need 64 align */
#define ALIGN_MASK 0xffffffc0

/* Input job ring - single entry input ring */
uint32_t g_input_ring[JOB_RING_ENTRIES] = {0};

/* Output job ring - single entry output ring (consists of two words) */
uint32_t g_output_ring[2*JOB_RING_ENTRIES] = {0, 0};

uint32_t decap_dsc[] = {
	DECAP_BLOB_DESC1,
	DECAP_BLOB_DESC2,
	DECAP_BLOB_DESC3,
	DECAP_BLOB_DESC4,
	DECAP_BLOB_DESC5,
	DECAP_BLOB_DESC6,
	DECAP_BLOB_DESC7,
	DECAP_BLOB_DESC8,
	DECAP_BLOB_DESC9
};

uint32_t encap_dsc[] = {
	ENCAP_BLOB_DESC1,
	ENCAP_BLOB_DESC2,
	ENCAP_BLOB_DESC3,
	ENCAP_BLOB_DESC4,
	ENCAP_BLOB_DESC5,
	ENCAP_BLOB_DESC6,
	ENCAP_BLOB_DESC7,
	ENCAP_BLOB_DESC8,
	ENCAP_BLOB_DESC9
};

uint32_t keygen_dsc[8] = {0};
uint32_t hash_dsc[8] = {0};
uint32_t hwrng_dsc[6] = {0};

uint32_t rng_inst_dsc[] = {
	RNG_INST_DESC1,
	RNG_INST_DESC2,
	RNG_INST_DESC3,
	RNG_INST_DESC4,
	RNG_INST_DESC5,
	RNG_INST_DESC6,
	RNG_INST_DESC7,
	RNG_INST_DESC8,
	RNG_INST_DESC9
};

static uint8_t skeymod[] = {
	0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09, 0x08,
	0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00
};

void caam_open(void) {
	uint32_t temp_reg;
	struct mxc_ccm_reg *mxc_ccm = (struct mxc_ccm_reg *)CCM_BASE_ADDR_VIRT;

	/* switch on the clock */
	temp_reg = readl(&mxc_ccm->CCGR0);
	temp_reg |= (3 << 8) | (3 << 10) | (3 << 12);

	writel(temp_reg, &mxc_ccm->CCGR0);

	/* Initialize job ring addresses */
	writel((uint32_t)g_input_ring, CAAM_IRBAR0);   // input ring address
	writel((uint32_t)g_output_ring, CAAM_ORBAR0);  // output ring address

	/* Initialize job ring sizes to 1 */
	writel(JOB_RING_ENTRIES, CAAM_IRSR0);
	writel(JOB_RING_ENTRIES, CAAM_ORSR0);

	/* HAB disables interrupts for JR0 so do the same here */
	temp_reg = readl(CAAM_JRCFGR0_LS) | JRCFG_LS_IMSK;
	writel(temp_reg, CAAM_JRCFGR0_LS);

	/* if RNG already instantiated then skip it */
	if ((readl(CAAM_RDSTA) & RDSTA_IF0) != RDSTA_IF0) {
		/* Enter TRNG Program mode */
		writel(RTMCTL_PGM, CAAM_RTMCTL);

		/* Set OSC_DIV field to TRNG */
		temp_reg = readl(CAAM_RTMCTL) | (RNG_TRIM_OSC_DIV << 2);
		writel(temp_reg, CAAM_RTMCTL);

		/* Set delay */
		writel(((RNG_TRIM_ENT_DLY << 16) | 0x09C4), CAAM_RTSDCTL);
		writel((RNG_TRIM_ENT_DLY >> 1), CAAM_RTFRQMIN);
		writel((RNG_TRIM_ENT_DLY << 4), CAAM_RTFRQMAX);

		/* Resume TRNG Run mode */
		temp_reg = readl(CAAM_RTMCTL) ^ RTMCTL_PGM;
		writel(temp_reg, CAAM_RTMCTL);

		temp_reg = readl(CAAM_RTMCTL) | RTMCTL_ERR;
		writel(temp_reg, CAAM_RTMCTL);

		/* Run descriptor to instantiate the RNG */
		/* Add job to input ring */
		g_input_ring[0] = (uint32_t)rng_inst_dsc;

		arch_clean_invalidate_cache_range((uint32_t)g_input_ring & ALIGN_MASK, 128 + ~ALIGN_MASK);
		/* Increment jobs added */
		writel(1, CAAM_IRJAR0);

		/* Wait for job ring to complete the job: 1 completed job expected */
		size_t timeout = 100000;
		while(readl(CAAM_ORSFR0) != 1 && timeout--);

		arch_clean_invalidate_cache_range((uint32_t)g_output_ring & ALIGN_MASK, 128 + ~ALIGN_MASK);

		/* check that descriptor address is the one expected in the out ring */
		if(g_output_ring[0] == (uint32_t)rng_inst_dsc) {
			/* check if any error is reported in the output ring */
			if ((g_output_ring[1] & JOB_RING_STS) != 0) {
				printf("Error: RNG instantiation error");
			}
		} else {
			printf("Error: RNG job output ring descriptor address does not match");
		}

		/* ensure that the RNG was correctly instantiated */
		temp_reg = readl(CAAM_RDSTA);
		if (temp_reg != (RDSTA_IF0 | RDSTA_SKVN)) {
			printf("Error: RNG instantiation failed 0x%X\n", temp_reg);
		}

		/* Remove job from Job Ring Output Queue */
		writel(1, CAAM_ORJRR0);
	}

	return;
}

uint32_t caam_decap_blob(uint8_t *plain_text, uint8_t *blob_addr, uint32_t size) {
	uint32_t ret = CAAM_SUCCESS;

	decap_dsc[0] = (uint32_t)0xB0800008;
	decap_dsc[1] = (uint32_t)0x14400010;
	decap_dsc[2] = (uint32_t)skeymod;
	decap_dsc[3] = (uint32_t)0xF0000000 | (0x0000ffff & (size+48) );
	decap_dsc[4] = (uint32_t)blob_addr;
	decap_dsc[5] = (uint32_t)0xF8000000 | (0x0000ffff & (size));
	decap_dsc[6] = (uint32_t)(uint8_t*)plain_text;
	decap_dsc[7] = (uint32_t)0x860D0000;

	g_input_ring[0] = (uint32_t)decap_dsc;

	arch_clean_invalidate_cache_range((uint32_t)blob_addr & ALIGN_MASK, 2 * size + ~ALIGN_MASK);
	arch_clean_invalidate_cache_range((uint32_t)plain_text & ALIGN_MASK, 2 * size + ~ALIGN_MASK);
	arch_clean_cache_range((uint32_t)decap_dsc & ALIGN_MASK,  1024 + ~ALIGN_MASK);
	arch_clean_cache_range((uint32_t)g_input_ring & ALIGN_MASK,1024 + ~ALIGN_MASK);
	/* Increment jobs added */
	writel(1, CAAM_IRJAR0);

	/* Wait for job ring to complete the job: 1 completed job expected */
	size_t timeout = 100000;
	while(readl(CAAM_ORSFR0) != 1 && timeout--);

	arch_clean_invalidate_cache_range((uint32_t)g_output_ring & ALIGN_MASK, 128 + ~ALIGN_MASK);
	/* check that descriptor address is the one expected in the output ring */
	if(g_output_ring[0] == (uint32_t)decap_dsc) {
		/* check if any error is reported in the output ring */
		if ((g_output_ring[1] & JOB_RING_STS) != 0) {
			printf("Error: blob decap job completed with error");
			ret = CAAM_FAILURE;
		}
	} else {
		printf("Error: blob decap job output ring descriptor address does not match\n");
		ret = CAAM_FAILURE;
	}
	arch_clean_cache_range((uint32_t)plain_text & ALIGN_MASK, ((uint32_t)plain_text & ALIGN_MASK) + 2 * size + ~ALIGN_MASK);

	/* Remove job from Job Ring Output Queue */
	writel(1, CAAM_ORJRR0);

	return ret;
}

uint32_t caam_gen_blob(uint8_t *plain_data_addr, uint8_t *blob_addr, uint32_t size) {

	uint32_t ret = CAAM_SUCCESS;

	uint8_t *blob = (uint8_t *)blob_addr;

	memset(blob,0,size);

	encap_dsc[0] = (uint32_t)0xB0800008;
	encap_dsc[1] = (uint32_t)0x14400010;
	encap_dsc[2] = (uint32_t)skeymod;
	encap_dsc[3] = (uint32_t)0xF0000000 | (0x0000ffff & (size));
	encap_dsc[4] = (uint32_t)plain_data_addr;
	encap_dsc[5] = (uint32_t)0xF8000000 | (0x0000ffff & (size+48));
	encap_dsc[6] = (uint32_t)blob;
	encap_dsc[7] = (uint32_t)0x870D0000;

	g_input_ring[0] = (uint32_t)encap_dsc;
	arch_clean_invalidate_cache_range((uint32_t)g_input_ring & ALIGN_MASK, 1024 + ~ALIGN_MASK);

	arch_clean_invalidate_cache_range((uint32_t)plain_data_addr & ALIGN_MASK,  2 * size + ~ALIGN_MASK);
	arch_clean_cache_range((uint32_t)encap_dsc & ALIGN_MASK, 1024 + ~ALIGN_MASK);
	arch_clean_invalidate_cache_range((uint32_t)blob & ALIGN_MASK,  2 * size + ~ALIGN_MASK);
	/* Increment jobs added */
	writel(1, CAAM_IRJAR0);

	/* Wait for job ring to complete the job: 1 completed job expected */
	size_t timeout = 100000;
	while(readl(CAAM_ORSFR0) == 0 && timeout--);
	arch_clean_invalidate_cache_range((uint32_t)g_output_ring & ALIGN_MASK, 1024 + ~ALIGN_MASK);

	/* check that descriptor address is the one expected in the output ring */
	if(g_output_ring[0] == (uint32_t)encap_dsc) {
		/* check if any error is reported in the output ring */
		if ((g_output_ring[1] & JOB_RING_STS) != 0) {
			printf("Error: blob encap job completed with error");
			ret = CAAM_FAILURE;
		}
	} else {
		printf("Error: blob encap job output ring descriptor address does not match\n");
		ret = CAAM_FAILURE;
	}

	/* Remove job from Job Ring Output Queue */
	writel(1, CAAM_ORJRR0);

	return ret;
}

uint32_t caam_aes_dec(uint8_t *input, uint8_t *output_ptr, uint32_t len_input) {
	uint32_t ret = CAAM_SUCCESS;

	uint8_t *output = (uint8_t *)output_ptr;

	memset(output,0,len_input);

	/*
	 * TODO:
	 * Now AES key use skeymod.
	 * Will replace this one by the RPMB key
	 * which injected by bootloader.
	 */
	int n = 0;
	keygen_dsc[n++] = (uint32_t)0xb0800008;
	keygen_dsc[n++] = (uint32_t)0x02000010;
	keygen_dsc[n++] = (uint32_t)skeymod;
	keygen_dsc[n++] = (uint32_t)0x8210020C;
	keygen_dsc[n++] = (uint32_t)0x22120000 | (0x0000ffff & len_input);
	keygen_dsc[n++] = (uint32_t)input;
	keygen_dsc[n++] = (uint32_t)0x60300000 | (0x0000ffff & len_input);
	keygen_dsc[n++] = (uint32_t)output;

	g_input_ring[0] = (uint32_t)keygen_dsc;
	arch_clean_invalidate_cache_range((uint32_t)g_input_ring & ALIGN_MASK, 1024 + ~ALIGN_MASK);

	arch_clean_invalidate_cache_range((uint32_t)input & ALIGN_MASK,  2 * len_input + ~ALIGN_MASK);
	arch_clean_cache_range((uint32_t)keygen_dsc& ALIGN_MASK, 1024 + ~ALIGN_MASK);
	arch_clean_invalidate_cache_range((uint32_t)output & ALIGN_MASK,  2 * len_input + ~ALIGN_MASK);
	/* Increment jobs added */
	writel(1, CAAM_IRJAR0);

	/* Wait for job ring to complete the job: 1 completed job expected */
	size_t timeout = 100000;
	while(readl(CAAM_ORSFR0) != 1 && timeout--);
	arch_clean_invalidate_cache_range((uint32_t)g_output_ring & ALIGN_MASK, 1024 + ~ALIGN_MASK);

	/* check that descriptor address is the one expected in the output ring */
	if(g_output_ring[0] == (uint32_t)keygen_dsc) {
		/* check if any error is reported in the output ring */
		if ((g_output_ring[1] & JOB_RING_STS) != 0) {
			printf("Error: aes enc job completed with error.");
			ret = CAAM_FAILURE;
		}
	} else {
		printf("Error: aes enc output ring descriptor address does not match\n");
	}

	/* Remove job from Job Ring Output Queue */
	writel(1, CAAM_ORJRR0);

	return ret;
}

uint32_t caam_aes_enc(uint8_t *input, uint8_t *output_ptr, uint32_t len_input) {
	uint32_t ret = CAAM_SUCCESS;
	uint8_t *output = (uint8_t *)output_ptr;

	memset(output,0,len_input);

	/*
	 * TODO:
	 * Now AES key use skeymod.
	 * Will replace this one by the RPMB key
	 * which injected by bootloader.
	 */
	int n = 0;
	keygen_dsc[n++] = (uint32_t)0xb0800008;
	keygen_dsc[n++] = (uint32_t)0x02000010;
	keygen_dsc[n++] = (uint32_t)skeymod;
	keygen_dsc[n++] = (uint32_t)0x8210020D;
	keygen_dsc[n++] = (uint32_t)0x22120000 | (0x0000ffff & len_input);
	keygen_dsc[n++] = (uint32_t)input;
	keygen_dsc[n++] = (uint32_t)0x60300000 | (0x0000ffff & len_input);
	keygen_dsc[n++] = (uint32_t)output;

	g_input_ring[0] = (uint32_t)keygen_dsc;
	arch_clean_invalidate_cache_range((uint32_t)g_input_ring & ALIGN_MASK, 1024 + ~ALIGN_MASK);

	arch_clean_invalidate_cache_range((uint32_t)input & ALIGN_MASK,  2 * len_input + ~ALIGN_MASK);
	arch_clean_cache_range((uint32_t)keygen_dsc& ALIGN_MASK, 1024 + ~ALIGN_MASK);
	arch_clean_invalidate_cache_range((uint32_t)output & ALIGN_MASK,  2 * len_input + ~ALIGN_MASK);
	/* Increment jobs added */
	writel(1, CAAM_IRJAR0);

	/* Wait for job ring to complete the job: 1 completed job expected */
	size_t timeout = 100000;
	while(readl(CAAM_ORSFR0) != 1 && timeout--);
	arch_clean_invalidate_cache_range((uint32_t)g_output_ring & ALIGN_MASK, 1024 + ~ALIGN_MASK);

	/* check that descriptor address is the one expected in the output ring */
	if(g_output_ring[0] == (uint32_t)keygen_dsc) {
		/* check if any error is reported in the output ring */
		if ((g_output_ring[1] & JOB_RING_STS) != 0) {
			printf("Error: aes enc job completed with error");
			ret = CAAM_FAILURE;
		}
	} else {
		printf("Error: aes enc output ring descriptor address does not match\n");
	}

	/* Remove job from Job Ring Output Queue */
	writel(1, CAAM_ORJRR0);

	return ret;
}
uint32_t caam_hwrng(uint8_t *output_ptr, uint32_t output_len) {
	uint32_t ret = CAAM_SUCCESS;

	/* Buffer to hold the resulting output*/
	uint8_t *output = (uint8_t *)output_ptr;

	/* initialize the output array */
	memset(output,0,output_len);

	int n = 0;
	hwrng_dsc[n++] = (uint32_t)0xB0800004;
	hwrng_dsc[n++] = (uint32_t)0x82500000;
	hwrng_dsc[n++] = (uint32_t)0x60340000| (0x0000ffff & output_len);
	hwrng_dsc[n++] = (uint32_t)output;

	g_input_ring[0] = (uint32_t)hwrng_dsc;
	arch_clean_invalidate_cache_range((uint32_t)g_input_ring & ALIGN_MASK, 1024 + ~ALIGN_MASK);

	arch_clean_cache_range((uint32_t)hwrng_dsc& ALIGN_MASK, 1024 + ~ALIGN_MASK);
	arch_clean_invalidate_cache_range((uint32_t)output & ALIGN_MASK,  2 * output_len + ~ALIGN_MASK);
	/* Increment jobs added */
	writel(1, CAAM_IRJAR0);

	/* Wait for job ring to complete the job: 1 completed job expected */
	size_t timeout = 100000;
	while(readl(CAAM_ORSFR0) != 1 && timeout--);
	arch_clean_invalidate_cache_range((uint32_t)g_output_ring & ALIGN_MASK, 1024 + ~ALIGN_MASK);

	/* check that descriptor address is the one expected in the output ring */
	if(g_output_ring[0] == (uint32_t)hwrng_dsc) {
		/* check if any error is reported in the output ring */
		if ((g_output_ring[1] & JOB_RING_STS) != 0) {
			printf("Error: RNG job completed with error");
			ret = CAAM_FAILURE;
		}
	} else {
		printf("Error: RNG output ring descriptor address does not match\n");
	}

	/* Remove job from Job Ring Output Queue */
	writel(1, CAAM_ORJRR0);

	return ret;
}

uint32_t caam_hash(uint8_t *input, uint8_t *output_ptr, uint32_t len_input) {
	uint32_t ret = CAAM_SUCCESS;

	/* Buffer to hold the resulting output*/
	uint8_t *output = (uint8_t *)output_ptr;

	/* initialize the output array */
	memset(output,0,len_input);

	int n = 0;
	hash_dsc[n++] = (uint32_t)0xB0800006;
	hash_dsc[n++] = (uint32_t)0x8441000D;
	hash_dsc[n++] = (uint32_t)0x24140000 | (0x0000ffff & len_input);
	hash_dsc[n++] = (uint32_t)input;
	hash_dsc[n++] = (uint32_t)0x54200000 | 20;
	hash_dsc[n++] = (uint32_t)output;

	g_input_ring[0] = (uint32_t)hash_dsc;
	arch_clean_invalidate_cache_range((uint32_t)g_input_ring & ALIGN_MASK, 1024 + ~ALIGN_MASK);

	arch_clean_invalidate_cache_range((uint32_t)input & ALIGN_MASK,  2 * len_input + ~ALIGN_MASK);
	arch_clean_cache_range((uint32_t)hash_dsc& ALIGN_MASK, 1024 + ~ALIGN_MASK);
	arch_clean_invalidate_cache_range((uint32_t)output & ALIGN_MASK,  2 * len_input + ~ALIGN_MASK);
	/* Increment jobs added */
	writel(1, CAAM_IRJAR0);

	/* Wait for job ring to complete the job: 1 completed job expected */
	size_t timeout = 100000;
	while(readl(CAAM_ORSFR0) != 1 && timeout--);
	arch_clean_invalidate_cache_range((uint32_t)g_output_ring & ALIGN_MASK, 1024 + ~ALIGN_MASK);

	/* check that descriptor address is the one expected in the output ring */
	if(g_output_ring[0] == (uint32_t)hash_dsc) {
		/* check if any error is reported in the output ring */
		if ((g_output_ring[1] & JOB_RING_STS) != 0) {
			printf("Error: aes enc job completed with error");
			ret = CAAM_FAILURE;
		}
	} else {
		printf("Error: aes enc output ring descriptor address does not match\n");
	}

	/* Remove job from Job Ring Output Queue */
	writel(1, CAAM_ORJRR0);

	return ret;
}

void caam_test(void) {

#define BUF_LEN 32

	char plain[BUF_LEN] = {0};
	unsigned char plain_bak[BUF_LEN] = {0};
	unsigned char blob[BUF_LEN+48] = {0};
	int i = 0;
	for (i = 0; i< BUF_LEN; i++) {
		plain[i] = i + '0';
		plain_bak[i] = plain[i];
	}

	caam_gen_blob(plain, blob, BUF_LEN);
	plain[0] = 0xff;
	caam_decap_blob(plain, blob, BUF_LEN);

	for (i=0; i< BUF_LEN; i++) {
		if (plain[i] != plain_bak[i]) {
			printf("caam test failed @ %d\n", i);
		}
	}

	if (i == BUF_LEN)
		printf("caam keyblob test PASS!!!\n");
	else
		printf("caam keyblob test failed!\n");

	uint8_t input[BUF_LEN];
	uint8_t output[BUF_LEN];
	uint8_t output2[BUF_LEN];

	for (i = 0; i < BUF_LEN; i++) {
		input[i] = i;
	}
	caam_aes_enc(input,output, BUF_LEN);
	caam_aes_enc(input,output2, BUF_LEN);

	for (i = 0; i < BUF_LEN; i++) {
		if (output[i] != output2[i]) {
			printf("caam AES enc error @@%d\n", i);
			break;
		}
	}
	if (i != BUF_LEN)
		printf("caam AES enc test failed\n");
	else
		printf("caam AES enc test PASS!!!\n");

	caam_aes_dec(output2, output, BUF_LEN);

	for (i = 0; i < BUF_LEN; i++) {
		if (output[i] != input[i]) {
			printf("caam AES dec error @%d\n", i);
		}
	}

	if (i != BUF_LEN)
		printf("caam AES dec test failed\n");
	else
		printf("caam AES dec test PASS!!!\n");
}

