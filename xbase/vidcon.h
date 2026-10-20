// XBase Video Controller Helper Functions (vidcon)
// (c) Michael Moffitt 2017-2024
//
// Provides some helper functions for configuring the video controller. The
// configuration is stored in three words of shadow copies of the registers,
// which are initially set with xb_vidcon_set_default().
//
// Afterwards, individual bits may be set or cleared with the clearly named
// helper functions. These functions modify the shadow copies. To write the shadow
// copies to the actual hardware registers, call xb_vidcon_commit_regs(). Ideally,
// the register commit occurs during video blank.
#pragma once 

#include "xbase/macro.h"
#include "xbase/memmap.h"
#include "xbase/crtc.h"

#ifndef __ASSEMBLER__
#include <stdint.h>
#endif

//
// R0: Screen Config
//
// Bits 0-1: color depth (0 = 16, 1 = 256, 2 = invalid, 3 = 65536)
// Bit 2: "Real screen mode" 0 = 512x512, 1 = 1024x1024
// This should match CRTC flags / R20 bits 8-10 shifted down to bits 0-2.

#define XB_VIDCON_R0_COL_16    (0)
#define XB_VIDCON_R0_COL_256   (1)
#define XB_VIDCON_R0_COL_65536 (3)
#define XB_VIDCON_R0_SIZ       XB_BITVAL(2)

//
// R1: Priority
//
// Ordering between layer groups, and between the graphics planes within
// the graphics plane group.
//
// 0 = topmost; 3 = bottom-most.
//
// fedc ba98 7654 3210
// ..SS .... .... .... SP priority
// .... TT.. .... .... TX priority
// .... ..GG .... .... GR priority
// .... .... 33.. ....  --> GP3 priority
// .... .... ..22 ....  --> GP2 priority
// .... .... .... 11..  --> GP1 priority
// .... .... .... ..00  --> GP0 priority
//
// Note: When in two-page mode (256 colors) or one-page mode (62256 colors) the
// page ordering must still be configured.
// For one-page mode, set the priorities in order with the pages (e.g. GP0=0)
// For two-page mode, set GP0/GP1 and GP2/GP3 consecutively as pairs.

#define XB_VIDCON_PRI3 0
#define XB_VIDCON_PRI2 1
#define XB_VIDCON_PRI1 2
#define XB_VIDCON_PRI0 3

#define XB_VIDCON_PRIO_HI XB_VIDCON_PRI3
#define XB_VIDCON_PRIO_LO XB_VIDCON_PRI0
// Source ordering
#define XB_VIDCON_R1_PRIO_SP(x) ((x)<<0xC)
#define XB_VIDCON_R1_PRIO_TX(x) ((x)<<0xA)
#define XB_VIDCON_R1_PRIO_GR(x) ((x)<<0x8)
// GP Ordering for the four GP pages.
#define XB_VIDCON_R1_PRIO_GP3(x) ((x)<<0x6)
#define XB_VIDCON_R1_PRIO_GP2(x) ((x)<<0x4)
#define XB_VIDCON_R1_PRIO_GP1(x) ((x)<<0x2)
#define XB_VIDCON_R1_PRIO_GP0(x) ((x)<<0x0)

//
// R2: Flags
//
// fedc ba98 7654 3210
// Y... .... .... .... YS: Video cut, maybe related to super-impose function.
// .A.. .... .... .... Ah: translucent text palette 0 regardless of exon ?
// ..v. .... .... .... Vht: Video image (used in color image unit)
// ...E .... .... .... Exon: Enable special priority / translucent mode
// .... H... .... .... Hp: special priority (0) or translucent (1) select
// .... .B.. .... .... Bp: Sharp reserved (0) or something else (1) ???
// .... ..G. .... .... GG: Set translucent: graphics screen
// .... ...T .... .... GT: Set translucent: text/sprite screen
// .... .... b... .... border: enable border color display
// .... .... .P.. .... Enable PCG display
// .... .... ..t. .... Enable Text display
// .... .... ...g .... Enable GP display
// .... .... .... 3... GP3 enable
// .... .... .... .2.. GP2 enable
// .... .... .... ..1. GP1 enable
// .... .... .... ...0 GP0 enable

// See Inside X68000 page 209 for information on the mixing and impact of R2
// bits $C, $9, and $8.

