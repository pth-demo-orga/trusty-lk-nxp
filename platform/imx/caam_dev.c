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

#include <debug.h>
#include <err.h>
#include <kernel/mutex.h>
#include <kernel/vm.h>
#include <lib/trusty/sys_fd.h>
#include <lk/init.h>
#include <mm.h>
#include <platform/caam.h>
#include <platform/caam_common.h>
#include <stdio.h>
#include <string.h>
#include <trace.h>

#define LOCAL_TRACE     0

#define DRIVER_FD SYSCALL_PLATFORM_FD_CAAM
#define CHECK_FD(x) \
	do { if(x!=DRIVER_FD) return ERR_BAD_HANDLE; } while (0)

static bool valid_address(vaddr_t addr, u_int size) {
	return uthread_is_valid_range(uthread_get_current(), addr, size);
}

static uint8_t rng_blobs[CAAM_RNG_MAX_LEN];

static uint8_t key_src_buf[CAAM_KEY_MAX_LEN];
static uint8_t key_dst_buf[CAAM_KEY_MAX_LEN];

static int32_t caam_key(int32_t cmd, user_addr_t user_addr) {
	if (cmd != CAAM_IOCMD_KEY)
		return ERR_INVALID_ARGS;
	struct key_msg *msg = (struct key_msg*) user_addr;
	if (!valid_address((vaddr_t)msg->src, msg->len))
		return ERR_INVALID_ARGS;
	uint32_t ret;

	uint32_t len = msg->len;
	uint32_t delta = len % 16;
	if (msg->len > CAAM_KEY_MAX_LEN)
		return ERR_INVALID_ARGS;
	memcpy(key_src_buf, msg->src, msg->len);
	if (delta) {
		LTRACEF_LEVEL(3, "caam_key src len:%d not align to 16bytes!\n", len);
		len -= delta;
		uint8_t fake_src[16] = {0};
		uint8_t fake_dst[16] = {0};
		memcpy(fake_src, msg->src+len, delta);
		ret = caam_aes_enc(fake_src, fake_dst, 16);
		if (ret != CAAM_OK)
			return ret;
			memcpy(msg->dst+len, fake_dst, delta);
	}
	ret = caam_aes_enc(key_src_buf,key_dst_buf,len);
	memcpy(msg->dst, key_dst_buf, len);
	LTRACEF_LEVEL(3, "caam key completed!\n");

	return ret;
}

static int32_t caam_blob(int32_t cmd, user_addr_t user_addr) {
	if (cmd != CAAM_IOCMD_GENKB || cmd != CAAM_IOCMD_DEKB)
		return ERR_INVALID_ARGS;

	struct keyblob_msg *msg = (struct keyblob_msg*) user_addr;
	if (!valid_address((vaddr_t) msg->plain_text, msg->plain_len) ||
		!valid_address((vaddr_t) msg->blob, msg->plain_len + CAAM_KB_HEADER_LEN))
		return ERR_INVALID_ARGS;

	uint32_t phy_plain_text = vaddr_to_paddr(msg->plain_text);
	uint32_t phy_blob = vaddr_to_paddr(msg->blob);
	uint32_t ret;

	if (cmd == CAAM_IOCMD_GENKB)
		ret = caam_gen_blob(phy_plain_text, phy_blob, msg->plain_len);
	else
		ret = caam_decap_blob(phy_plain_text, phy_blob, msg->plain_len);

	if (ret == CAAM_SUCCESS)
		return CAAM_OK;
	else
		return CAAM_INTERNAL_ERROR;
}

static int32_t caam_rng(int32_t cmd, user_addr_t user_addr) {
	if (cmd != CAAM_IOCMD_RNG)
		return ERR_INVALID_ARGS;

	struct rng_msg *msg = (struct rng_msg*) user_addr;
	if (!valid_address((vaddr_t)msg->data, msg->len)) {
		return ERR_INVALID_ARGS;
	}
	uint32_t ret;
	uint8_t *target = msg->data;
	size_t target_len = msg->len;
	size_t work_len_sum = 0;
	size_t work_len = 0;

	while (work_len_sum < target_len) {
		work_len = target_len - work_len_sum;
		if (work_len > CAAM_RNG_MAX_LEN)
			work_len = CAAM_RNG_MAX_LEN;

		ret = caam_hwrng(rng_blobs, CAAM_RNG_MAX_LEN);
		if (ret != CAAM_SUCCESS)
			return CAAM_INTERNAL_ERROR;
		memcpy(target, rng_blobs, work_len);
		target += work_len;
		work_len_sum += work_len;
	}

	return CAAM_OK;
}

static int32_t sys_caam_read(uint32_t fd, user_addr_t user_ptr, uint32_t size) {
	LTRACEF_LEVEL(3, "CAAM read fd=%d user_ptr=0x%x size=0x%x\n", fd, user_ptr, size);
	if (!valid_address((vaddr_t)user_ptr, size)) {
		return ERR_INVALID_ARGS;
	}
	CHECK_FD(fd);
	return size;
}

static int32_t sys_caam_write(uint32_t fd, user_addr_t user_ptr, uint32_t size) {
	LTRACEF_LEVEL(3, "CAAM write fd=%d user_ptr=0x%x size=0x%x\n", fd, user_ptr, size);
	if (!valid_address((vaddr_t)user_ptr, size)) {
		return ERR_INVALID_ARGS;
	}
	CHECK_FD(fd);
	LTRACEF_LEVEL(3, "CAAM write data:\n");
#if LOCAL_TRACE
	int i = 0;
	for (i = 0; i < size; i++) {
		TRACEF("0x%x",*((unsigned char*)user_ptr + i));
	}
#endif
	return size;
}

static int32_t sys_caam_ioctl(uint32_t fd, uint32_t cmd, user_addr_t user_ptr) {
	LTRACEF_LEVEL(3, "CAAM ioctl fd=%d, cmd=0x%x, user_ptr=0x%x\n", fd, cmd, user_ptr);
	if (!valid_address((vaddr_t)user_ptr, sizeof(user_addr_t))) {
		return ERR_INVALID_ARGS;
	}
	CHECK_FD(fd);
	switch (cmd) {
		case CAAM_IOCMD_STATUS:
			return CAAM_OK;
		case CAAM_IOCMD_RNG:
			caam_open();
			return caam_rng(cmd, user_ptr);
		case CAAM_IOCMD_KEY:
			caam_open();
			return caam_key(cmd, user_ptr);
		case CAAM_IOCMD_GENKB:
		case CAAM_IOCMD_DEKB:
			caam_open();
			return caam_blob(cmd, user_ptr);
		default:
			return CAAM_OK;
	}

	return CAAM_OK;
}

static const struct sys_fd_ops caam_ops = {
	.read  = sys_caam_read,
	.write = sys_caam_write,
	.ioctl = sys_caam_ioctl,
};

void platform_init_caam(uint level) {
	TRACEF("platform_init_caam\n");
	caam_open();
	caam_test();
	install_sys_fd_handler(DRIVER_FD, &caam_ops);
}

LK_INIT_HOOK(caam_dev_init, platform_init_caam, LK_INIT_LEVEL_PLATFORM + 1);
