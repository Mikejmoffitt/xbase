// XBase CRTC and Scroll Functions (crtc)
// (c) Michael Moffitt 2021-2026
//
// The X68000 CRTC manages a few aspects related to video, limit not only to
// video output and display timing:
//
//   * Raster configuration (video timing)
//   * TVRAM and GVRAM scroll
//   * Raster Copy
//   * Control Port
//
// Scroll updates are handled in xbase/util/scroll.h.
//
// Raster Copy and Control Port have not yet had support added.
// TODO: Read Page 224 of Inside X68000 for raster copy.
//

#pragma once

#include "xbase/macro.h"
#include "xbase/memmap.h"
#include "xbase/pcg.h"
#include "xbase/vidcon.h"

#ifndef __ASSEMBLER__
#include <stdint.h>
#endif

//
// Raster Timing Configuration Structures
//

//
// R00-R07 calculations, from Inside X68000 (Write only):
//

//
// R00 = ((Hsync period * H total dots) / (H active dots * 8)) - 1
// R01 = ((Hsync pulse * H total dots) / (H active dots * 8)) - 1
// R02 = (((Hsync pulse + H back porch) * (H total dots)) / (H active dots)) - 5
// R03 = (((Hsync period + H front porch) * (H total dots)) / (H active dots)) - 5
// R04 = (Vsync period / Hsync period) - 1
// R05 = (Vsync pulse / Hsync period) - 1
// R06 = ((Vsync pulse + V back porch) / Hsync period) - 1
// R07 = ((Vsync period - V front porch) / Hysnc period) - 1
//
// Sample config table from Inside X68000:
//
//    [       High-resolution       ] [   Low resolution    ]
//    768x512 512x512 512x256 256x256 512x512 512x256 256x256
// R00     $89     $5B     $5B     $2D     $4B     $4B     $25
// R01     $0E     $09     $09     $04     $03     $03     $01
// R02     $1C     $11     $11     $06     $05     $05     $00
// R03     $7C     $51     $51     $26     $45     $45     $20
// R04    $237    $237    $237    $237    $103    $103    $103
// R05     $05     $05     $05     $05     $02     $02     $02
// R06     $28     $28     $28     $28     $10     $10     $10
// R07    $228    $228    $228    $228    $100    $100    $100
// R08     $1B     $1B     $1B     $1B     $2C     $2C     $24
//
// In addition to the timing registers, there is R08, the horizontal adjust.
//

//
// R20: Flags (R/W)
//
// fedc ba98 7654 3210
// .... .zcc ...f vvhh
//       ||     | | \__ Horizontal disp:  00 = 256 dots
//       ||     | |                       01 = 512 dots
//       ||     | |                       10 = 768 dots
//       ||     | |                       11 = Invalid (50MHz dotclock?)
//       ||     | |
//       ||     |  \___ Vertical disp:    00 = 256 dots
//       ||     |                         01 = 512 dots
//       ||     |                         10 = invalid config (1024 vertical dots)
//       ||     |                         11 = invalid config
//       ||     |
//       ||      \_____ Frequency:        0 = 15.98KHz
//       ||                               1 = 31.5KHz (is this the dot clock?)
//       ||
//       |\____________ Color depth:      00 = 16 colors
//       |                                01 = 256 colors
//       |                                10 = invalid config
//       |                                11 = 65536 colors
//       |
//        \____________ Plane size:       0 = 512  dots
//                                        1 = 1024 dots
//
// The first bit of the horizontal display acts as an enable for a dot clock
// divider (active low). It affects crtc timings by a factor of two.
//
// The second bit of the horizontal display appears to select the dot clock.
// The faster dot clock of the 768 dot mode allows for roughly square pixels,
// but PCG is incompatible, and using both will cause an open bus error.
//
// If the vertical height is set to 256 dots while f is set to 31.5KHz, lines will be
// doubled.
//
// The frequency select also appears to active a clock divider (active low).

#define XB_CRTC_R20_HSIZE_256  (0x0<<0)
#define XB_CRTC_R20_HSIZE_512  (0x1<<0)
#define XB_CRTC_R20_HSIZE_768  (0x2<<0)
#define XB_CRTC_R20_HSIZE_NG   (0x3<<0)

#define XB_CRTC_R20_VSIZE_256  (0x0<<2)
#define XB_CRTC_R20_VSIZE_512  (0x1<<2)

#define XB_CRTC_R20_FREQ_15K   0
#define XB_CRTC_R20_FREQ_31K   XB_BITVAL(4)
// Rather than using these below, call xb_vidcon_update_crtc().
#define XB_CRTC_R20_COL_16     (0x00<<8)
#define XB_CRTC_R20_COL_256    (0x01<<8)
#define XB_CRTC_R20_COL_NG     (0x02<<8)
#define XB_CRTC_R20_COL_65536  (0x03<<8)
#define XB_CRTC_R20_SIZ_512    0
#define XB_CRTC_R20_SIZ_1024   XB_BITVAL(10)