// Transparency / etc settings (upper byte)
#define XB_VIDCON_R2_YS        XB_BITVAL(0xF)
#define XB_VIDCON_R2_AH        XB_BITVAL(0xE)
#define XB_VIDCON_R2_VHT       XB_BITVAL(0xD)
#define XB_VIDCON_R2_EXON      XB_BITVAL(0xC)
#define XB_VIDCON_R2_H_P       XB_BITVAL(0xB)
#define XB_VIDCON_R2_B_P       XB_BITVAL(0xA)
#define XB_VIDCON_R2_G_G       XB_BITVAL(0x9)
#define XB_VIDCON_R2_G_T       XB_BITVAL(0x8)
// Layer enablement (lower byte)
#define XB_VIDCON_R2_BON       XB_BITVAL(0x7)
#define XB_VIDCON_R2_SON       XB_BITVAL(0x6)
#define XB_VIDCON_R2_TON       XB_BITVAL(0x5)
#define XB_VIDCON_R2_GS4       XB_BITVAL(0x4)
#define XB_VIDCON_R2_GS3       XB_BITVAL(0x3)
#define XB_VIDCON_R2_GS2       XB_BITVAL(0x2)
#define XB_VIDCON_R2_GS1       XB_BITVAL(0x1)
#define XB_VIDCON_R2_GS0       XB_BITVAL(0x0)

#define XB_VIDCON_R2_ALL_ON    (XB_VIDCON_R2_SON | \
                                XB_VIDCON_R2_TON | \
                                XB_VIDCON_R2_GS4 | \
                                XB_VIDCON_R2_GS3 | \
                                XB_VIDCON_R2_GS2 | \
                                XB_VIDCON_R2_GS1 | \
                                XB_VIDCON_R2_GS0)

#ifndef __ASSEMBLER__

static inline volatile uint16_t *xb_get_vidcon(void)
{
	return (volatile uint16_t *)XB_VIDCON_R0;
}

// Set all regs at once.
static inline void xb_vidcon_set(const uint16_t *r)
{
	*(volatile uint16_t *)XB_VIDCON_R0 = r[0];
	*(volatile uint16_t *)XB_VIDCON_R1 = r[1];
	*(volatile uint16_t *)XB_VIDCON_R2 = r[2];
}

// Updates R0 based on CRTC config.
static inline void xb_vidcon_set_crtc_regs(void)
{
	volatile uint16_t *crtc_regs = (volatile uint16_t *)XB_CRTC_BASE;
	const uint16_t mode = crtc_regs[20];

	*(volatile uint16_t *)XB_VIDCON_R0 = mode >> 8;
}

// R1 : Priority
static inline void xb_vidcon_set_sp_prio(uint16_t prio)
{
	uint16_t rdata = *(volatile uint16_t *)XB_VIDCON_R1;
	rdata &= ~(XB_VIDCON_R1_PRIO_SP(3));
	rdata |= XB_VIDCON_R1_PRIO_SP(prio);
	*(volatile uint16_t *)XB_VIDCON_R1 = rdata;
}

static inline void xb_vidcon_set_tx_prio(uint16_t prio)
{
	uint16_t rdata = *(volatile uint16_t *)XB_VIDCON_R1;
	rdata &= ~(XB_VIDCON_R1_PRIO_TX(3));
	rdata |= XB_VIDCON_R1_PRIO_TX(prio);
	*(volatile uint16_t *)XB_VIDCON_R1 = rdata;
}

static inline void xb_vidcon_set_gr_prio(uint16_t prio)
{
	uint16_t rdata = *(volatile uint16_t *)XB_VIDCON_R1;
	rdata &= ~(XB_VIDCON_R1_PRIO_GR(3));
	rdata |= XB_VIDCON_R1_PRIO_GR(prio);
	*(volatile uint16_t *)XB_VIDCON_R1 = rdata;
}

static inline void xb_vidcon_set_gp_prio(uint16_t layer, uint16_t prio)
{
	uint16_t rdata = *(volatile uint16_t *)XB_VIDCON_R1;
	switch (layer)
	{
		case 0:
			rdata &= ~(XB_VIDCON_R1_PRIO_GP0(3));
			rdata |= XB_VIDCON_R1_PRIO_GP0(prio);
			break;

		case 1:
			rdata &= ~(XB_VIDCON_R1_PRIO_GP1(3));
			rdata |= XB_VIDCON_R1_PRIO_GP1(prio);
			break;

		case 2:
			rdata &= ~(XB_VIDCON_R1_PRIO_GP2(3));
			rdata |= XB_VIDCON_R1_PRIO_GP2(prio);
			break;

		case 3:
			rdata &= ~(XB_VIDCON_R1_PRIO_GP3(3));
			rdata |= XB_VIDCON_R1_PRIO_GP3(prio);
			break;

		default:
			break;
	}
	*(volatile uint16_t *)XB_VIDCON_R1 = rdata;
}

// R2

static inline void xb_vidcon_set_layers(uint16_t layer_bits)
{
	uint16_t rdata = *(volatile uint16_t *)XB_VIDCON_R2;
	rdata &= 0xFF00;
	rdata |= layer_bits;
	*(volatile uint16_t *)XB_VIDCON_R2 = rdata;
}

static inline void xb_vidcon_set_ys(bool val)
{
	const uint16_t reg_mask = XB_VIDCON_R2_YS;
	if (val) *(volatile uint16_t *)XB_VIDCON_R2 |= reg_mask;
	else *(volatile uint16_t *)XB_VIDCON_R2 &= ~(reg_mask);
}

