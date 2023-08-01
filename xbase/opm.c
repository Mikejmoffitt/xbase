#include "xbase/opm.h"
#include "xbase/macro.h"
#include "xbase/memmap.h"
#include <stdio.h>

// Register cache / interface.

static uint8_t s_regs[0x100];
static bool s_dirty[0x100];  // If true, corresponding s_reg value needs a copy

static inline void opm_write(uint8_t address, uint8_t data)
{
	*(volatile uint8_t *)(XB_OPM_BASE + 1) = address;
	while (opm_status() & 0x80) __asm__ volatile ("nop");
	*(volatile uint8_t *)(XB_OPM_BASE + 3) = data;
}

static inline void reg_cache_w(uint8_t address, uint8_t data)
{
	s_dirty[address] = s_regs[address] != data;
	s_regs[address] = data;
}

void xb_opm_commit(void)
{
	// Not all low value regs are used
	static const uint8_t kreglist[] =
	{
		0x0F, 0x18, 0x19, 0x1B
	};
	for (uint16_t i = 0; i < ARRAYSIZE(kreglist); i++)
	{
		const uint8_t idx = kreglist[i];
		if (!s_dirty[idx]) continue;
		opm_write(idx, s_regs[idx]);
		s_dirty[idx] = false;
	}

	// Update higher value cached regs
	for (uint16_t i = 0x20; i < ARRAYSIZE(s_regs); i++)
	{
		const uint8_t idx = i;
		if (!s_dirty[idx]) continue;
		opm_write(idx, s_regs[idx]);
		s_dirty[idx] = false;
	}
}

// Interface / direct

uint8_t opm_status(void)
{
	return *(volatile uint8_t *)(XB_OPM_BASE + 3);
}

void xb_opm_set_lfo_reset(bool en)
{
	opm_write(OPM_REG_LFO_RESET, en ? 0x02 : 0x00);
}

void xb_opm_set_key_on(uint8_t channel, uint8_t sn)
{
	opm_write(OPM_REG_KEY_ON, channel | (sn << 3));
}

void xb_opm_set_clka_period(uint16_t period)
{
	opm_write(OPM_REG_CLKA_UPPER, period >> 2);
	opm_write(OPM_REG_CLKA_LOWER, period & 0x03);
}

void xb_opm_set_clkb_period(uint8_t period)
{
	opm_write(OPM_REG_CLKB, period);
}

void xb_opm_set_timer_flags(XBOpmTimerFlag flags)
{
	opm_write(OPM_REG_TIMER_FLAGS, flags);
}

// Cached

void xb_opm_set_noise(bool en, uint8_t nfreq)
{
	reg_cache_w(OPM_REG_NOISE, (en ? 0x80 : 0x00) | nfreq);
}

void xb_opm_set_lfo_freq(uint8_t freq)
{
	reg_cache_w(OPM_REG_LFO_FREQ, freq);
}

void xb_opm_set_lfo_am_depth(uint8_t depth)
{
	reg_cache_w(OPM_REG_LFO_DEPTH, depth);
}

void xb_opm_set_lfo_pm_depth(uint8_t depth)
{
	reg_cache_w(OPM_REG_LFO_DEPTH, 0x80 | depth);
}

void xb_opm_set_control(bool ct1_adpcm_8mhz, bool ct2_fdc_ready,
                        XBOpmLfoWave lfo_wave)
{
	reg_cache_w(OPM_REG_CONTROL, (ct1_adpcm_8mhz ? 0x80 : 0x00) |
	                             (ct2_fdc_ready ? 0x40 : 0x00) |
	                             lfo_wave);
}

void xb_opm_set_lr_fl_con(uint8_t channel, XBOpmPan pan, uint8_t fl,
                          uint8_t con)
{
	reg_cache_w(OPM_CH_PAN_FL_CON + channel, pan | (fl << 3) | con);
}

void xb_opm_set_oct_note(uint8_t channel, uint8_t octave, XBOpmNote note)
{
	reg_cache_w(OPM_CH_OCT_NOTE + channel, note | (octave << 4 ));
}

void xb_opm_set_kc(uint8_t channel, uint8_t kc)
{
	reg_cache_w(OPM_CH_OCT_NOTE + channel, kc);
}

void xb_opm_set_key_fraction(uint8_t channel, uint8_t fraction)
{
	reg_cache_w(OPM_CH_KF + channel, fraction << 2);
}

void xb_opm_set_pms_ams(uint8_t channel, uint8_t pms, uint8_t ams)
{
	reg_cache_w(OPM_CH_PMS_AMS + channel, (pms << 4) | ams);
}

void xb_opm_set_dt1_mul(uint8_t channel, uint8_t op, uint8_t dt1, uint8_t mul)
{
	reg_cache_w(OPM_CH_DT1_MUL + channel + (8 * op), (dt1 << 4) | mul);
}

void xb_opm_set_tl(uint8_t channel, uint8_t op, uint8_t tl)
{
	reg_cache_w(OPM_CH_TL + channel + (8 * op), tl);
}

void xb_opm_set_ks_ar(uint8_t channel, uint8_t op, uint8_t ks, uint8_t ar)
{
	reg_cache_w(OPM_CH_KS_AR + channel + (8 * op), ar | (ks << 6));
}

void xb_opm_set_ame_d1r(uint8_t channel, uint8_t op, uint8_t ame, uint8_t d1r)
{
	reg_cache_w(OPM_CH_AME_D1R + channel + (8 * op), (ame ? 0x80 : 0x00) | d1r);
}

void xb_opm_set_dt2_d2r(uint8_t channel, uint8_t op, uint8_t dt2, uint8_t d2r)
{
	reg_cache_w(OPM_CH_DT2_D2R + channel + (8 * op), (dt2 << 6) | d2r);
}

void xb_opm_set_d1l_rr(uint8_t channel, uint8_t op, uint8_t d1l, uint8_t rr)
{
	reg_cache_w(OPM_CH_D1L_RR + channel + (8 * op), rr | (d1l << 4));
}
