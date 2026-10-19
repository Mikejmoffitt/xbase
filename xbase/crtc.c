#include "xbase/crtc.h"

XBCrtcScrollCfg g_xb_scroll;

void xb_crtc_init_scroll(void)
{
	g_xb_scroll = (XBCrtcScrollCfg){ 0 };
}

void xb_crtc_set_timing(const XBCrtcTimingCfg *c)
{
	volatile uint16_t *crtc = (volatile uint16_t *)XB_CRTC_BASE;
	crtc[0] = c->htotal;
	crtc[1] = c->hsync_length;
	crtc[2] = c->hdisp_start;
	crtc[3] = c->hdisp_end;
	crtc[4] = c->vtotal;
	crtc[5] = c->vsync_length;
	crtc[6] = c->vdisp_start;
	crtc[7] = c->vdisp_end;
	crtc[8] = c->ext_h_adjust;

	crtc[20] = c->flags;
}

void xb_crtc_update_regs(void)
{
	volatile uint16_t *crtc = (volatile uint16_t *)XB_CRTC_BASE;
	crtc[10] = g_xb_scroll.text[0];
	crtc[11] = g_xb_scroll.text[1];
	crtc[12] = g_xb_scroll.gp0[0];
	crtc[13] = g_xb_scroll.gp0[1];
	crtc[14] = g_xb_scroll.gp1[0];
	crtc[15] = g_xb_scroll.gp1[1];
	crtc[16] = g_xb_scroll.gp2[0];
	crtc[17] = g_xb_scroll.gp2[1];
	crtc[18] = g_xb_scroll.gp3[0];
	crtc[19] = g_xb_scroll.gp3[1];
}
