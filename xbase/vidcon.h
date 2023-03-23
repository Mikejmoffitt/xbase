/*

X68000 Video Controller Helper Functions (vidcon)
c. Michael Moffitt 2017-2022

Provides some helper functions for configuring the video controller. The
configuration is stored in three words of shadow copies of the registers,
which are initially set with xb_vidcon_init_default().

Afterwards, individual bits may be set or cleared with the clearly named
helper functions. These functions modify the shadow copies. To write the shadow
copies to the actual hardware registers, call xb_vidcon_commit_regs(). Ideally,
the register commit occurs during video blank.

*/

#ifndef XB_VIDCON_H
#define XB_VIDCON_H

#include <stdint.h>
#include "xbase/memmap.h"

// RGB palette entry macro
#define PAL_RGB5(r, g, b) ( (((r) & 0x1F) << 6) | (((g) & 0x1F) << 11) | (((b) & 0x1F) << 1) )
#define PAL_RGB4(r, g, b) ( (((r << 1) & 0x1F) << 6) | (((g << 1) & 0x1F) << 11) | (((b << 1) & 0x1F) << 1) )
#define PAL_RGB8(r, g, b) ( (((r >> 3) & 0x1F) << 6) | (((g >> 3) & 0x1F) << 11) | (((b >> 3) & 0x1F) << 1) )

/*

Example config:

screen = 0x0000;  // 512x512, 16-color
prio = 0x12E4;  // Text > PCG > (GP0 > GP1 > GP2 > GP3)
flags = 0x007F;  // Enable all layers.

*/

// TODO: Enums for screen, prio, and flag registers.

typedef struct XBVidconCfg
{
	// R0: Screen
	// Bits 0-1: color depth (0 = 16, 1 = 256, 2 = invalid, 3 = 65536)
	// Bit 2: "Real screen mode" 0 = 512x512, 1 = 1024x1024
	// This should match CRTC flags / R20 bits 8-10 shifted down to bits 0-2.
	uint16_t screen;

	// R1: Priority
	// Ordering between layer groups, and between the graphics planes within
	// the graphics plane group.
	// 0 = topmost; 3 = bottom-most.
	// 15 -------------- 0
	// ..PP .... .... .... PCG priority
	// .... TT.. .... .... Text priority
	// .... ..GG .... .... GP priority
	// .... .... 33.. ....  --> GP3 priority
	// .... .... ..22 ....  --> GP2 priority
	// .... .... .... 11..  --> GP1 priority
	// .... .... .... ..00  --> GP0 priority
	uint16_t prio;

	// R2: Flags
	// 15 -------------- 0
	// V... .... .... .... Video cut / don't show image even when superimposing
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
	uint16_t flags;
} XBVidconCfg;

void xb_vidcon_init(const XBVidconCfg *c);

// Graphics plane palette entries
static inline void xb_vidcon_set_gp_color(uint16_t index, uint16_t val)
{
	volatile uint16_t *p = (volatile uint16_t *)XB_VIDCON_GP_PAL_BASE;
	p += index;
	*p = val;
}

// Text plane color entries
static inline void xb_vidcon_set_text_color(uint16_t index, uint16_t val)
{
	volatile uint16_t *p = (volatile uint16_t *)XB_VIDCON_TX_PAL_BASE;
	p += index;
	*p = val;
}

// Sprite palette entries
static inline void xb_vidcon_set_pcg_color(uint16_t index, uint16_t val)
{
	volatile uint16_t *p = (volatile uint16_t *)XB_VIDCON_PCG_PAL_BASE;
	p += index;
	*p = val;
}

static inline void xb_vidcon_set_gp_pal(uint16_t palno, const uint16_t *src)
{
	volatile uint16_t *p = (volatile uint16_t *)XB_VIDCON_GP_PAL_BASE;
	p = &p[palno * 16];
	for (uint16_t i = 0; i < 16; i++)
	{
		p[i] = src[i];
	}
}

static inline void xb_vidcon_set_text_pal(const uint16_t *src)
{
	volatile uint16_t *p = (volatile uint16_t *)XB_VIDCON_TX_PAL_BASE;
	for (uint16_t i = 0; i < 16; i++)
	{
		p[i] = src[i];
	}
}

static inline void xb_vidcon_set_pcg_pal(uint16_t palno, const uint16_t *src)
{
	volatile uint16_t *p = (volatile uint16_t *)XB_VIDCON_PCG_PAL_BASE;
	p = &p[palno * 16];
	for (uint16_t i = 0; i < 16; i++)
	{
		p[i] = src[i];
	}
}

#endif // X68K_VIDCON_H
