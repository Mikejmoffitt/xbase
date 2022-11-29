#ifndef XBASE_UTIL_VBL_WAIT_H
#define XBASE_UTIL_VBL_WAIT_H

#include <stbool.h>

// Registers a simple interrupt handler for the vertical blank interval.
void xb_vbl_wait_init(void);

// Blocks until vertical blank has been hit.
void xb_vbl_wait(void);

// Returns the number of frames that have elapsed since xb_vbl_wait_init().
int xb_vbl_get_frame_count(void);

#endif  // XBASE_UTIL_VBL_WAIT_H
