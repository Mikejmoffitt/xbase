#pragma once
#ifndef __ASSEMBLER__
#include "xbase/util/display.h"
#include "xbase/vidcon.h"
#endif

// Defines an XBDisplayMode based on desired width, height, and scan rate.
// w    : active display columns
// h    : active display lines
// hrate: desired h. scan rate (scanline frequency) (0 = 15k, 1 = 24k, 2 = 31k)
// vrate: desired v. scan rate (framerate)
// flags: CRTC R20 flags (see crtc.h)
// mode : XBDisplayMode out pointer
//void crtcgen_set(uint16_t w, uint16_t h, uint16_t hrate, uint16_t vrate,
//                 uint16_t flags, XBDisplayMode *mode);

#ifdef __ASSEMBLER__
	.struct 0
XBCrtcGenParam.htotal:			ds.w 1
XBCrtcGenParam.hsync:			ds.w 1
XBCrtcGenParam.hstart:			ds.w 1
XBCrtcGenParam.hsize:			ds.w 1
XBCrtcGenParam.vtotal:			ds.w 1
XBCrtcGenParam.vsync:			ds.w 1
XBCrtcGenParam.vstart:			ds.w 1
XBCrtcGenParam.vsize:			ds.w 1
XBCrtcGenParam.ext_h_adj:		ds.w 1
XBCrtcGenParam.crtc_flags:		ds.w 1
XBCrtcGenParam.pcg_mode:		ds.w 1
XBCrtcGenParam.prio:			ds.w 1
XBCrtcGenParam.vidcon_flags:	ds.w 1

	.global xb_crtcgen_set
#else
typedef struct XBCrtcGenParam
{
	uint16_t htotal;
	uint16_t hsync;
	uint16_t hstart;
	uint16_t hsize;
	uint16_t vtotal;
	uint16_t vsync;
	uint16_t vstart;
	uint16_t vsize;
	uint16_t ext_h_adj;
	uint16_t crtc_flags;
	int16_t pcg_mode;  // -1 for auto
	uint16_t prio;
	uint16_t vidcon_flags;
} XBCrtcGenParam;

void xb_crtcgen_set(const XBCrtcGenParam *param, XBDisplayMode *mode);
#endif
