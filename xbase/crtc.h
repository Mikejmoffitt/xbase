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

#ifndef __ASSEMBLER__
#include <stdint.h>
#endif

//
// Raster Timing Configuration Structures
//

//
// R00-R07 calculations, from Inside X68000:
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
// R20: Flags
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
#define XB_CRTC_R20_VSIZE_256  (0x0<<3)
#define XB_CRTC_R20_VSIZE_512  (0x1<<3)
#define XB_CRTC_R20_FREQ_31K   SAI_BITVAL(4)
// Rather than using these below, call xb_vidcon_update_crtc().
#define XB_CRTC_R20_COL_16     (0x00<<8)
#define XB_CRTC_R20_COL_256    (0x01<<8)
#define XB_CRTC_R20_COL_65536  (0x03<<8)
#define XB_CRTC_R20_SIZ        SAI_BITVAL(10)

#ifndef __ASSEMBLER__

static inline volatile uint16_t *xb_get_crtc(void)
{
	return (volatile uint16_t *)XB_CRTC_BASE;
}

// Copies the appropriate bits from Vidcon R0 into CRTC R20.
static inline void xb_crtc_update_vidcon_bits(uint16_t r20_hibits)
{
	volatile uint16_t *crtc = xb_get_crtc();
	uint16_t rdata = crtc[20];
	rdata &= ~(0x07 << 8);
	rdata |= r20_hibits;
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
