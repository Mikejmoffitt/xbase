#ifndef XBASE_IPL_H
#define XBASE_IPL_H

// Macro for modifying the processor interrupt priority level.
// This is useful for protecting critical sections.

#include <stdint.h>

#define XB_ISR __attribute__((__interrupt_handler__))
#define XB_IPL_ALLOW_ALL 0
#define XB_IPL_ALLOW_NONE 7

// Returns the previous IPL level so that it may be restored.
// Contributed by UNDEF2.
static inline uint8_t xb_set_ipl(uint8_t ipl)
{
	uint16_t sr;
	__asm volatile ("move.w %%sr, %0" : "=r" (sr));
	const uint8_t old_ipl = (sr >> 8) & 7;
	sr &= ~(7 << 8);
	sr |= (uint16_t)ipl << 8;
	__asm volatile ("move.w %0, %%sr" : : "r" (sr));
	return old_ipl;
}

#endif  // XBASE_IPL_H
