/*
 * Copyright (c) 2012-2016, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __CAAM_INTERNAL_H__
#define	__CAAM_INTERNAL_H__

/* 4kbyte pages */
#define CAAM_SEC_RAM_START_ADDR CAAM_ARB_BASE_ADDR

#define SEC_MEM_PAGE0       CAAM_SEC_RAM_START_ADDR
#define SEC_MEM_PAGE1       (CAAM_SEC_RAM_START_ADDR + 0x1000)
#define SEC_MEM_PAGE2       (CAAM_SEC_RAM_START_ADDR + 0x2000)
#define SEC_MEM_PAGE3       (CAAM_SEC_RAM_START_ADDR + 0x3000)

/* Configuration and special key registers */
#define CAAM_MCFGR          CAAM_BASE_ADDR + 0x0004
#define CAAM_SCFGR          CAAM_BASE_ADDR + 0x000c
#define CAAM_JR0MIDR        CAAM_BASE_ADDR + 0x0010
#define CAAM_JR1MIDR        CAAM_BASE_ADDR + 0x0018
#define CAAM_DECORR         CAAM_BASE_ADDR + 0x009c
#define CAAM_DECO0MID       CAAM_BASE_ADDR + 0x00a0
#define CAAM_DAR            CAAM_BASE_ADDR + 0x0120
#define CAAM_DRR            CAAM_BASE_ADDR + 0x0124
#define CAAM_JDKEKR         CAAM_BASE_ADDR + 0x0400
#define CAAM_TDKEKR         CAAM_BASE_ADDR + 0x0420
#define CAAM_TDSKR          CAAM_BASE_ADDR + 0x0440
#define CAAM_SKNR           CAAM_BASE_ADDR + 0x04e0
#define CAAM_SMSTA          CAAM_BASE_ADDR + 0x0FB4
#define CAAM_STA            CAAM_BASE_ADDR + 0x0FD4
#define CAAM_SMPO_0         CAAM_BASE_ADDR + 0x1FBC

/* RNG registers */
#define CAAM_RTMCTL         CAAM_BASE_ADDR + 0x0600
#define CAAM_RTSDCTL        CAAM_BASE_ADDR + 0x0610
#define CAAM_RTFRQMIN       CAAM_BASE_ADDR + 0x0618
#define CAAM_RTFRQMAX       CAAM_BASE_ADDR + 0x061C
#define CAAM_RTSTATUS       CAAM_BASE_ADDR + 0x063C
#define CAAM_RDSTA          CAAM_BASE_ADDR + 0x06C0

/* Job Ring 0 registers */
#define CAAM_IRBAR0         CAAM_BASE_ADDR + 0x1004
#define CAAM_IRSR0          CAAM_BASE_ADDR + 0x100c
#define CAAM_IRSAR0         CAAM_BASE_ADDR + 0x1014
#define CAAM_IRJAR0         CAAM_BASE_ADDR + 0x101c
#define CAAM_ORBAR0         CAAM_BASE_ADDR + 0x1024
#define CAAM_ORSR0          CAAM_BASE_ADDR + 0x102c
#define CAAM_ORJRR0         CAAM_BASE_ADDR + 0x1034
#define CAAM_ORSFR0         CAAM_BASE_ADDR + 0x103c
#define CAAM_JRSTAR0        CAAM_BASE_ADDR + 0x1044
#define CAAM_JRINTR0        CAAM_BASE_ADDR + 0x104c
#define CAAM_JRCFGR0_MS     CAAM_BASE_ADDR + 0x1050
#define CAAM_JRCFGR0_LS     CAAM_BASE_ADDR + 0x1054
#define CAAM_IRRIR0         CAAM_BASE_ADDR + 0x105c
#define CAAM_ORWIR0         CAAM_BASE_ADDR + 0x1064
#define CAAM_JRCR0          CAAM_BASE_ADDR + 0x106c
#define CAAM_SMCJR0         CAAM_BASE_ADDR + 0x10f4
#define CAAM_SMCSJR0        CAAM_BASE_ADDR + 0x10fc
#define CAAM_SMAPJR0(y)     (CAAM_BASE_ADDR + 0x1104 + y*16)
#define CAAM_SMAG2JR0(y)    (CAAM_BASE_ADDR + 0x1108 + y*16)
#define CAAM_SMAG1JR0(y)    (CAAM_BASE_ADDR + 0x110C + y*16)
#define CAAM_SMAPJR0_PRTN1  CAAM_BASE_ADDR + 0x1114
#define CAAM_SMAG2JR0_PRTN1 CAAM_BASE_ADDR + 0x1118
#define CAAM_SMAG1JR0_PRTN1 CAAM_BASE_ADDR + 0x111c
#define CAAM_SMPO           CAAM_BASE_ADDR + 0x1fbc

