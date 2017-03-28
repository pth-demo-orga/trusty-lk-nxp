/*
 * Copyright (C) 2017 The Android Open Source Project
 * Copyright 2017 NXP
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <assert.h>
#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <trusty_std.h>
#include <interface/hwkey/hwkey.h>
#include <openssl/cipher.h>
#include <openssl/aes.h>
#include <openssl/digest.h>
#include <openssl/err.h>
#include <openssl/hkdf.h>
#include <platform/caam_common.h>
#include <platform/hwkey_keyslots_common.h>

#include "common.h"
#include "uuids.h"
#include "hwkey_srv_priv.h"

#define LOCAL_TRACE  1
#define LOG_TAG      "hwkey_srv"

/*
 * Derive key V1 - AES based key derive.
 */
uint32_t derive_key_v1(const uuid_t *uuid,
		       const uint8_t *ikm_data, size_t ikm_len,
		       uint8_t *key_buf, size_t *key_len)
{
	int ret = 0;
	struct key_msg ioctl_msg;
	ioctl_msg.src = ikm_data;
	ioctl_msg.dst = key_buf;
	ioctl_msg.len = ikm_len;

	ret = ioctl(SYSCALL_PLATFORM_FD_CAAM, CAAM_IOCMD_KEY, &ioctl_msg);

	if (ret != CAAM_OK) {
		TLOGE("error in ioctl ret=%d\n", ret);
		return HWKEY_ERR_GENERIC;
	}
	*key_len = ikm_len;

	return HWKEY_NO_ERROR;
}

/*
 *  RPMB Key support
 */
#define RPMB_SS_AUTH_KEY_SIZE    32
#define RPMB_SS_AUTH_KEY_ID      "com.android.trusty.storage_auth.rpmb"

/* Secure storage service app uuid */
static const uuid_t ss_uuid = SECURE_STORAGE_SERVER_APP_UUID;

/*
 * Fetch RPMB Secure Storage Authentication key
 */
static uint32_t get_rpmb_ss_auth_key(const struct hwkey_keyslot *slot,
				     uint8_t *kbuf, size_t kbuf_len, size_t *klen)
{
	struct keyslot_parameter_t rpmb_slot;
	rpmb_slot.slot_id_len = strlen(RPMB_SS_AUTH_KEY_ID);
	status_t stat;
	rpmb_slot.status = &stat;

	rpmb_slot.key_len = klen;
	strcpy(rpmb_slot.slot_id, RPMB_SS_AUTH_KEY_ID);
	ioctl(SYSCALL_PLATFORM_FD_KEYSLOTS, KEYSLOT_IOCTL_GET, &rpmb_slot);

	if (*rpmb_slot.status == HWKEY_NO_ERROR) {
		memcpy(kbuf, rpmb_slot.key, *klen);
		return HWKEY_NO_ERROR;
	} else {
		return HWKEY_ERR_GENERIC;
	}
}

/*
 *  List of keys slots that hwkey service supports
 */
static const struct hwkey_keyslot _keys[] = {
	{
		.uuid = &ss_uuid,
		.key_id = RPMB_SS_AUTH_KEY_ID,
		.handler = get_rpmb_ss_auth_key,
	},
};

/*
 *  Initialize Fake HWKEY service provider
 */
void hwkey_init_srv_provider(void)
{
	int rc;

	TLOGE("Init HWKEY service provider\n");

	/* install key handlers */
	hwkey_install_keys(_keys, countof(_keys));

	/* start service */
	rc = hwkey_start_service();
	if (rc != NO_ERROR ) {
		TLOGE("failed (%d) to start HWKEY service\n", rc);
	}
}
