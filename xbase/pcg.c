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
	volatile uint16_t *pcg = (volatile uint16_t *)XB_PCG_HTOTAL;
	volatile uint16_t *crtc = (volatile uint16_t *)XB_CRTC_BASE;
	uint16_t mode = crtc[20] & 0xFF;
//	const bool hmode = (mode & XB_PCG_MODE_HMODE) ? true : false;

//	pcg[0] = hmode ? 0xFF : crtc[0];
//	pcg[1] = crtc[2] + 4;
//	pcg[2] = crtc[6];
	pcg[3] = mode;

	xb_pcg_set_blank(true);
	xb_pcg_set_bg0_txsel(0);
	xb_pcg_set_bg1_txsel(1);
	xb_pcg_set_bg0_enable(1);
	xb_pcg_set_bg1_enable(1);
	xb_pcg_set_bg0_xscroll(0);
	xb_pcg_set_bg1_xscroll(0);
	xb_pcg_set_bg0_yscroll(0);
	xb_pcg_set_bg1_yscroll(0);
	xb_pcg_set_blank(false);
}
