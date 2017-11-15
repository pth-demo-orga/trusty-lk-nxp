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
#include <trace.h>
#include <stdio.h>
#include <string.h>
#include <kernel/vm.h>
#include <kernel/mutex.h>
#include <err.h>
#include <uapi/mm.h>
#include <imx-regs.h>
#include <lk/init.h>
#include <platform/caam.h>
#include <lib/trusty/sys_fd.h>
#include <lib/sm.h>
#include <platform/hwkey_keyslots.h>
#include <platform/debug.h>

enum hwkey_err {
	HWKEY_NO_ERROR            = 0,
	HWKEY_ERR_GENERIC         = 1,
	HWKEY_ERR_NOT_VALID       = 2,
	HWKEY_ERR_BAD_LEN         = 3,
	HWKEY_ERR_NOT_IMPLEMENTED = 4,
	HWKEY_ERR_NOT_FOUND       = 5,
};

#define LOCAL_TRACE 0

#define DRIVER_FD SYSCALL_PLATFORM_FD_KEYSLOTS
#define CHECK_FD(x) \
	do { if(x!=DRIVER_FD) return ERR_BAD_HANDLE; } while (0)

static bool valid_address(vaddr_t addr, u_int size) {
       size = ROUNDUP(size + (addr & (PAGE_SIZE - 1)), PAGE_SIZE);
       addr = ROUNDDOWN(addr, PAGE_SIZE);

       while (size) {
               if (!is_user_address(addr) || !vaddr_to_paddr((void*)addr)) {
                       return false;
               }
               addr += PAGE_SIZE;
               size -= PAGE_SIZE;
       }

       return true;
}

struct hwkey_keyslot_t keyslots[10] = {
	{
		.slot_id = KEYSLOT_ID_RPMB,
	},
	{0}
};

int get_key_from_slot(char *slot_id, unsigned char* key, uint32_t * key_len)
{
	if (slot_id == NULL || key == NULL || key_len == NULL)
		return ERR_INVALID_ARGS;
	LTRACEF_LEVEL(3, "get_key_from_slot: slot_id=%s\n", slot_id);
	int i = 0;
	for (i = 0; i < KEYSLOT_NUM; i++) {
		if (keyslots[i].slot_id != NULL) {
			if (!strncmp(slot_id, keyslots[i].slot_id, KEYSLOT_ID_LEN)) {
				*key_len = keyslots[i].keylen;
				memcpy(key, keyslots[i].key, *key_len);
				break;
			}
		}
	}

	if (i >= KEYSLOT_NUM)
		return ERR_NOT_FOUND;

	return NO_ERROR;
}

static int32_t sys_keyslots_ioctl(uint32_t fd, uint32_t cmd, user_addr_t user_ptr)
{
	CHECK_FD(fd);
	if (!valid_address((vaddr_t) user_ptr, sizeof(struct keyslot_parameter_t)))
		return ERR_INVALID_ARGS;
	struct keyslot_parameter_t *msg = (struct keyslot_parameter_t*) user_ptr;

	if (!valid_address((vaddr_t) msg->slot_id, msg->slot_id_len) ||
		!valid_address((vaddr_t) msg->key, KEYSLOT_KEY_LEN))
		return ERR_INVALID_ARGS;
	if (cmd != KEYSLOT_IOCTL_GET)
		return ERR_INVALID_ARGS;

	int ret = get_key_from_slot(msg->slot_id, msg->key, msg->key_len);
	if (ret == ERR_NOT_FOUND) {
		*(msg->status) = HWKEY_ERR_NOT_FOUND;
	} else if (ret == NO_ERROR) {
		*(msg->status) = HWKEY_NO_ERROR;
	} else {
		*(msg->status) = HWKEY_ERR_GENERIC;
	}
	return ret;
}

static const struct sys_fd_ops keyslots_ops = {
	.ioctl = sys_keyslots_ioctl,
};

static void keyslots_init(uint level)
{
	status_t ret;
	unsigned char keyblob_buf[KEYSLOT_KEY_LEN + KEYBLOB_LEN];

	install_sys_fd_handler(DRIVER_FD, &keyslots_ops);

	/*
	 * keyblobs are fill in u-boot when it is in secure mode before enter TEE.
	 * All keyblobs stored in secure ram during TEE boot process.
	 * This make sure that non-secure u-boot will be not able to get keyblobs.
	 * Since secure ram will be only accessible for secure world.
	 */

	struct keyslot_package *sec_ram_p = (struct keyslot_package *)SECURE_RAM_START_ADDR;
	if (strcmp(sec_ram_p->magic, KEYPACK_MAGIC)) {
		LTRACEF_LEVEL(0, "keyslots wrong magic when init.");
		return;
	}

	if (sec_ram_p->rpmb_keyblob_len == 0) {
		LTRACEF_LEVEL(0, "secure parameter has no RPMB key.\n");
		return;
	}
	memcpy(keyblob_buf, sec_ram_p->rpmb_keyblob, sec_ram_p->rpmb_keyblob_len);
	caam_open();
	ret = caam_decap_blob(vaddr_to_paddr(keyslots[KEYSLOT_IDX_RPMB].key),
		vaddr_to_paddr(keyblob_buf), sec_ram_p->rpmb_keyblob_len - KEYBLOB_LEN);

	if (ret != CAAM_SUCCESS) {
		LTRACEF_LEVEL(0, "keyslots_init rpmb_key init failed\n");
		return;
	} else {
		keyslots[KEYSLOT_IDX_RPMB].keylen = sec_ram_p->rpmb_keyblob_len - KEYBLOB_LEN;
		LTRACEF_LEVEL(3, "rpmb key inited.\n");
		/* Use RPMB key as aes key */
		install_aeskey((uint8_t *)keyslots[KEYSLOT_IDX_RPMB].key);
	}
}

LK_INIT_HOOK(keyslot, keyslots_init, LK_INIT_LEVEL_PLATFORM);
