#include "xbase/pcg.h"
#include "xbase/crtc.h"

#include "xbase/memmap.h"

#include <string.h>

// Scroll registers
static inline void xb_pcg_set_bg0_xscroll(uint16_t x)
{
	volatile uint16_t *scr = (volatile uint16_t *)XB_PCG_BG0_XSCRL;
	*scr = x;
}

static inline void xb_pcg_set_bg1_xscroll(uint16_t x)
{
	volatile uint16_t *scr = (volatile uint16_t *)XB_PCG_BG1_XSCRL;
	*scr = x;
}

static inline void xb_pcg_set_bg0_yscroll(uint16_t y)
{
	volatile uint16_t *scr = (volatile uint16_t *)XB_PCG_BG0_YSCRL;
	*scr = y;
}

static inline void xb_pcg_set_bg1_yscroll(uint16_t y)
{
	volatile uint16_t *scr = (volatile uint16_t *)XB_PCG_BG1_YSCRL;
	*scr = y;
}

void xb_pcg_init(void)
{
	xb_pcg_set_blank(true);
	xb_pcg_set_bg0_txsel(0);
	xb_pcg_set_bg1_txsel(1);
	xb_pcg_set_bg0_enable(1);
	xb_pcg_set_bg1_enable(1);
	xb_pcg_set_bg0_xscroll(0);
	xb_pcg_set_bg1_xscroll(0);
	xb_pcg_set_bg0_yscroll(0);
	xb_pcg_set_bg1_yscroll(0);
	xb_pcg_set_crtc_regs();
	xb_pcg_set_blank(false);
}

void xb_pcg_set_crtc_regs(void)
{
	const uint16_t *crtc_timing = g_xb_crtc_regs;
	volatile uint16_t *crtc_regs = (volatile uint16_t *)XB_CRTC_BASE;
	uint16_t mode = crtc_regs[20] & 0xFF;

	if (mode & XB_CRTC_R20_HSIZE_768) return;  // PCG inactive.

	const bool hmode = (mode & XB_PCG_MODE_HMODE) ? true : false;

	volatile uint16_t *pcg = (volatile uint16_t *)XB_PCG_HTOTAL;
	pcg[0] = hmode ? 0xFF : crtc_timing[0];
	pcg[1] = crtc_timing[2] + 4;
	pcg[2] = crtc_timing[6];
	pcg[3] = mode;
}
