#pragma once

#ifdef __ASSEMBLER__
	.global	xb_vbl_wait_init
	.global	xb_vbl_wait
	.global	xb_vbl_get_frame_count
#else

#include <stdint.h>

// Registers a simple interrupt handler for the vertical blank interval.
void xb_vbl_wait_init(void);

// Blocks until vertical blank has been hit.
void xb_vbl_wait(void);

// Returns the number of frames that have elapsed since xb_vbl_wait_init().
uint32_t xb_vbl_get_frame_count(void);

#endif
