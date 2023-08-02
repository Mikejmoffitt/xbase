/*

XBase PCG support (pcg)
c. Michael Moffitt 2021-2022

The PCG chip provides two tile-based scroll planes as well as 128 separately
movable sprites.

This file contains simple functions for configuring the PCG registers. Many
of them are generally set in conjunction with values taken from the CRTC's
configuration.

I have placed my notes on the PCG registers in doc/pcg.txt.

*/

#ifndef XBASE_PCG_H
#define XBASE_PCG_H

#include <stdint.h>
#include <stdbool.h>
#include "xbase/memmap.h"

// Attributes to specify sprite and backdrop tiles
#define XB_PCG_ATTR(_yf_,_xf_,_c_,_p_) ((((_yf_)&1)<<15) | (((_xf_)&1)<<14) |\
                                       (((_c_)&0xF)<<8) | (((_p_)&0xFF)))

// Struct representing a sprite
typedef struct
{
	uint16_t x; // Limited to 10 bits of precision (0 - 1024)
	uint16_t y; // "
	uint16_t attr; // Bitfield; see PCG_ATTR macro
	uint16_t prio; // Priority relative to BG.
	               // 0 - Sprite not drawn.
	               // 1 - Sprite behind both tile planes.
	               // 2 - Sprite behind BG0, but on top of BG1.
	               // 3 - Sprite above both planes.
} X68kPcgSprite;

// Setup =======================================================================

// This should match the lower eight bits of CRTC Register 20.
typedef enum __attribute__((packed)) XBPcgMode
{
	XB_PCG_MODE_LH = 0x0010,  // Set for 31khz (line double) mode.
	XB_PCG_MODE_VMODE = 0x0004,  // Set for 512-line mode.
	XB_PCG_MODE_HMODE = 0x0001,  // Set for 512-dot mode; enables 16x16 tiles.
} XBPcgMode;

typedef struct XBPcgCfg
{
	// H-total should be the same value as the CRTC R00 (Htotal) in low-res
	// mode (256 x 256), otherwise it should be set to $FF. As with CRTC R00,
	// make sure that the value set for this register is an odd number.
	uint16_t htotal;  // 0xEB080A
	// H-disp is set by taking the value of R02 from CRTC and adding four.
	uint16_t hdisp;  // 0xEB080C
	// V-disp is set by taking the value of R06 from CRTC.
	uint16_t vdisp;  // 0xEB080E
	// Mode is generally set by taking the lower 8 bits from CRTC R20.
	XBPcgMode mode;  // 0xEB0810
} XBPcgCfg;

void xb_pcg_init(const XBPcgCfg *c);

// Turn off the display for faster transfer
void xb_pcg_set_disp_en(bool en);

// Change the mappings for BG1 and BG0 nametables
void xb_pcg_set_bg1_txsel(uint8_t t);
void xb_pcg_set_bg0_txsel(uint8_t t);

// Enable or disable BG layer display
void xb_pcg_set_bg1_enable(bool en);
void xb_pcg_set_bg0_enable(bool en);

// These functions are slower than just working with pointers to PCG_BGx_NAME,
// but may be helpful for debugging or small changes.
static inline void xb_pcg_set_bg0_tile(uint16_t x, uint16_t y, uint16_t attr);
static inline void xb_pcg_set_bg1_tile(uint16_t x, uint16_t y, uint16_t attr);

// Background plane scroll.
static inline void xb_pcg_set_bg0_xscroll(uint16_t x);
static inline void xb_pcg_set_bg1_xscroll(uint16_t x);
static inline void xb_pcg_set_bg0_yscroll(uint16_t y);
static inline void xb_pcg_set_bg1_yscroll(uint16_t y);

// Sprites. Take care to avoid manipulating these during active display (in
// other words, write to this during vertical blank).
static inline volatile X68kPcgSprite *xb_pcg_get_sprite(uint8_t idx);
static inline void xb_pcg_set_sprite(uint8_t idx, int16_t x,
                                     int16_t y, uint16_t attr, uint16_t prio);
static inline void xb_pcg_clear_sprites(void);

// Optional interface for placing sprites without regard to slot number.
void xb_pcg_add_sprite(int16_t x, int16_t y, uint16_t attr, uint16_t prio);

// If using xb_pcg_add_sprite() to draw sprites, call xb_pcg_finish_sprites()
// once you've finished drawing sprites.
void xb_pcg_finish_sprites(void);

// Helper functions for putting tiles in PCG VRAM.
void xb_pcg_transfer_pcg_data(const void *source, uint16_t dest_tile,
                              uint16_t num_tiles);

// Static implementations ======================================================

// Sets a tile. Not recommended for drawing a background or much else beyond
// small changes or playing around, as it's slower than doing a large DMA.
static inline void xb_pcg_set_bg0_tile(uint16_t x, uint16_t y, uint16_t attr)
{
	volatile uint16_t *nt = (volatile uint16_t *)XB_PCG_BG0_NAME;
	nt += x;
	nt += (y << 6);
	*nt = attr;
}

static inline void xb_pcg_set_bg1_tile(uint16_t x, uint16_t y, uint16_t attr)
{
	volatile uint16_t *nt = (volatile uint16_t *)XB_PCG_BG1_NAME;
	nt += x;
	nt += (y << 6);
	*nt = attr;
}

// Scroll registers
static inline void xb_pcg_set_bg0_xscroll(uint16_t x)
{
	volatile uint16_t *scr = (volatile uint16_t *)XB_PCG_BG0_XSCRL;
	*scr = x;
}

static inline void xb_pcg_set_bg1_xscroll(uint16_t x)
{
	volatile uint16_t *scr = (volatile uint16_t *)XB_PCG_BG1_XSCRL;
	*scr = x;
}

static inline void xb_pcg_set_bg0_yscroll(uint16_t y)
{
	volatile uint16_t *scr = (volatile uint16_t *)XB_PCG_BG0_YSCRL;
	*scr = y;
}

static inline void xb_pcg_set_bg1_yscroll(uint16_t y)
{
	volatile uint16_t *scr = (volatile uint16_t *)XB_PCG_BG1_YSCRL;
	*scr = y;
}

static inline volatile X68kPcgSprite *xb_pcg_get_sprite(uint8_t idx)
{
	volatile X68kPcgSprite *ret = (volatile X68kPcgSprite *)XB_PCG_SPR_TABLE;
	ret += idx;
	return ret;
}

static inline void xb_pcg_set_sprite(uint8_t idx, int16_t x,
                                     int16_t y, uint16_t attr, uint16_t prio)
{
	volatile X68kPcgSprite *spr = xb_pcg_get_sprite(idx);
	spr->x = x + 16;
	spr->y = y + 16;
	spr->attr = attr;
	spr->prio = prio;
}

// Clears all sprites by setting priority to %00 (invisible)
static inline void xb_pcg_clear_sprites(void)
{
	volatile X68kPcgSprite *spr = xb_pcg_get_sprite(0);
	uint8_t i = 0;
	for (i = 0; i < 128; i++)
	{
		spr[i].prio = 0x00;
	}
}

#endif  // XBASE_PCG_H
