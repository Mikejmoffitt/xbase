// XBase PCG support (pcg)
// c. Michael Moffitt 2021-2022
//
// The PCG chip provides two tile-based scroll planes as well as 128 separately
// movable sprites.
//
// This file contains simple functions for configuring the PCG registers. Many
// of them are generally set in conjunction with values taken from the CRTC's
// configuration.
//
// I have placed my notes on the PCG registers in doc/pcg.txt.
#pragma once

#ifndef __ASSEMBLER__
#include <stdint.h>
#include <stdbool.h>
#include "xbase/memmap.h"
#include "xbase/macro.h"
#endif

#define XB_PCG_BG_OFFS(x, y) (sizeof(uint16_t)*((x) + ((y) << 6)))

#define XB_PCG_SPR_COUNT 128

#define XB_PCG_VF XB_BITVAL(15)
#define XB_PCG_HF XB_BITVAL(14)
#define XB_PCG_PAL(x) ((x)<<8)

#define XB_PCG_PRIO_HIDDEN 0
#define XB_PCG_PRIO_BOTTOM 1
#define XB_PCG_PRIO_MIDDLE 2
#define XB_PCG_PRIO_TOP    3

#ifdef __ASSEMBLER__
	.struct 0
XBSprite.x:		ds.w 1  // Limited to 10 bits of precision (0 - 1024)
XBSprite.y:		ds.w 1  // "
XBSprite.attr:	ds.w 1  // Bitfield; see PCG_ATTR macro
XBSprite.prio:	ds.w 1  // Priority relative to BG.
	                    // 0 - Sprite not drawn.
	                    // 1 - Sprite behind both tile planes.
	                    // 2 - Sprite behind BG0, but on top of BG1.
	                    // 3 - Sprite above both planes.
XBSprite.len:
#else
typedef struct
{
	uint16_t x;    // Limited to 10 bits of precision (0 - 1024)
	uint16_t y;    // "
	uint16_t attr; // Bitfield; see PCG_ATTR macro
	uint16_t prio; // Priority relative to BG.
	               // 0 - Sprite not drawn.
	               // 1 - Sprite behind both tile planes.
	               // 2 - Sprite behind BG0, but on top of BG1.
	               // 3 - Sprite above both planes.
} XBSprite;
#endif

// Setup =======================================================================

// This should match the lower eight bits of CRTC Register 20 (flags).
#define XB_PCG_MODE_LH    0x0010  // Set for 31khz (line double) mode.
#define XB_PCG_MODE_VMODE 0x0004  // Set for 512-line mode.
#define XB_PCG_MODE_HMODE 0x0001  // Set for 512-dot mode; enables 16x16 tiles.
#define XB_PCG_MODE_CUT   0x8000  // Disables output (?)

#define XB_PCG_MODE_HSIZE_256 0
#define XB_PCG_MODE_HSIZE_512 XB_BITVAL(0)

#define XB_PCG_MODE_VSIZE_256 0
#define XB_PCG_MODE_VSIZE_512 XB_BITVAL(2)

#ifndef __ASSEMBLER__

/*
Control:    0xEB0808
---- --9- ---- ---- D/C          Disable PCG for faster memory/register access
---- ---- --54 ---- BG1 TXsel    nametable mapping
---- ---- ---- 3--- BG1 ON
---- ---- ---- -21- BG0 TXsel    nametable mapping
---- ---- ---- ---0 BG0 ON
H-total:    0xEB080A
---- ---- 7654 3210 H total
H-disp:     0xEB080C
---- ---- --54 3210 H disp
V-disp:     0xEB080E
---- ---- 7654 3210 V disp
Mode:       0xEB0810
---- ---- ---4 ---- L/H          15KHz(0), 31Khz(1)
---- ---- ---- 32-- V-res
---- ---- ---- --10 H-res        If nonzero, enables 16x16 tiles

*/

// Configuration data is taken from the CRTC.
void xb_pcg_init(void);

// Turn off the display for faster transfer.
static inline void xb_pcg_set_blank(bool blank);

// Change the mappings for BG1 and BG0 nametables
static inline void xb_pcg_set_bg1_txsel(uint8_t t);
static inline void xb_pcg_set_bg0_txsel(uint8_t t);

// Enable or disable BG layer display
static inline void xb_pcg_set_bg1_enable(bool en);
static inline void xb_pcg_set_bg0_enable(bool en);

// Mode bits
static inline void xb_pcg_set_mode_line_double(bool dbl)
{
	volatile uint16_t *pcg_mode = (volatile uint16_t *)XB_PCG_MODE;
	uint16_t rdata = *pcg_mode;
	if (dbl) rdata |= XB_PCG_MODE_LH;
	else rdata &= ~(XB_PCG_MODE_LH);
	*pcg_mode = rdata;
}

