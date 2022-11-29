#include "xb_display.h"
#include "xbase/vbl.h"
#include <iocs.h>

static void apply_mode(const XBDisplayMode *mode)
{
	xb_crtc_init(&mode->crtc);
	xb_vidcon_init(&mode->vidcon);

#ifdef XB_DISPLAY_512PX_PCG_HACK
	// Hack to init the PCG with alternative register values to clear the
	// sprite line buffer. Without this, the right half of the screen may be
	// filled with randomly colored vertical lines when forcing the PCG to
	// use 8x8 BG tile pacing in a high resolution mode.
	static const X68kPcgConfig hires_hack_pcg =
	{
		0x00FF, 0x0015, 0x001C, 0x0011
	};

	xb_pcg_init(&hires_hack_pcg);
#endif  // XB_DISPLAY_512PX_PCG_HACK

	xb_vbl_wait_for_vblank();
	xb_pcg_init(&mode->pcg);
}

// Initialize with a list of display modes. Mode 0 is applied to the video
// chipset.
void xb_display_init(XBDisplay *d, const XBDisplayMode **modes,
                     int16_t num_modes)
{
	d->modes = modes;
	d->num_modes = num_modes;
	d->current_mode = 0;

	apply_mode(d->modes[0]);
}

const XBDisplayMode *xb_display_get_mode(const XBDisplay *d)
{
	return d->modes[d->current_mode];
}

// Go to the next display mode.
void xb_display_cycle_mode(XBDisplay *d)
{
	d->current_mode++;
	if (d->current_mode >= d->num_modes)
	{
		d->current_mode = 0;
	}
	apply_mode(d->modes[d->current_mode]);
}
