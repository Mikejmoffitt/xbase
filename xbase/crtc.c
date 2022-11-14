#include "xbase/crtc.h"

#include "xbase/memmap.h"

void xb_crtc_set_timing(const XBCrtcTimingCfg *c)
{
	volatile uint16_t *reg = (volatile uint16_t *)XB_CRTC_BASE;
	reg[0] = c->htotal;
	reg[1] = c->hsync_length;
	reg[2] = c->hdisp_start;
	reg[3] = c->hdisp_end;
	reg[4] = c->vtotal;
	reg[5] = c->vsync_length;
	reg[6] = c->vdisp_start;
	reg[7] = c->vdisp_end;
	reg[8] = c->ext_h_adjust;

	reg[20] = c->flags;
	// R22-R23 and the CTRL register are not touched here
}

// R09: Raster number for raster interrupt
void xb_crtc_set_raster_interrupt(uint16_t v)
{
	volatile uint16_t *reg = (volatile uint16_t *)XB_CRTC_BASE;
	reg[9] = v;
}

// R10 - R19: Scroll registers ===============================================

void xb_crtc_set_text_xscroll(uint16_t v)
{
	volatile uint16_t *reg = (volatile uint16_t *)XB_CRTC_BASE;
	reg[10] = v;
}

void xb_crtc_set_text_yscroll(uint16_t v)
{
	volatile uint16_t *reg = (volatile uint16_t *)XB_CRTC_BASE;
	reg[11] = v;
}

void xb_crtc_set_gp0_xscroll(uint16_t v)
{
	volatile uint16_t *reg = (volatile uint16_t *)XB_CRTC_BASE;
	reg[12] = v;
}

void xb_crtc_set_gp0_yscroll(uint16_t v)
{
	volatile uint16_t *reg = (volatile uint16_t *)XB_CRTC_BASE;
	reg[13] = v;
}

void xb_crtc_set_gp1_xscroll(uint16_t v)
{
	volatile uint16_t *reg = (volatile uint16_t *)XB_CRTC_BASE;
	reg[14] = v;
}

void xb_crtc_set_gp1_yscroll(uint16_t v)
{
	volatile uint16_t *reg = (volatile uint16_t *)XB_CRTC_BASE;
	reg[15] = v;
}

void xb_crtc_set_gp2_xscroll(uint16_t v)
{
	volatile uint16_t *reg = (volatile uint16_t *)XB_CRTC_BASE;
	reg[16] = v;
}

void xb_crtc_set_gp2_yscroll(uint16_t v)
{
	volatile uint16_t *reg = (volatile uint16_t *)XB_CRTC_BASE;
	reg[17] = v;
}

void xb_crtc_set_gp3_xscroll(uint16_t v)
{
	volatile uint16_t *reg = (volatile uint16_t *)XB_CRTC_BASE;
	reg[18] = v;
}

void xb_crtc_set_gp3_yscroll(uint16_t v)
{
	volatile uint16_t *reg = (volatile uint16_t *)XB_CRTC_BASE;
	reg[19] = v;
}

void xb_crtc_set_scroll(const XBCrtcScrollCfg *c)
{
	xb_crtc_set_text_xscroll(c->text[0]);
	xb_crtc_set_text_yscroll(c->text[1]);
	xb_crtc_set_gp0_xscroll(c->gp0[0]);
	xb_crtc_set_gp0_yscroll(c->gp0[1]);
	xb_crtc_set_gp1_xscroll(c->gp1[0]);
	xb_crtc_set_gp1_yscroll(c->gp1[1]);
	xb_crtc_set_gp2_xscroll(c->gp2[0]);
	xb_crtc_set_gp2_yscroll(c->gp2[1]);
	xb_crtc_set_gp3_xscroll(c->gp3[0]);
	xb_crtc_set_gp3_yscroll(c->gp3[1]);
}

void xb_crtc_set_control(uint8_t v)
{
	volatile uint8_t *reg = (volatile uint8_t *)(XB_CRTC_BASE + 0x481);
	*reg = v;
}
