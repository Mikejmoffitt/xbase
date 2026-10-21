//
// X68000 Fade Routines. Each palette line is handled individually.
//
#pragma once

#define XB_FADE_OP_IDLE           0  // Doing nothing.
#define XB_FADE_OP_STEP           1  // Stepping from current palette to target.
#define XB_FADE_OP_FROM_BLACK     2  // Sets palette to black before stepping.
#define XB_FADE_OP_TO_BLACK       3  // Sets target to internal black palette.
#define XB_FADE_OP_FROM_WHITE     4  // Sets palette to white before stepping.
#define XB_FADE_OP_TO_WHITE       5  // Sets target to internal white palette.

#define XB_FADE_WAIT(wait_frames) ((wait_frames)<<4)

#ifndef __ASSEMBLER__
#include <stdint.h>
#include <stddef.h>

typedef struct FadeCmd
{
	uint16_t op;  // Op * 4 for jump table
	uint8_t speed;
	uint8_t cnt;
	const uint16_t *target;  // Target 16-color palette
} FadeCmd;

#else

	.struct	0
FadeCmd.op:            ds.w 1
FadeCmd.target:        ds.l 1
FadeCmd.speed:         ds.b 1
FadeCmd.cnt:           ds.b 1
FadeCmd.len:

#endif  // __ASSEMBLER__

#ifndef __ASSEMBLER__

// Clear fade state.
void xb_fade_init(void);

// Step fade for one frame. Returns bitfield of fade channels that made changes.
// Call this *before* vertical blank.
uint16_t xb_fade_poll(void);

// Returns true if a fade is in progress.
bool xb_fade_active(void);

// Speed: top nybble is frames between ticks; low nybble is change per tick.

// Fade from the current palette to the target.
static inline void xb_fade_to(uint16_t line, const void *target, uint16_t speed, uint16_t lines);

// Fade from an initial black palette to the target.
static inline void xb_fade_from_black(uint16_t line, const void *const target, uint16_t speed, uint16_t lines);

// Fade from the current palette to black.
static inline void xb_fade_to_black(uint16_t line, uint16_t speed, uint16_t lines);

// Fade from an initial white palette to the target.
static inline void xb_fade_from_white(uint16_t line, const void *const target, uint16_t speed, uint16_t lines);

// Fade from the current palette to white.
static inline void xb_fade_to_white(uint16_t line, uint16_t speed, uint16_t lines);

// -----------------------------------------------------------------------------

extern void xb_fade_set(void);  // fade.a68
static inline void xb_fade_to(uint16_t line, const void *target, uint16_t speed, uint16_t lines)
{
	for (uint16_t i = 0; i < lines; i++)
	{
		register uint16_t d0 __asm__("d0") = line * sizeof(FadeCmd);
		register uint32_t d1 __asm__("d1") = (speed << 16) | (XB_FADE_OP_STEP * 4);
		register const void *a0 __asm__("a0") = target;
		__asm__ __volatile__ ("jsr xb_fade_set"
		              :
		              : "d" (d0), "d" (d1), "a" (a0)
		              : "a1", "memory", "cc");
		target = (uint8_t *)(target)+sizeof(uint16_t)*XB_PAL_LINE_SIZE;
		line++;
	}
}

static inline void xb_fade_from_black(uint16_t line, const void *target, uint16_t speed, uint16_t lines)
{
	for (uint16_t i = 0; i < lines; i++)
	{
		register uint16_t d0 __asm__("d0") = line * sizeof(FadeCmd);
		register uint32_t d1 __asm__("d1") = (speed << 16) | (XB_FADE_OP_FROM_BLACK * 4);
		register const void *a0 __asm__("a0") = target;
		__asm__ __volatile__ ("jsr xb_fade_set"
		              :
		              : "d" (d0), "d" (d1), "a" (a0)
		              : "a1", "memory", "cc");
		line++;
	}
}

static inline void xb_fade_to_black(uint16_t line, uint16_t speed, uint16_t lines)
{
	for (uint16_t i = 0; i < lines; i++)
	{
		register uint16_t d0 __asm__("d0") = line * sizeof(FadeCmd);
		register uint32_t d1 __asm__("d1") = (speed << 16) | (XB_FADE_OP_TO_BLACK * 4);
		register const void *a0 __asm__("a0") = NULL;  // ignored value
		__asm__ __volatile__ ("jsr xb_fade_set"
		              :
		              : "d" (d0), "d" (d1), "a" (a0)
		              : "a1", "memory", "cc");
		line++;
	}
}

static inline void xb_fade_from_white(uint16_t line, const void *target, uint16_t speed, uint16_t lines)
{
	for (uint16_t i = 0; i < lines; i++)
	{
		register uint16_t d0 __asm__("d0") = line * sizeof(FadeCmd);
		register uint32_t d1 __asm__("d1") = (speed << 16) | (XB_FADE_OP_FROM_WHITE * 4);
		register const void *a0 __asm__("a0") = target;
		__asm__ __volatile__ ("jsr xb_fade_set"
		              :
		              : "d" (d0), "d" (d1), "a" (a0)
		              : "a1", "memory", "cc");
		line++;
	}
}

static inline void xb_fade_to_white(uint16_t line, uint16_t speed, uint16_t lines)
{
	for (uint16_t i = 0; i < lines; i++)
	{
		register uint16_t d0 __asm__("d0") = line * sizeof(FadeCmd);
		register uint32_t d1 __asm__("d1") = (speed << 16) | (XB_FADE_OP_TO_WHITE * 4);
		register const void *a0 __asm__("a0") = NULL;  // ignored value
		__asm__ __volatile__ ("jsr xb_fade_set"
		              :
		              : "d" (d0), "d" (d1), "a" (a0)
		              : "a1", "memory", "cc");
		line++;
	}
}

#endif  // __ASSEMBLER__

