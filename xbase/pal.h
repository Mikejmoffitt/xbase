#pragma once
// XBase Palette manamger (pal)
// (c) Michael Moffitt 2024
//
// X68000 palette memory is $400 bytes in size, organized like so:
//
// 0xE82000 | Graphic Plane palette 0
//          | ~
// 0xE821E0 | Graphic Plane palette 15
// 0xE82200 | PCG palette 0, Text palette
// 0xE82220 | PCG palette 1
//          | ~
// 0xE823E0 | PCG palette 15
//
// Palette data is placed into a buffer, and that buffer is pollted to
// palette memory when xb_pal_poll() is called.
//
// Palette lines (32-byte array of color data) can be written to the buffer
// with xb_pal_set(), which automatically marks that line as needing a transfer.
//
// Alternatively, you may directly manipulate g_xb_pal_buffer, but must be sure
// to mark the row(s) for transfer that have been modified.
#ifndef __ASSEMBLER__
#include <stdint.h>
#include "xbase/memmap.h"
#endif

#define XB_PAL_LINE_COUNT 32
#define XB_PAL_LINE_SIZE  16
#define XB_PAL_COUNT      (XB_PAL_LINE_COUNT*XB_PAL_LINE_SIZE)

// Starting row definitions. There are sixteen row within the GP section,
// and sixteen more in the PCG section. The text palette is shared with
// PCG palette 0 (so, row 16).
#define XB_PAL_GR 0x00
#define XB_PAL_TX 0x10
#define XB_PAL_SP 0x10

// Palette definition macros.
#define XB_PAL555(r, g, b) ((((r) & 0x1F) << 6) | \
                            (((g) & 0x1F) << 11) | \
                            (((b) & 0x1F) << 1))

#define XB_PAL444(r, g, b) (XB_PAL555((r)<<1, (g)<<1, (b)<<1))
#define XB_PAL888(r, g, b) (XB_PAL555((r)>>3, (g)>>3, (b)>>3))
#define XB_PALHEX(x)       (XB_PAL888((((x) >> 16) & 0xFF), \
                                      (((x) >> 8) & 0xFF), \
                                      ((x) & 0xFF)))

#define XB_PAL_DIRTY_MASK_FULL (0xFFFFFFFF)


#ifdef __ASSEMBLER__

	.global	g_xb_pal
	.global	g_xb_pal_dirty

	.global	xb_pal_init
	.global	xb_pal_poll

#else

extern uint16_t g_xb_pal[XB_PAL_COUNT];
extern uint32_t g_xb_pal_dirty;

// Initializes palette manager by clearing palette buffer and requesting a
// full transfer (clear to black).
void xb_pal_init(void);

// Sets a single palette color.
static inline void xb_pal_set(uint16_t idx, uint16_t val);

// Fills a range with a single color.
static inline void xb_pal_fill(uint16_t idx, uint16_t val, uint16_t count);

// Caches and schedules a transfer to the palette.
// dest:  palette line (0 - 31)
// src:   pointer to data
// count: number of palette lines to copy (1 = sixteen colors).
static inline void xb_pal_load(uint16_t dest, const void *src, uint16_t count);

// Static implementations

static inline void xb_pal_set(uint16_t idx, uint16_t val)
{
	idx = idx % XB_ARRAYSIZE(g_xb_pal);
	g_xb_pal[idx] = val;
	g_xb_pal_dirty |= XB_BITVAL(idx >> 4);
}

static inline void xb_pal_fill(uint16_t idx, uint16_t val, uint16_t count)
{
	g_xb_pal_dirty = XB_PAL_DIRTY_MASK_FULL;
	uint16_t *dest = &g_xb_pal[idx];
	for (uint16_t i = 0; i < count; i++)
	{
		*dest++ = val;
	}
}

static inline void xb_pal_load(uint16_t dest, const void *src, uint16_t count)
{
	if ((dest+count) > XB_PAL_LINE_COUNT) return;
	const uint16_t pal_offs = dest << 4;

	const uint32_t *src32 = (const uint32_t *)(src);
	uint32_t *dest32 = (uint32_t *)(&g_xb_pal[pal_offs]);
	uint32_t dirty_bit_mark = XB_BITVAL(dest);

	while (count--)
	{
		*dest32++ = *src32++;
		*dest32++ = *src32++;
		*dest32++ = *src32++;
		*dest32++ = *src32++;
		*dest32++ = *src32++;
		*dest32++ = *src32++;
		*dest32++ = *src32++;
		*dest32++ = *src32++;
		g_xb_pal_dirty |= dirty_bit_mark;
		dirty_bit_mark <<= 1;
	}
}

// Copy data from the palette buffer to color memory. Call during vblank.
void xb_pal_poll(void);

#endif