static inline void xb_vidcon_set_ah(bool val)
{
	const uint16_t reg_mask = XB_VIDCON_R2_AH;
	if (val) *(volatile uint16_t *)XB_VIDCON_R2 |= reg_mask;
	else *(volatile uint16_t *)XB_VIDCON_R2 &= ~(reg_mask);
}

static inline void xb_vidcon_set_vht(bool val)
{
	const uint16_t reg_mask = XB_VIDCON_R2_VHT;
	if (val) *(volatile uint16_t *)XB_VIDCON_R2 |= reg_mask;
	else *(volatile uint16_t *)XB_VIDCON_R2 &= ~(reg_mask);
}

static inline void xb_vidcon_set_exon(bool val)
{
	const uint16_t reg_mask = XB_VIDCON_R2_EXON;
	if (val) *(volatile uint16_t *)XB_VIDCON_R2 |= reg_mask;
	else *(volatile uint16_t *)XB_VIDCON_R2 &= ~(reg_mask);
}

static inline void xb_vidcon_set_h_p(bool val)
{
	const uint16_t reg_mask = XB_VIDCON_R2_H_P;
	if (val) *(volatile uint16_t *)XB_VIDCON_R2 |= reg_mask;
	else *(volatile uint16_t *)XB_VIDCON_R2 &= ~(reg_mask);
}

static inline void xb_vidcon_set_b_p(bool val)
{
	const uint16_t reg_mask = XB_VIDCON_R2_B_P;
	if (val) *(volatile uint16_t *)XB_VIDCON_R2 |= reg_mask;
	else *(volatile uint16_t *)XB_VIDCON_R2 &= ~(reg_mask);
}

static inline void xb_vidcon_set_g_g(bool val)
{
	const uint16_t reg_mask = XB_VIDCON_R2_G_G;
	if (val) *(volatile uint16_t *)XB_VIDCON_R2 |= reg_mask;
	else *(volatile uint16_t *)XB_VIDCON_R2 &= ~(reg_mask);
}

static inline void xb_vidcon_set_g_t(bool val)
{
	const uint16_t reg_mask = XB_VIDCON_R2_G_T;
	if (val) *(volatile uint16_t *)XB_VIDCON_R2 |= reg_mask;
	else *(volatile uint16_t *)XB_VIDCON_R2 &= ~(reg_mask);
}

static inline void xb_vidcon_set_bon(bool val)
{
	const uint16_t reg_mask = XB_VIDCON_R2_BON;
	if (val) *(volatile uint16_t *)XB_VIDCON_R2 |= reg_mask;
	else *(volatile uint16_t *)XB_VIDCON_R2 &= ~(reg_mask);
}

static inline void xb_vidcon_set_son(bool val)
{
	const uint16_t reg_mask = XB_VIDCON_R2_SON;
	if (val) *(volatile uint16_t *)XB_VIDCON_R2 |= reg_mask;
	else *(volatile uint16_t *)XB_VIDCON_R2 &= ~(reg_mask);
}

static inline void xb_vidcon_set_ton(bool val)
{
	const uint16_t reg_mask = XB_VIDCON_R2_TON;
	if (val) *(volatile uint16_t *)XB_VIDCON_R2 |= reg_mask;
	else *(volatile uint16_t *)XB_VIDCON_R2 &= ~(reg_mask);
}

static inline void xb_vidcon_set_gs4(bool val)
{
	const uint16_t reg_mask = XB_VIDCON_R2_GS4;
	if (val) *(volatile uint16_t *)XB_VIDCON_R2 |= reg_mask;
	else *(volatile uint16_t *)XB_VIDCON_R2 &= ~(reg_mask);
}

static inline void xb_vidcon_set_gs3(bool val)
{
	const uint16_t reg_mask = XB_VIDCON_R2_GS3;
	if (val) *(volatile uint16_t *)XB_VIDCON_R2 |= reg_mask;
	else *(volatile uint16_t *)XB_VIDCON_R2 &= ~(reg_mask);
}

static inline void xb_vidcon_set_gs2(bool val)
{
	const uint16_t reg_mask = XB_VIDCON_R2_GS2;
	if (val) *(volatile uint16_t *)XB_VIDCON_R2 |= reg_mask;
	else *(volatile uint16_t *)XB_VIDCON_R2 &= ~(reg_mask);
}

static inline void xb_vidcon_set_gs1(bool val)
{
	const uint16_t reg_mask = XB_VIDCON_R2_GS1;
	if (val) *(volatile uint16_t *)XB_VIDCON_R2 |= reg_mask;
	else *(volatile uint16_t *)XB_VIDCON_R2 &= ~(reg_mask);
}

static inline void xb_vidcon_set_gs0(bool val)
{
	const uint16_t reg_mask = XB_VIDCON_R2_GS0;
	if (val) *(volatile uint16_t *)XB_VIDCON_R2 |= reg_mask;
	else *(volatile uint16_t *)XB_VIDCON_R2 &= ~(reg_mask);
}

#endif
