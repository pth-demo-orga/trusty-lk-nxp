/*
 * Copyright (C) 2016-2017 The Android Open Source Project
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

#include <err.h>
#include <platform/caam_common.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "hwrng_srv_priv.h"

#define LOCAL_TRACE  1
#define LOG_TAG      "hwrng_srv"


void hwrng_dev_get_rng_data(uint8_t *buf, size_t buf_len)
{
	struct rng_msg msg = {
		.data = buf,
		.len = buf_len,
	};
	int ret;

	ret = ioctl(SYSCALL_PLATFORM_FD_CAAM, CAAM_IOCMD_RNG, &msg);
	if (ret != CAAM_OK)
		TLOGE("error in ioctl ret=%d\n", ret);
}

void hwrng_init_srv_provider(void)
{
	int rc;

	TLOGE("Init HWRNG service provider\n");
	/* Nothing to initialize here, just start service */
	rc = hwrng_start_service();
	if (rc != NO_ERROR) {
		TLOGE("failed (%d) to start HWRNG service\n", rc);
	}
}

