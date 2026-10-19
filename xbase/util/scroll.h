#pragma once

#include "xbase/macro.h"
#ifndef __ASSEMBLER__
#include <stdint.h>
#endif  // __ASSEMBLER__


#ifdef __ASSEMBLER__
// Indexed as x, y
XBScroll.tx:        ds.w 2
XBScroll.gp:
XBScroll.gp0:       ds.w 2
XBScroll.gp1:       ds.w 2
XBScroll.gp2:       ds.w 2
XBScroll.gp3:       ds.w 2
XBScroll.bg:
XBScroll.bg0:       ds.w 2
XBScroll.bg1:       ds.w 2
XBScroll.len:

	.extern	g_xb_scroll

	.extern	xb_scroll_init
	.extern	xb_scroll_poll

#else
typedef struct XBScroll
{
	// { x, y }
	uint16_t tx[2];   // TVRAM
	union
	{
		struct
		{
			uint16_t gp0[2];  // GR Page 0
			uint16_t gp1[2];  // "       1
			uint16_t gp2[2];  // "       2
			uint16_t gp3[2];  // "       3
		};
		uint16_t gp[8];  // All GVRAM scroll
	};
	union
	{
		struct
		{
			uint16_t bg0[2];
			uint16_t bg1[2];
		};
		uint16_t bg[4];  // All PCG BG scroll
	};
} XBScroll;

extern XBScroll g_xb_scroll;

void xb_scroll_init(void);
void xb_scroll_poll(void);
#endif
