#ifndef XBASE_UTIL_CRTCGEN_H
#define XBASE_UTIL_CRTCGEN_H

#include "xbase/util/display.h"
#include "xbase/vidcon.h"

// Defines an XBDisplayMode based on desired width, height, and scan rate.
// w    : active display columns
// h    : active display lines
// hrate: desired h. scan rate (scanline frequency) (0 = 15k, 1 = 24k, 2 = 31k)
// vrate: desired v. scan rate (framerate)
// flags: CRTC R20 flags (see crtc.h)
// mode : XBDisplayMode out pointer
//void crtcgen_set(uint16_t w, uint16_t h, uint16_t hrate, uint16_t vrate,
//                 uint16_t flags, XBDisplayMode *mode);

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



#endif  // XBASE_UTIL_CRTCGEN_H
