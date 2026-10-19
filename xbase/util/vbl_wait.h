#pragma once

#ifdef __ASSEMBLER__
	.extern	g_vbl_count

	.extern	xb_vbl_wait_init
	.extern	xb_vbl_wait
	.extern	xb_vbl_get_frame_count
#else

#include <stdint.h>

extern uint32_t g_vbl_count;

// Registers a simple interrupt handler for the vertical blank interval.
// Returns a pointer to the previous routine so it may be saved.
void *xb_vbl_wait_init(void);

// Blocks until vertical blank has been hit.
void xb_vbl_wait(void);

// Returns the number of frames that have elapsed since xb_vbl_wait_init().
static inline uint32_t xb_vbl_get_frame_count(void) { return g_vbl_count; }

#endif
