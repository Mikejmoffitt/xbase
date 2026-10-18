#include "xbase/vidcon.h"

XBVidconCfg g_xb_vidcon_cfg;

void xb_vidcon_update_regs(void)
{
	volatile uint16_t *r0 = (volatile uint16_t *)XB_VIDCON_R0;
	volatile uint16_t *r1 = (volatile uint16_t *)XB_VIDCON_R1;
	volatile uint16_t *r2 = (volatile uint16_t *)XB_VIDCON_R2;
	*r0 = g_xb_vidcon_cfg.screen;
	*r1 = g_xb_vidcon_cfg.prio;
	*r2 = g_xb_vidcon_cfg.flags;
}
