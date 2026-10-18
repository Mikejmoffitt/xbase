#pragma once

//
// Bilingual enums, with strict limits
//
#ifdef __ASSEMBLER__
#define XB_ENUMSTART .struct 0
#else
#define XB_ENUMSTART enum {
#endif  // __ASSEMBLER__

#ifdef __ASSEMBLER__
#define XB_ENUMNEXT(name) name##: ds.b 1
#else
#define XB_ENUMNEXT(name) name ,
#endif  // __ASSEMBLER__

#ifdef __ASSEMBLER__
#define XB_ENUMEND  // end of enum
#else
#define XB_ENUMEND };
#endif

//
// Bit operations
//

#define XB_BITVAL(n) (1 << (n))
#define XB_BTST(x, n) (!!((x) & XB_BITVAL(n)))
#define XB_BSET(x, n) x = ((x) | (XB_BITVAL(n)))
#define XB_BCLR(x, n) x =((x) & (~XB_BITVAL(n)))

// Misc

#define XB_NUM_IS_POW2(x) (((x) & ((x) - 1)) == 0)

#define XB_MAX(a, b) \
    ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
       _a > _b ? _a : _b; })

#define XB_MIN(a, b) \
    ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
       _a < _b ? _a : _b; })

#define XB_ARRAYSIZE(x) (sizeof(x) / sizeof(x[0]))

// Interrupt handler attributes
#define XB_ISR __attribute__((__interrupt_handler__))

// Interrupt enablement/disablement for critical sections

#ifdef __ASSEMBLER__

	.macro	xb_di
	ori.w	#0x0700, sr
	.endm

	.macro	xb_ei
	andi.w	#0xF8FF, sr
	.endm

#else

static inline void xb_di(void)
{
	__asm__ __volatile__("ori.w	#0x0700, %sr");
}

static inline void xb_ei(void)
{
	__asm__ __volatile__("andi.w	#0xF8FF, %sr");
}

#endif   // __ASSEMBLER__

// C bit (for return status in assembly)

#ifdef __ASSEMBLER__

	.macro	xb_sec
	ori.b	#0x01, ccr
	.endm

	.macro	xb_clc
	andi.b	#0xFE, ccr
	.endm

#endif  // __ASSEMBLER__
