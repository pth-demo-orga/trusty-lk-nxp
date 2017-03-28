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

#ifndef _CAAM_H
#define _CAAM_H

#include <linux/types.h>

#define CAAM_SEC_RAM_SIZE	0x8000
#define CAAM_KB_HEADER_LEN	48
#define CAAM_SUCCESS		0
#define CAAM_FAILURE		1

void caam_open(void);
uint32_t caam_gen_blob(uint8_t *plain_data_addr, uint8_t *blob_addr, uint32_t size);
uint32_t caam_decap_blob(uint8_t *plain_text, uint8_t *blob_addr, uint32_t size);
uint32_t caam_aes_enc(uint8_t *input, uint8_t *output_ptr, uint32_t len_input);
uint32_t caam_aes_dec(uint8_t *input, uint8_t *output_ptr, uint32_t len_input);
uint32_t caam_hwrng(uint8_t *output_ptr, uint32_t output_len);
void install_aeskey(uint8_t *key);
void caam_test(void);

struct mxc_ccm_reg {
	u32 ccr;	/* 0x0000 */
	u32 ccdr;
	u32 csr;
	u32 ccsr;
	u32 cacrr;	/* 0x0010*/
	u32 cbcdr;
	u32 cbcmr;
	u32 cscmr1;
	u32 cscmr2;	/* 0x0020 */
	u32 cscdr1;
	u32 cs1cdr;
	u32 cs2cdr;
	u32 cdcdr;	/* 0x0030 */
	u32 chsccdr;
	u32 cscdr2;
	u32 cscdr3;
	u32 cscdr4;	/* 0x0040 */
	u32 resv0;
	u32 cdhipr;
	u32 cdcr;
	u32 ctor;	/* 0x0050 */
	u32 clpcr;
	u32 cisr;
	u32 cimr;
	u32 ccosr;	/* 0x0060 */
	u32 cgpr;
	u32 CCGR0;
	u32 CCGR1;
	u32 CCGR2;	/* 0x0070 */
	u32 CCGR3;
	u32 CCGR4;
	u32 CCGR5;
	u32 CCGR6;	/* 0x0080 */
	u32 CCGR7;
	u32 cmeor;
	u32 resv[0xfdd];
	u32 analog_pll_sys;			/* 0x4000 */
	u32 analog_pll_sys_set;
	u32 analog_pll_sys_clr;
	u32 analog_pll_sys_tog;
	u32 analog_usb1_pll_480_ctrl;		/* 0x4010 */
	u32 analog_usb1_pll_480_ctrl_set;
	u32 analog_usb1_pll_480_ctrl_clr;
	u32 analog_usb1_pll_480_ctrl_tog;
	u32 analog_usb2_pll_480_ctrl;		/* 0x4020 */
	u32 analog_usb2_pll_480_ctrl_set;
	u32 analog_usb2_pll_480_ctrl_clr;
	u32 analog_usb2_pll_480_ctrl_tog;
	u32 analog_pll_528;			/* 0x4030 */
	u32 analog_pll_528_set;
	u32 analog_pll_528_clr;
	u32 analog_pll_528_tog;
	u32 analog_pll_528_ss;			/* 0x4040 */
	u32 analog_reserved1[3];
	u32 analog_pll_528_num;			/* 0x4050 */
	u32 analog_reserved2[3];
	u32 analog_pll_528_denom;		/* 0x4060 */
	u32 analog_reserved3[3];
	u32 analog_pll_audio;			/* 0x4070 */
	u32 analog_pll_audio_set;
	u32 analog_pll_audio_clr;
	u32 analog_pll_audio_tog;
	u32 analog_pll_audio_num;		/* 0x4080*/
	u32 analog_reserved4[3];
	u32 analog_pll_audio_denom;		/* 0x4090 */
	u32 analog_reserved5[3];
	u32 analog_pll_video;			/* 0x40a0 */
	u32 analog_pll_video_set;
	u32 analog_pll_video_clr;
	u32 analog_pll_video_tog;
	u32 analog_pll_video_num;		/* 0x40b0 */
	u32 analog_reserved6[3];
	u32 analog_pll_video_denom;		/* 0x40c0 */
	u32 analog_reserved7[3];
	u32 analog_pll_mlb;		        /* 0x40d0 */
	u32 analog_pll_mlb_set;
	u32 analog_pll_mlb_clr;
	u32 analog_pll_mlb_tog;
	u32 analog_pll_enet;			/* 0x40e0 */
	u32 analog_pll_enet_set;
	u32 analog_pll_enet_clr;
	u32 analog_pll_enet_tog;
	u32 analog_pfd_480;			/* 0x40f0 */
	u32 analog_pfd_480_set;
	u32 analog_pfd_480_clr;
	u32 analog_pfd_480_tog;
	u32 analog_pfd_528;			/* 0x4100 */
	u32 analog_pfd_528_set;
	u32 analog_pfd_528_clr;
	u32 analog_pfd_528_tog;
	u32 reg_1p1;			/* 0x4110 */
	u32 reg_1p1_set;		/* 0x4114 */
	u32 reg_1p1_clr;		/* 0x4118 */
	u32 reg_1p1_tog;		/* 0x411c */
	u32 reg_3p0;			/* 0x4120 */
	u32 reg_3p0_set;		/* 0x4124 */
	u32 reg_3p0_clr;		/* 0x4128 */
	u32 reg_3p0_tog;		/* 0x412c */
	u32 reg_2p5;			/* 0x4130 */
	u32 reg_2p5_set;		/* 0x4134 */
	u32 reg_2p5_clr;		/* 0x4138 */
	u32 reg_2p5_tog;		/* 0x413c */
	u32 reg_core;			/* 0x4140 */
	u32 reg_core_set;		/* 0x4144 */
	u32 reg_core_clr;		/* 0x4148 */
	u32 reg_core_tog;		/* 0x414c */
	u32 ana_misc0;			/* 0x4150 */
	u32 ana_misc0_set;		/* 0x4154 */
	u32 ana_misc0_clr;		/* 0x4158 */
	u32 ana_misc0_tog;		/* 0x415c */
	u32 ana_misc1;			/* 0x4160 */
	u32 ana_misc1_set;		/* 0x4164 */
	u32 ana_misc1_clr;		/* 0x4168 */
	u32 ana_misc1_tog;		/* 0x416c */
	u32 ana_misc2;			/* 0x4170 */
	u32 ana_misc2_set;		/* 0x4174 */
	u32 ana_misc2_clr;		/* 0x4178 */
	u32 ana_misc2_tog;		/* 0x417c */
	u32 tempsense0;			/* 0x4180 */
	u32 tempsense0_set;		/* 0x4184 */
	u32 tempsense0_clr;		/* 0x4188 */
	u32 tempsense0_tog;		/* 0x418c */
	u32 tempsense1;			/* 0x4190 */
	u32 tempsense1_set;		/* 0x4194 */
	u32 tempsense1_clr;		/* 0x4198 */
	u32 tempsense1_tog;		/* 0x419c */
	u32 usb1_vbus_detect;		/* 0x41a0 */
	u32 usb1_vbus_detect_set;	/* 0x41a4 */
	u32 usb1_vbus_detect_clr;	/* 0x41a8 */
	u32 usb1_vbus_detect_tog;	/* 0x41ac */
	u32 usb1_chrg_detect;		/* 0x41b0 */
	u32 usb1_chrg_detect_set;	/* 0x41b4 */
	u32 usb1_chrg_detect_clr;	/* 0x41b8 */
	u32 usb1_chrg_detect_tog;	/* 0x41bc */
	u32 usb1_vbus_det_stat;		/* 0x41c0 */
	u32 usb1_vbus_det_stat_set;	/* 0x41c4 */
	u32 usb1_vbus_det_stat_clr;	/* 0x41c8 */
	u32 usb1_vbus_det_stat_tog;	/* 0x41cc */
	u32 usb1_chrg_det_stat;		/* 0x41d0 */
	u32 usb1_chrg_det_stat_set;	/* 0x41d4 */
	u32 usb1_chrg_det_stat_clr;	/* 0x41d8 */
	u32 usb1_chrg_det_stat_tog;	/* 0x41dc */
	u32 usb1_loopback;		/* 0x41e0 */
	u32 usb1_loopback_set;		/* 0x41e4 */
	u32 usb1_loopback_clr;		/* 0x41e8 */
	u32 usb1_loopback_tog;		/* 0x41ec */
	u32 usb1_misc;			/* 0x41f0 */
	u32 usb1_misc_set;		/* 0x41f4 */
	u32 usb1_misc_clr;		/* 0x41f8 */
	u32 usb1_misc_tog;		/* 0x41fc */
	u32 usb2_vbus_detect;		/* 0x4200 */
	u32 usb2_vbus_detect_set;	/* 0x4204 */
	u32 usb2_vbus_detect_clr;	/* 0x4208 */
	u32 usb2_vbus_detect_tog;	/* 0x420c */
	u32 usb2_chrg_detect;		/* 0x4210 */
	u32 usb2_chrg_detect_set;	/* 0x4214 */
	u32 usb2_chrg_detect_clr;	/* 0x4218 */
	u32 usb2_chrg_detect_tog;	/* 0x421c */
	u32 usb2_vbus_det_stat;		/* 0x4220 */
	u32 usb2_vbus_det_stat_set;	/* 0x4224 */
	u32 usb2_vbus_det_stat_clr;	/* 0x4228 */
	u32 usb2_vbus_det_stat_tog;	/* 0x422c */
	u32 usb2_chrg_det_stat;		/* 0x4230 */
	u32 usb2_chrg_det_stat_set;	/* 0x4234 */
	u32 usb2_chrg_det_stat_clr;	/* 0x4238 */
	u32 usb2_chrg_det_stat_tog;	/* 0x423c */
	u32 usb2_loopback;		/* 0x4240 */
	u32 usb2_loopback_set;		/* 0x4244 */
	u32 usb2_loopback_clr;		/* 0x4248 */
	u32 usb2_loopback_tog;		/* 0x424c */
	u32 usb2_misc;			/* 0x4250 */
	u32 usb2_misc_set;		/* 0x4254 */
	u32 usb2_misc_clr;		/* 0x4258 */
	u32 usb2_misc_tog;		/* 0x425c */
	u32 digprog;			/* 0x4260 */
	u32 reserved1[7];
	u32 digprog_sololite;		/* 0x4280 */
};

#endif
