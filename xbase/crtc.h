/*

XBase CRTC and Scroll Functions (crtc)
c. Michael Moffitt 2021-2022

The X68000 CRTC controls several things, which are different enough to warrant
separate access functions and structures.

Display Timing
--------------

In a CRTC, display timings are not typically changed in real-time, and all of
it is set at once. Accordingly, the timings are set in a configuration struct,
which is passed in to xb_crtc_set_timing(). It is OK to call this multiple times to
support multiple configurations.

Example:

    XBCrtcTimingCfg crtc_config = { ... }.
    xb_crtc_set_timing(&crtc_config);

Scroll Registers
----------------

The scroll registers for the TVRAM and GVRAM planes live in the CRTC register
range, so they are controlled here as well.

Scroll values are written immediately to the registers, so take care to write
to them during vertical to avoid tearing (unless that is what you want).

A struct has been provided that allows for copying all registers in mass, and
may be used instead of calling each scroll function separately. It is
recommended that you store your scroll values in one of these structs, and
call the mass update function during vertical blank.

Example:

    static XBCrtcScrollCfg s_scroll_config;

    // At VBlank:
    xb_crtc_set_scroll(&s_scroll_config);

Raster Copy
-----------

TODO: I haven't looked at how this works yet.
Page 224 of Inside has a good example of the raster copy


Control Port
------------

TODO: Document.

*/
#ifndef XBASE_CRTC_H
#define XBASE_CRTC_H

#include <stdint.h>

/*

Calculations for R00 - R07, translated from InsideX68000:

R00 = ((Hsync period * H total dots) / (H active dots * 8)) - 1
R01 = ((Hsync pulse * H total dots) / (H active dots * 8)) - 1
R02 = (((Hsync pulse + H back porch) * (H total dots)) / (H active dots)) - 5
R03 = (((Hsync period + H front porch) * (H total dots)) / (H active dots)) - 5
R04 = (Vsync period / Hsync period) - 1
R05 = (Vsync pulse / Hsync period) - 1
R06 = ((Vsync pulse + V back porch) / Hsync period) - 1
R07 = ((Vsync period - V front porch) / Hysnc period) - 1

Sample config table from Inside X68000:

    [       High-resolution       ] [   Low resolution    ]
    768x512 512x512 512x256 256x256 512x512 512x256 256x256
R00     $89     $5B     $5B     $2D     $4B     $4B     $25
R01     $0E     $09     $09     $04     $03     $03     $01
R02     $1C     $11     $11     $06     $05     $05     $00
R03     $7C     $51     $51     $26     $45     $45     $20
R04    $237    $237    $237    $237    $103    $103    $103
R05     $05     $05     $05     $05     $02     $02     $02
R06     $28     $28     $28     $28     $10     $10     $10
R07    $228    $228    $228    $228    $100    $100    $100
R08     $1B     $1B     $1B     $1B     $2C     $2C     $24

*/

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
	/*
	15                0
	.... .zcc ...f vvhh
	      ||     | | \__ Horizontal disp:  00 = 256 dots
	      ||     | |                       01 = 512 dots
	      ||     | |                       10 = 768 dots
	      ||     | |                       11 = Invalid (50MHz dotclock?)
	      ||     | |
	      ||     |  \___ Vertical disp:    00 = 256 dots
	      ||     |                         01 = 512 dots
	      ||     |                         10 = invalid config (1024 vertical dots)
	      ||     |                         11 = invalid config
	      ||     |
	      ||      \_____ Frequency:        0 = 15.98KHz
	      ||                               1 = 31.5KHz (is this the dot clock?)
	      ||
	      |\____________ Color depth:      00 = 16 colors
	      |                                01 = 256 colors
	      |                                10 = invalid config
	      |                                11 = 65536 colors
	      |
	       \____________ Plane size:       0 = 512  dots
	                                       1 = 1024 dots

	The first bit of the horizontal display acts as an enable for a dot clock
	divider (active low). It affects crtc timings by a factor of two.

	The second bit of the horizontal display appears to select the dot clock.
	The faster dot clock of the 768 dot mode allows for roughly square pixels,
	but PCG is incompatible, and using both will cause an open bus error.

	If the vertical height is set to 256 dots while f is set to 31.5KHz, lines will be
	doubled.

	The frequency select also appears to active a clock divider (active low).
	*/
} XBCrtcTimingCfg;

typedef struct XBCrtcScrollCfg
{
	// { x, y }
	uint16_t text[2];   // TVRAM
	uint16_t gp0[2];  // GVRAM0
	uint16_t gp1[2];  // GVRAM1
	uint16_t gp2[2];  // GVRAM2
	uint16_t gp3[2];  // GVRAM3
} XBCrtcScrollCfg;

void xb_crtc_set_timing(const XBCrtcTimingCfg *c);

// R09: Raster number for raster interrupt
void xb_crtc_set_raster_interrupt(uint16_t v);

// R10 - R19: Scroll registers ===============================================

void xb_crtc_set_text_xscroll(uint16_t v);  // R10: Text layer X scroll
void xb_crtc_set_text_yscroll(uint16_t v);  // R11: Text layer Y scroll
void xb_crtc_set_gp0_xscroll(uint16_t v);  // R12: Graphic layer 0 X scroll
void xb_crtc_set_gp0_yscroll(uint16_t v);  // R13: Graphic layer 0 Y scroll
void xb_crtc_set_gp1_xscroll(uint16_t v);  // R14: Graphic layer 1 X scroll
void xb_crtc_set_gp1_yscroll(uint16_t v);  // R15: Graphic layer 1 Y scroll
void xb_crtc_set_gp2_xscroll(uint16_t v);  // R16: Graphic layer 2 X scroll
void xb_crtc_set_gp2_yscroll(uint16_t v);  // R17: Graphic layer 2 Y scroll
void xb_crtc_set_gp3_xscroll(uint16_t v);  // R18: Graphic layer 3 X scroll
void xb_crtc_set_gp3_yscroll(uint16_t v);  // R19: Graphic layer 3 Y scroll

void xb_crtc_set_scroll(const XBCrtcScrollCfg *c);  // Set all at once.

// TODO: Raster copy stuff (R21-R23)

// CRTC control port
void xb_crtc_set_control(uint8_t v);

#endif // XCRTC_H
