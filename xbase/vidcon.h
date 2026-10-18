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

#define XB_VIDCON_R2_ALL_ON    (XB_VIDCON_R2_BON | \
                                XB_VIDCON_R2_SON | \
                                XB_VIDCON_R2_TON | \
                                XB_VIDCON_R2_GS4 | \
                                XB_VIDCON_R2_GS3 | \
                                XB_VIDCON_R2_GS2 | \
                                XB_VIDCON_R2_GS1 | \
                                XB_VIDCON_R2_GS0)

#ifdef __ASSEMBLER__
	.struct 0
XBVidconCfg.screen:	ds.w 1
XBVidconCfg.prio:	ds.w 1
XBVidconCfg.flags:	ds.w 1
XBVidconCfg.len:

#else
typedef struct XBVidconCfg
{
	uint16_t screen;  // R0
	uint16_t prio;    // R1
	uint16_t flags;   // R2
} XBVidconCfg;
#endif

#ifdef __ASSEMBLER__
	.extern	g_xb_vidcon_cfg
	.global	xb_vidcon_set
	.global	xb_vidcon_update_regs

#else

extern XBVidconCfg g_xb_vidcon_cfg;

static inline void xb_vidcon_set(const XBVidconCfg *c)
{
	g_xb_vidcon_cfg = *c;
}

static inline uint16_t xb_vidcon_get_crtc_r20(void)
{
	return (g_xb_vidcon_cfg.screen & 0x7) << 8;
}

void xb_vidcon_update_regs(void);

#endif
