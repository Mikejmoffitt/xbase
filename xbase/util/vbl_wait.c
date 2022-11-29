#include "xbase/util/vbl_wait.h"
#include "xbase/mfp.h"

// TODO: Is this the start or end of VDISP?
#define VBL_VECTOR XB_MFP_INT_VDISP

static volatile bool s_vbl_wait_flag = true;
static volatile int s_vbl_count = 0;

__attribute__((interrupt)) static void vbl_irq(void)
{
	s_vbl_wait_flag = false;
	s_vbl_count++;
}

void xb_vbl_wait_init(void)
{
	xb_mfp_set_interrupt(VBL_VECTOR, vbl_irq);
	xb_mfp_set_interrupt_enable(VBL_VECTOR, true);
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

#undef VBL_VECTOR
