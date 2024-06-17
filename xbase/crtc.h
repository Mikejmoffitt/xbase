// XBase CRTC and Scroll Functions (crtc)
// (c) Michael Moffitt 2021-2024
//
// The X68000 CRTC manages a few aspects related to video, limit not only to
// video output and display timing:
//   * Raster configuration (video timing)
//   * TVRAM and GVRAM scroll
//   * Raster Copy
//   * Control Port
//
// Display timing is configured by populating an XBCrtcTimingCfg struct and
// passing it to xb_crtc_set_timing(). All registers are set at once.
//
// It is acceptable to call xb_crtc_set_timing() more than once in order to
// support multiple configurations.
//
// C Example:
//  void set_crtc(void)
//  {
//      XBCrtcTimingCfg crtc_config = { ... }.
//      xb_crtc_set_timing(&crtc_config);
//  }
//
// ASM Example:
//
//  .text
//  set_crtc:
//          pea     timing_cfg
//          jsr     xb_crtc_set_timing
//          addq.l  #4, sp
//          rts
//  .data
//
//  timing_cfg:
//          ds.b bCrtcTimingCfg.len
//
// TVRAM and GVRAM scroll planes' scroll is controlled by the CRTC.
// Edit values in the global scroll configuration array, g_xb_crtc_scroll, and
// then call xb_crtc_set_scroll() to commit values to registers. This allows
// all registers to be updated in one pass. It is suggested that this is done
// during the vertical blanking interval.
//
// Please see XBCrtcScrollConfig for the fields available.
//
// Raster Copy and Control Port have not yet had support added.
// TODO: Read Page 224 of Inside X68000 for raster copy.
//
// Raster Notes:
//
// Calculations for R00 - R07, translated from InsideX68000:
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
// R20 (0xE80028) contains some timing and color flags:
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

#pragma once

#ifndef __ASSEMBLER__
#include <stdint.h>
#endif

//
// Raster Timing Configuration Structures
//

#ifdef __ASSEMBLER__
	.struct 0
XBCrtcTimingCfg.htotal:			ds.w 1
XBCrtcTimingCfg.hsync_length:	ds.w 1
XBCrtcTimingCfg.hdisp_start:	ds.w 1
XBCrtcTimingCfg.hdisp_end:		ds.w 1
XBCrtcTimingCfg.vtotal:			ds.w 1
XBCrtcTimingCfg.vsync_length:	ds.w 1
XBCrtcTimingCfg.vdisp_start:	ds.w 1
XBCrtcTimingCfg.vdisp_end:		ds.w 1
XBCrtcTimingCfg.ext_h_adjust:	ds.w 1
XBCrtcTimingCfg.flags:			ds.w 1
XBCrtcTimingCfg.len:
#else
typedef struct XBCrtcTimingCfg
{
	// R00 - R08 at 0xE80000
	uint16_t htotal;        // 1 bit = 8px
	uint16_t hsync_length;
	uint16_t hdisp_start;
	uint16_t hdisp_end;
	uint16_t vtotal;        // 1 bit = 1 line
	uint16_t vsync_length;  // "
	uint16_t vdisp_start;   // "
	uint16_t vdisp_end;     // "
	uint16_t ext_h_adjust;
	// R09 - R19 are not represented here (h-int, scroll registers).
	// R20 at 0xE80028. Bits 8-10 (color, plane size) should match the video
	// controller's top 3 bits in the "screen" field (0xE82400).
	uint16_t flags;
} XBCrtcTimingCfg;
#endif

//
// Scroll Configuration Structures
//

#define XB_CRTC_SCROLL_TEXT_X 0
#define XB_CRTC_SCROLL_TEXT_Y 1
#define XB_CRTC_SCROLL_GP0_X 2
#define XB_CRTC_SCROLL_GP0_Y 3
#define XB_CRTC_SCROLL_GP1_X 4
#define XB_CRTC_SCROLL_GP1_Y 5
#define XB_CRTC_SCROLL_GP2_X 6
#define XB_CRTC_SCROLL_GP2_Y 7
#define XB_CRTC_SCROLL_GP3_X 8
#define XB_CRTC_SCROLL_GP3_Y 9

#ifdef __ASSEMBLER__

// Indexed as x, y
XBCrtcScrollCfg.text:		ds.w 2
XBCrtcScrollCfg.gp0:		ds.w 2
XBCrtcScrollCfg.gp1:		ds.w 2
XBCrtcScrollCfg.gp2:		ds.w 2
XBCrtcScrollCfg.gp3:		ds.w 2
XBCrtcScrollCfg.len:

	.extern	g_xb_crtc_scroll
#else
typedef struct XBCrtcScrollCfg
{
	// { x, y }
	uint16_t text[2];   // TVRAM
	uint16_t gp0[2];  // GVRAM0
	uint16_t gp1[2];  // GVRAM1
	uint16_t gp2[2];  // GVRAM2
	uint16_t gp3[2];  // GVRAM3
} XBCrtcScrollCfg;

extern XBCrtcScrollCfg g_xb_crtc_scroll;
#endif

//
// Functions
//
#ifndef __ASSEMBLER__
void xb_crtc_set_timing(const XBCrtcTimingCfg *c);
void xb_crtc_set_raster_interrupt(uint16_t v);
void xb_crtc_set_scroll(void);  // Update register values.
// TODO: Raster copy stuff (R21-R23)
void xb_crtc_set_control(uint8_t v);
#endif
