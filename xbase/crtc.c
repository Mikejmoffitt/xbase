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

	crtc[0x28/sizeof(uint16_t)] = c->flags;
}

void xb_crtc_update_regs(void)
{
	volatile uint16_t *crtc = (volatile uint16_t *)XB_CRTC_BASE;
	crtc[0x14] = g_xb_scroll.text[0];
	crtc[0x16] = g_xb_scroll.text[1];
	crtc[0x18] = g_xb_scroll.gp0[0];
	crtc[0x1A] = g_xb_scroll.gp0[1];
	crtc[0x1C] = g_xb_scroll.gp1[0];
	crtc[0x1E] = g_xb_scroll.gp1[1];
	crtc[0x20] = g_xb_scroll.gp2[0];
	crtc[0x22] = g_xb_scroll.gp2[1];
	crtc[0x24] = g_xb_scroll.gp3[0];
	crtc[0x26] = g_xb_scroll.gp3[1];
}
