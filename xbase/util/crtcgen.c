#include "xbase/util/crtcgen.h"
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

// LOWER H rate results in brighter display, with slightly wider raster.
// SUPER STREET FIGHTER II
// 15k mode sets up 15.52KHz x 59.71Hz
// 31k mode sets up 31.05KHz x 54.66Hz (can adjust V...)
// COTTON
// 15k (wider) 15.98KHz x 61.46Hz; multiple 2 x 1
// 15k (narrower) 15.98KHz x 61.46Hz; multiple 1 x 2
// 31k (wider) 31.50KHz x 55.45Hz; multiple 1 x 2
// 31k (narrower) 31.50KHz x 55.42Hz; multiple 1 x 2

void xb_crtcgen_set(const XBCrtcGenParam *param, XBDisplayMode *mode)
{
	mode->crtc.htotal = param->htotal;
	mode->crtc.hsync_length = param->hsync;
	mode->crtc.hdisp_start = param->hstart;
	mode->crtc.hdisp_end = param->hend;

	mode->crtc.vtotal = param->vtotal;
	mode->crtc.vsync_length = param->vsync;
	mode->crtc.vdisp_start = param->vstart;
	mode->crtc.vdisp_end = param->vend;

	mode->crtc.ext_h_adjust = param->ext_h_adj;
	mode->crtc.flags = param->crtc_flags;

	if (param->pcg_mode < 0) mode->pcg.mode = param->crtc_flags & 0xFF;
	else mode->pcg.mode = param->pcg_mode & 0xFF;
	const bool pcg_hires = !(mode->pcg.mode & 0x0001);

	mode->pcg.htotal = pcg_hires ? 0xFF : param->htotal;
	mode->pcg.hdisp = mode->crtc.hdisp_start + 4;
	mode->pcg.vdisp = mode->crtc.vdisp_start;

	mode->vidcon.screen = param->crtc_flags >> 8;
	mode->vidcon.prio = param->prio;
	mode->vidcon.flags = param->vidcon_flags;
}
/*
void crtcgen_set(uint16_t w, uint16_t h, uint16_t hrate, uint16_t vrate,
                 uint16_t flags, XBDisplayMode *mode)
{
	// Ideal horizontal scanning frequencies
//	static const float kideal_hf_15 = 15.625e3;
//	static const float kideal_hf_24 = 24.830e3;
//	static const float kideal_hf_31 = 31.778e3;

	// Dot clocks

	// 15.98KHz
	// 296 dots per line (for htotal = $25)
	// 259 lines * 296 columns * 61.46Hz = ~4.71MHz dot clock
	// "low-res" dot clock is 39 / 8?


	// 31.5KHz
	// 728 dots per line (for htotal = $5B)
	// 69Mhz and 39MHz crystals
	// 567 lines * 728 columns * 55.42Hz = ~22.876MHz dot clock

	// "hi-res" dot clock is 69Mhz / 3
//	static const float kpclk_15 = 39e6 / 7.0f;

	// Width is limited to a multiple of 8.
	w = w & 0xFFF7;

	//
	// Baseline timings
	//
	// 15k
	if (hrate == 0)
	{
		mode->crtc.htotal = 600 / 8;
		mode->crtc.hsync_length = 3;
	}
	// 24k
	else if (hrate == 1)
	{
		mode->crtc.htotal = 560 / 8;
		mode->crtc.hsync_length = 6;
		mode->crtc.vtotal = 262;
		mode->crtc.vsync_length = 2;
	}
	// 31k and beyond
	else
	{
		mode->crtc.htotal = 728 / 8;
		mode->crtc.hsync_length = 9;
	}

	// Alternate dot clock changes H in proportion
	if (flags & 0x0002)
	{
		// 34.5MHz dot clock (768px alternate) ?
		mode->crtc.htotal *= (1096 / 728.0);
		mode->crtc.hsync_length *= (1096 / 728.0);
	}

	// Horizontal gets cut in half if using low-res mode
	if (!flags & 0x0001)
	{
		mode->crtc.htotal >>= 1;
		mode->crtc.hsync_length >>= 1;
	}

	// Center active display within line.
	mode->crtc.hdisp_start = (mode->crtc.htotal - (w / 8)) / 2;
	mode->crtc.hdisp_end = mode->crtc.hdisp_start + (w / 8);

	//
	// Vertical
	//
}*/
