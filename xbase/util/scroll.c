#include "xbase/util/scroll.h"
#include "xbase/memmap.h"
#include "xbase/crtc.h"

XBScroll g_xb_scroll;

void xb_scroll_init(void)
{
	g_xb_scroll = (XBScroll){ 0 };
}

void xb_scroll_poll(void)
{
	volatile uint16_t *crtc = (volatile uint16_t *)XB_CRTC_BASE;
	crtc[10] = g_xb_scroll.tx[0];
	crtc[11] = g_xb_scroll.tx[1];
	crtc[12] = g_xb_scroll.gp0[0];
	crtc[13] = g_xb_scroll.gp0[1];
	crtc[14] = g_xb_scroll.gp1[0];
	crtc[15] = g_xb_scroll.gp1[1];
	crtc[16] = g_xb_scroll.gp2[0];
	crtc[17] = g_xb_scroll.gp2[1];
	crtc[18] = g_xb_scroll.gp3[0];
	crtc[19] = g_xb_scroll.gp3[1];
	volatile uint16_t *pcg = (volatile uint16_t *)XB_PCG_BG0_XSCRL;
	pcg[0] = g_xb_scroll.bg0[0];
	pcg[1] = g_xb_scroll.bg0[1];
	pcg[2] = g_xb_scroll.bg1[0];
	pcg[3] = g_xb_scroll.bg1[1];
}
