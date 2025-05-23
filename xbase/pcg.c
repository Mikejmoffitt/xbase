#include "xbase/pcg.h"

#include "xbase/memmap.h"

#include <string.h>

uint16_t g_xb_pcg_ctrl;

void xb_pcg_init(const XBPcgCfg *c)
{
	volatile uint16_t *pcg_reg = (volatile uint16_t *)XB_PCG_HTOTAL;
	pcg_reg[0] = c->htotal;
	pcg_reg[1] = c->hdisp;
	pcg_reg[2] = c->vdisp;
	pcg_reg[3] = c->mode;

	xb_pcg_set_disp_en(0);
	xb_pcg_set_bg0_txsel(0);
	xb_pcg_set_bg1_txsel(1);
	xb_pcg_set_bg0_enable(1);
	xb_pcg_set_bg1_enable(1);
	xb_pcg_set_bg0_xscroll(0);
	xb_pcg_set_bg1_xscroll(0);
	xb_pcg_set_bg0_yscroll(0);
	xb_pcg_set_bg1_yscroll(0);
	xb_pcg_clear_sprites();
	xb_pcg_set_disp_en(1);
}
