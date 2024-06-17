// Monitor config management.
#pragma once

#ifndef __ASSEMBLER__
#include <stdint.h>

#include "xbase/pcg.h"
#include "xbase/crtc.h"
#include "xbase/vidcon.h"
#endif


#ifdef __ASSEMBLER__
	.struct 0
XBDisplayMode.crtc:		ds.b XBCrtcTimingCfg.len
XBDisplayMode.pcg:		ds.b XBPcgCfg.len
XBDisplayMode.vidcon:	ds.b XBVidconCfg.len
XBDisplayMode.len:

	.struct 0
XBDisplay.modes:		ds.l 1
XBDisplay.num_modes:	ds.w 1
XBDisplay.current_mode:	ds.w 1
XBDisplay.len:
#else
typedef struct XBDisplayMode
{
	XBCrtcTimingCfg crtc;
	XBPcgCfg pcg;
	XBVidconCfg vidcon;
} XBDisplayMode;

typedef struct XBDisplay
{
	const XBDisplayMode *modes;
	int16_t num_modes;
	int16_t current_mode;
} XBDisplay;
#endif

#ifdef __ASSEMBLER__
	.global	xb_display_init
	.global	xb_display_get_mode
	.global	xb_display_cycle_mode
#else
// Initialize with a list of display modes. The first mode from the list is
// applied to the video chipset.
void xb_display_init(XBDisplay *d, const XBDisplayMode *modes,
                     int16_t num_modes);

// Get the current display mode information.
const XBDisplayMode *xb_display_get_mode(const XBDisplay *d);

// Go to the next display mode.
void xb_display_cycle_mode(XBDisplay *d);

#endif
