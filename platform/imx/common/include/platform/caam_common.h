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

#ifndef _CAAM_COMMON_H_
#define _CAAM_COMMON_H_

#define SYSCALL_PLATFORM_FD_CAAM	0x8 //Maxium 0xA
#define CAAM_RNG_MAX_LEN		256
#define CAAM_KEY_MAX_LEN		256

#define CAAM_KB_MISC_LEN		48

#define CAAM_IOCMD_STATUS		0x00000001
#define CAAM_IOCMD_RNG			0x00000002
#define CAAM_IOCMD_KEY			0x00000004
#define CAAM_IOCMD_GENKB		0x00000008
#define CAAM_IOCMD_DEKB			0x00000010

#define CAAM_OK				0x0
#define CAAM_NOT_OPEN			0x1
#define CAAM_INTERNAL_ERROR		0x2

struct rng_msg {
	uint8_t *data;
	size_t len; 
};

struct key_msg {
	uint8_t *src;
	uint8_t *dst;
	uint32_t len;
};

struct keyblob_msg {
	uint8_t *plain_text;
	uint8_t *blob;
	uint32_t plain_len;
};

#endif
