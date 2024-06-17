#pragma once

// Macro for modifying the processor interrupt priority level.
// This is useful for protecting critical sections.

#ifndef __ASSEMBLER__
// Attribute for ISR handlers written in C.
#define XB_ISR __attribute__((__interrupt_handler__))
#include <stdint.h>
#endif

// Macros for the IPL level to enable or disable all ints.
#define XB_IPL_ALLOW_ALL 0
#define XB_IPL_ALLOW_NONE 7

#ifdef __ASSEMBLER__
	.global	xb_set_ipl
#else
// Sets the IPL.
// Returns the previous level so that it may be restored later.
uint8_t xb_set_ipl(uint8_t ipl);
#endif
