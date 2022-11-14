#include "xbase/vidcon.h"

void xb_vidcon_init(const XBVidconConfig *c)
{
	volatile uint16_t *r0 = (volatile uint16_t *)XB_VIDCON_R0;
	volatile uint16_t *r1 = (volatile uint16_t *)XB_VIDCON_R1;
	volatile uint16_t *r2 = (volatile uint16_t *)XB_VIDCON_R2;

	*r0 = c->screen;
	*r1 = c->prio;
	*r2 = c->flags;
}
