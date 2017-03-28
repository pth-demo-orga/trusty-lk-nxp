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

#ifndef _HWKEY_KEYSLOTS_COMMON_H_
#define _HWKEY_KEYSLOTS_COMMON_H_

#define SYSCALL_PLATFORM_FD_KEYSLOTS 0x9

#define KEYSLOT_IOCTL_GET    0xA

#define KEYSLOT_ID_LEN   1024
#define KEYSLOT_KEY_LEN  2048

#define KEYSLOT_ID_RPMB "com.android.trusty.storage_auth.rpmb"
#define KEYSLOT_IDX_RPMB 0

#define KEYSLOT_ID_PUBKEY "avb_public_key_slot"
#define KEYSLOT_IDX_PUBKEY 1

#define KEYBLOB_LEN 48
#define RPMBKEY_LEN (32 + KEYBLOB_LEN)
#define PUBKEY_LEN (1032 + KEYBLOB_LEN)
#define KEYPACK_MAGIC "!KS"

struct keyslot_parameter_t
{
	char  slot_id[KEYSLOT_ID_LEN];
	uint32_t slot_id_len;
	unsigned char  key[KEYSLOT_KEY_LEN];
	uint32_t *key_len;
	status_t *status;
};

#endif
