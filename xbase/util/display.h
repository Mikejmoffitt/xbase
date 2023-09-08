// Monitor config management.
#ifndef UTIL_XB_DISPLAY_H
#define UTIL_XB_DISPLAY_H

#include <stdint.h>

#include "xbase/pcg.h"
#include "xbase/crtc.h"
#include "xbase/vidcon.h"

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

// Initialize with a list of display modes. The first mode from the list is
// applied to the video chipset.
void xb_display_init(XBDisplay *d, const XBDisplayMode *modes,
                     int16_t num_modes);

// Get the current display mode information.
const XBDisplayMode *xb_display_get_mode(const XBDisplay *d);

// Go to the next display mode.
void xb_display_cycle_mode(XBDisplay *d);

#endif  // XB_DISPLAY_H
