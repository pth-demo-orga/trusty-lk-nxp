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

#include <platform/tzasc.h>

int check_region_table_end(tzasc_region_t *region) {

	if ((region->addr_l | region->addr_h | region->attr) == 0)
		return 1;

	return 0;
}

int write_tzasc_region(tzasc_region_t *region, int region_num) {

	if (region != NULL) {
		TZ_REG(TZ_GET_REGION_ADDR(region_num)) = region->addr_l;
		TZ_REG(TZ_GET_REGION_ADDR(region_num) + 0x4) = region->addr_h;
		TZ_REG(TZ_GET_REGION_ADDR(region_num) + 0x8) = region->attr;
	} else {
		return -1;
	}

    return 0;
}

int initial_tzasc(tzasc_region_t* regions) {

	int ret = 0;
	/*
	 * ACTION field 0x2 means
	 * sets tzasc_int HIGH and issues an OKAY response
	 */
	TZ_REG(TZ_ACTION) = 0x2;

	//From number 0 region to config.
	int region_num = 0;
	while(!(check_region_table_end(regions))) {
		if (write_tzasc_region(regions, region_num)) {
			ret = -1;
			goto out;
		}
		region_num++;
		regions++;
	}

	TZ_REG(TZ_INT_CLEAR) = 0;

out:
	return ret;
}