#ifndef __ASSEMBLER__

extern uint16_t g_xb_crtc_regs[9];  // Shadow of the timing registers.

static inline volatile uint16_t *xb_get_crtc(void)
{
	return (volatile uint16_t *)XB_CRTC_BASE;
}

// Updates registers downstream of CRTC. Call after setting a mode.
static inline void xb_crtc_disseminate_regs(void)
{
	xb_vidcon_set_crtc_regs();
	xb_pcg_set_crtc_regs();
}

// Sets the horizontal scan registers, and if appropriate,
static inline void xb_crtc_set_h(uint16_t r0, uint16_t r1, uint16_t r2, uint16_t r3)
{
	volatile uint16_t *crtc = xb_get_crtc();
	g_xb_crtc_regs[0] = r0;
	g_xb_crtc_regs[1] = r1;
	g_xb_crtc_regs[2] = r2;
	g_xb_crtc_regs[3] = r3;
	for (uint16_t i = 0; i < 4; i++)
	{
		crtc[i] = g_xb_crtc_regs[i];
	}
}

// Vertical scan.
static inline void xb_crtc_set_v(uint16_t r4, uint16_t r5, uint16_t r6, uint16_t r7)
{
	volatile uint16_t *crtc = xb_get_crtc();
	g_xb_crtc_regs[4] = r4;
	g_xb_crtc_regs[5] = r5;
	g_xb_crtc_regs[6] = r6;
	g_xb_crtc_regs[7] = r7;
	for (uint16_t i = 4; i < 8; i++)
	{
		crtc[i] = g_xb_crtc_regs[i];
	}
}

// Horizontal Adjust.
static inline void xb_crtc_set_hadj(uint16_t r8)
{
	volatile uint16_t *crtc = xb_get_crtc();
	g_xb_crtc_regs[8] = r8;
	crtc[8] = g_xb_crtc_regs[8];
}

// Screen configuration.
static inline void xb_crtc_set_flags(uint16_t r20)
{
	volatile uint16_t *crtc = xb_get_crtc();
	crtc[20] = r20;
}

//
// R20 specific configuration. After updating remember to call
// xb_crtc_disseminate_regs() for the sake of downstream ICs.
//

// Set active area width. Use XB_CRTC_R20_HSIZE_*.
static inline void xb_crtc_set_hsize(uint16_t hsize)
{
	volatile uint16_t *crtc = xb_get_crtc();
	uint16_t rdata = crtc[20];
	rdata &= ~(0x03 << 0);
	rdata |= hsize;
	crtc[20] = rdata;
}

// Set active area height. Use XB_CRTC_R20_VSIZE_*.
static inline void xb_crtc_set_vsize(uint16_t vsize)
{
	volatile uint16_t *crtc = xb_get_crtc();
	uint16_t rdata = crtc[20];
	if (vsize) rdata |= XB_CRTC_R20_VSIZE_512;
	else rdata &= ~(XB_CRTC_R20_VSIZE_512);
	crtc[20] = rdata;
}

// Sets line doubler on/off. Use XB_CRTC_R20_FREQ_31K.
static inline void xb_crtc_set_freq(uint16_t freq)
{
	volatile uint16_t *crtc = xb_get_crtc();
	uint16_t rdata = crtc[20];
	if (freq) rdata |= XB_CRTC_R20_FREQ_31K;
	else rdata &= ~(XB_CRTC_R20_FREQ_31K);
	crtc[20] = rdata;
}

// Updates color mode. Use XB_CRTC_R20_COL_*
static inline void xb_crtc_set_col(uint16_t col)
{
	volatile uint16_t *crtc = xb_get_crtc();
	uint16_t rdata = crtc[20];
	rdata &= ~(0x03 << 8);
	rdata |= col;
	crtc[20] = rdata;
}

// Updates plane size.
static inline void xb_crtc_set_siz(uint16_t siz)
{
	volatile uint16_t *crtc = xb_get_crtc();
	uint16_t rdata = crtc[20];
	if (siz) rdata |= XB_CRTC_R20_SIZ_1024;
	else rdata &= ~(XB_CRTC_R20_SIZ_1024);
	crtc[20] = rdata;
}

//
// Functions
//

static inline void xb_crtc_set_raster_interrupt(uint16_t line)
{
	volatile uint16_t *crtc = xb_get_crtc();
	crtc[9] = line;
}


#endif

// TODO: Raster copy stuff (R21-R23)
