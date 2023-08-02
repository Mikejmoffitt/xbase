#include "xbase/pcg.h"

#include "xbase/memmap.h"

#include <string.h>

static uint8_t s_spr_next = 0;
static uint8_t s_spr_count_prev = 0;

static uint16_t s_xb_pcg_ctrl_cache;
#define XB_PCG_CTRL_R (*(volatile uint16_t *)XB_PCG_BG_CTRL)

/*
Control:    0xEB0808
---- --9- ---- ---- D/C          turn on PCG to allow PCG register access
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

void xb_pcg_init(const XBPcgCfg *c)
{
	volatile uint16_t *pcg_reg = (volatile uint16_t *)XB_PCG_HTOTAL;
	pcg_reg[0] = c->htotal;
	pcg_reg[1] = c->hdisp;
	pcg_reg[2] = c->vdisp;
	pcg_reg[3] = c->mode;

	xb_pcg_set_disp_en(0);
	xb_pcg_set_bg0_txsel(0);
	xb_pcg_set_bg1_txsel(1);
	xb_pcg_set_bg0_enable(1);
	xb_pcg_set_bg1_enable(1);
	xb_pcg_set_bg0_xscroll(0);
	xb_pcg_set_bg1_xscroll(0);
	xb_pcg_set_bg0_yscroll(0);
	xb_pcg_set_bg1_yscroll(0);
	xb_pcg_clear_sprites();
	xb_pcg_set_disp_en(1);
}

// Set to CPU(1) or display(0)
void xb_pcg_set_disp_en(bool en)
{
	s_xb_pcg_ctrl_cache &= ~(0x0200);
	s_xb_pcg_ctrl_cache |= (en ? 0x0200 : 0x0000);
	XB_PCG_CTRL_R = s_xb_pcg_ctrl_cache;
}

// Change the mappings for BG1 and BG0 nametables
void xb_pcg_set_bg1_txsel(uint8_t t)
{
	s_xb_pcg_ctrl_cache &= ~(0x0030);
	s_xb_pcg_ctrl_cache |= (t & 0x03) << 4;
	XB_PCG_CTRL_R = s_xb_pcg_ctrl_cache;
}
void xb_pcg_set_bg0_txsel(uint8_t t)
{
	
	s_xb_pcg_ctrl_cache &= ~(0x0030);
	s_xb_pcg_ctrl_cache |= (t & 0x03) << 1;
	XB_PCG_CTRL_R = s_xb_pcg_ctrl_cache;
}

// Enable or disable BG layer display
void xb_pcg_set_bg1_enable(bool en)
{
	s_xb_pcg_ctrl_cache &= ~(0x0008);
	s_xb_pcg_ctrl_cache |= (en ? 0x08 : 0x000);
	XB_PCG_CTRL_R = s_xb_pcg_ctrl_cache;
}
void xb_pcg_set_bg0_enable(bool en)
{
	s_xb_pcg_ctrl_cache &= ~(0x0001);
	s_xb_pcg_ctrl_cache |= (en ? 0x001 : 0x000);
	XB_PCG_CTRL_R = s_xb_pcg_ctrl_cache;
}

void xb_pcg_add_sprite(int16_t x, int16_t y, uint16_t attr, uint16_t prio)
{
	volatile X68kPcgSprite *spr = xb_pcg_get_sprite(s_spr_next);
	while (s_spr_count_prev > 0)
	{
		s_spr_count_prev--;
		spr->prio = 0;
	}
	if (s_spr_next >= 128) return;
	xb_pcg_set_sprite(s_spr_next++, x, y, attr, prio);
}

void xb_pcg_finish_sprites(void)
{
	s_spr_count_prev = s_spr_next;
	s_spr_next = 0;
}

void xb_pcg_transfer_pcg_data(const void *source, uint16_t dest_tile,
                              uint16_t num_tiles)
{
	uint8_t *dest_addr = (uint8_t *)XB_PCG_TILE_DATA;
	dest_addr += (dest_tile * (32));  // 4bpp = 32 bytes per tile.
	memcpy(dest_addr, source, num_tiles * 32);
}

#undef XB_PCG_CTRL_R