#define JRCFG_LS_IMSK       0x00000001
#define JR_MID              2
#define KS_G1               (1 << JR_MID)
#define PERM                0x0000B008

#define CMD_PAGE_ALLOC      0x1
#define CMD_PAGE_DEALLOC    0x2
#define CMD_PART_DEALLOC    0x3
#define CMD_INQUIRY         0x5
#define PAGE(x)             (x << 16)
#define PARTITION(x)        (x << 8)

#define SMCSJR_AERR         (3 << 12)
#define SMCSJR_CERR         (3 << 14)
#define CMD_COMPLETE        (3 << 14)

#define SMCSJR_PO           (3 << 6)
#define PAGE_AVAILABLE      0
#define PAGE_OWNED          (3 << 6)

#define PARTITION_OWNER(x)  (0x3 << (x*2))

#define CAAM_BUSY_MASK      0x00000001
#define CAAM_IDLE_MASK      0x00000002
#define JOB_RING_ENTRIES    1
#define JOB_RING_STS        (0xF << 28)

#define RNG_TRIM_OSC_DIV    0
#define RNG_TRIM_ENT_DLY    3200

#define RTMCTL_PGM  (1 << 16)
#define RTMCTL_ERR  (1 << 12)
#define RDSTA_IF0   1
#define RDSTA_SKVN  (1 << 30)

#define DECAP_BLOB_DESC1 0xB0800009
#define DECAP_BLOB_DESC2 0x14C00C08
#define DECAP_BLOB_DESC3 0x00105566
#define DECAP_BLOB_DESC4 0x00000000
#define DECAP_BLOB_DESC5 0xF0000400
#define DECAP_BLOB_DESC6 0x00000000
#define DECAP_BLOB_DESC7 0xF80003d0
#define DECAP_BLOB_DESC8 SEC_MEM_PAGE1
#define DECAP_BLOB_DESC9 0x860D0008

#define ENCAP_BLOB_DESC1  0xB0800009
#define ENCAP_BLOB_DESC2 0x14C00C08
#define ENCAP_BLOB_DESC3 0x00105566
#define ENCAP_BLOB_DESC4 0x00000000
#define ENCAP_BLOB_DESC5 0xF00003d0
#define ENCAP_BLOB_DESC6 SEC_MEM_PAGE1
#define ENCAP_BLOB_DESC7  0xF8000400
#define ENCAP_BLOB_DESC8  0x00000000
#define ENCAP_BLOB_DESC9  0x870D0008

#define RNG_INST_DESC1 0xB0800009
#define RNG_INST_DESC2 0x12A00008
#define RNG_INST_DESC3 0x01020304
#define RNG_INST_DESC4 0x05060708
#define RNG_INST_DESC5 0x82500404
#define RNG_INST_DESC6 0xA2000001
#define RNG_INST_DESC7 0x10880004
#define RNG_INST_DESC8 0x00000001
#define RNG_INST_DESC9 0x82501000

#endif /* __CAAM_INTERNAL_H__ */