static inline void xb_pcg_set_mode_vsize(bool hires)
{
	volatile uint16_t *pcg_mode = (volatile uint16_t *)XB_PCG_MODE;
	uint16_t rdata = *pcg_mode;
	rdata &= ~(0x03 << 2);
	if (hires) rdata |= XB_PCG_MODE_VSIZE_512;
	*pcg_mode = rdata;
}

static inline void xb_pcg_set_mode_hsize(bool hires)
{
	volatile uint16_t *pcg_mode = (volatile uint16_t *)XB_PCG_MODE;
	uint16_t rdata = *pcg_mode;
	rdata &= ~(0x03);
	if (hires) rdata |= XB_PCG_MODE_HSIZE_512;
	*pcg_mode = rdata;
}

// Manual timing register manipulation
static inline void xb_pcg_set_htotal(uint8_t htotal)
{
	volatile uint16_t *reg = (volatile uint16_t *)XB_PCG_HTOTAL;
	*reg = htotal;
}

static inline void xb_pcg_set_hdisp(uint8_t hdisp)
{
	volatile uint16_t *reg = (volatile uint16_t *)XB_PCG_HDISP;
	*reg = hdisp;
}

static inline void xb_pcg_set_vdisp(uint8_t vdisp)
{
	volatile uint16_t *reg = (volatile uint16_t *)XB_PCG_VDISP;
	*reg = vdisp;
}



// These functions are slower than just working with pointers to PCG_BGx_NAME,
// but may be helpful for debugging or small changes.
static inline void xb_pcg_set_bg0_tile(uint16_t x, uint16_t y, uint16_t attr);
static inline void xb_pcg_set_bg1_tile(uint16_t x, uint16_t y, uint16_t attr);

// Helper function for putting tiles in PCG VRAM.
void xb_pcg_transfer_pcg_data(const void *source, uint16_t dest_tile,
                              uint16_t num_tiles);

// Sets PCG registers based on CRTC (for custom modes).
void xb_pcg_set_crtc_regs(void);

// Static implementations ======================================================

// Set to CPU(1) or display(0)
static inline void xb_pcg_set_blank(bool blank)
{
	volatile uint16_t *pcg_ctrl_r = (volatile uint16_t *)XB_PCG_BG_CTRL;
	uint16_t rdata = *pcg_ctrl_r;
	if (blank) XB_BCLR(rdata, 9);
	else XB_BSET(rdata, 9);
	*pcg_ctrl_r = rdata;
}

// Change the mappings for BG1 and BG0 nametables
static inline void xb_pcg_set_bg0_txsel(uint8_t t)
{
	volatile uint16_t *pcg_ctrl_r = (volatile uint16_t *)XB_PCG_BG_CTRL;
	uint16_t rdata = *pcg_ctrl_r;
	rdata &= ~(0x0006);
	rdata |= (t & 0x03) << 1;
	*pcg_ctrl_r = rdata;
}

static inline void xb_pcg_set_bg1_txsel(uint8_t t)
{
	volatile uint16_t *pcg_ctrl_r = (volatile uint16_t *)XB_PCG_BG_CTRL;
	uint16_t rdata = *pcg_ctrl_r;
	rdata &= ~(0x0030);
	rdata |= (t & 0x03) << 4;
	*pcg_ctrl_r = rdata;
}

// Enable or disable BG layer display
static inline void xb_pcg_set_bg0_enable(bool en)
{
	volatile uint16_t *pcg_ctrl_r = (volatile uint16_t *)XB_PCG_BG_CTRL;
	uint16_t rdata = *pcg_ctrl_r;
	if (en) rdata |= 0x01;
	else rdata &= ~(0x01);
	*pcg_ctrl_r = rdata;
}

static inline void xb_pcg_set_bg1_enable(bool en)
{
	volatile uint16_t *pcg_ctrl_r = (volatile uint16_t *)XB_PCG_BG_CTRL;
	uint16_t rdata = *pcg_ctrl_r;
	if (en) rdata |= 0x08;
	else rdata &= ~(0x08);
	*pcg_ctrl_r = rdata;
}

// Sets a tile. Not recommended for drawing a background or much else beyond
// small changes or playing around, as it's slower than doing a large DMA.
static inline void xb_pcg_set_bg0_tile(uint16_t x, uint16_t y, uint16_t attr)
{
	volatile uint16_t *nt = (volatile uint16_t *)XB_PCG_BG0_NAME;
	nt += XB_PCG_BG_OFFS(x, y)/sizeof(uint16_t);
	*nt = attr;
}

static inline void xb_pcg_set_bg1_tile(uint16_t x, uint16_t y, uint16_t attr)
{
	volatile uint16_t *nt = (volatile uint16_t *)XB_PCG_BG1_NAME;
	nt += XB_PCG_BG_OFFS(x, y)/sizeof(uint16_t);
	*nt = attr;
}

#endif
