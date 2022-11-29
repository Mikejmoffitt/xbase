#include "xbase/util/vbl_wait.h"
#include "xbase/mfp.h"

static volatile bool s_vbl_wait_flag = true;
static volatile int s_vbl_count = 0;

static void vbl_irq(void)
{
	s_vbl_wait_flag = false;
	s_vbl_count++;
}

void xb_vbl_wait_init(void)
{
	xb_mfp_set_interrupt(XB_MFP_INT_CRTC_IRQ, vbl_irq);
}

void xb_vbl_wait(void)
{
	s_vbl_wait_flag = true;
	while (s_vbl_wait_flag) {}
}

int xb_vbl_get_frame_count(void)
{
	return s_vbl_count;
}
