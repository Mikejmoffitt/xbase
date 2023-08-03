#include "xbase/opm.h"
#include "xbase/macro.h"
#include "xbase/memmap.h"
#include <stdio.h>

// Interface / direct

uint8_t opm_status(void)
{
	return *(volatile uint8_t *)(XB_OPM_BASE + 3);
}

void xb_opm_set_lfo_reset(bool en)
{
	xb_opm_write(OPM_REG_LFO_RESET, en ? 0x02 : 0x00);
}

void xb_opm_set_key_on(uint8_t channel, uint8_t sn)
{
	xb_opm_write(OPM_REG_KEY_ON, channel | (sn << 3));
}

void xb_opm_set_clka_period(uint16_t period)
{
	xb_opm_write(OPM_REG_CLKA_UPPER, period >> 2);
	xb_opm_write(OPM_REG_CLKA_LOWER, period & 0x03);
}

void xb_opm_set_clkb_period(uint8_t period)
{
	xb_opm_write(OPM_REG_CLKB, period);
}

void xb_opm_set_timer_flags(XBOpmTimerFlag flags)
{
	xb_opm_write(OPM_REG_TIMER_FLAGS, flags);
}

// Cached

void xb_opm_set_noise(bool en, uint8_t nfreq)
{
	xb_opm_write(OPM_REG_NOISE, (en ? 0x80 : 0x00) | nfreq);
}

void xb_opm_set_lfo_freq(uint8_t freq)
{
	xb_opm_write(OPM_REG_LFO_FREQ, freq);
}

void xb_opm_set_lfo_am_depth(uint8_t depth)
{
	xb_opm_write(OPM_REG_LFO_DEPTH, depth);
}

void xb_opm_set_lfo_pm_depth(uint8_t depth)
{
	xb_opm_write(OPM_REG_LFO_DEPTH, 0x80 | depth);
}

void xb_opm_set_control(bool ct1_adpcm_8mhz, bool ct2_fdc_ready,
                        XBOpmLfoWave lfo_wave)
{
	xb_opm_write(OPM_REG_CONTROL, (ct1_adpcm_8mhz ? 0x80 : 0x00) |
	                             (ct2_fdc_ready ? 0x40 : 0x00) |
	                             lfo_wave);
}

void xb_opm_set_lr_fl_con(uint8_t channel, XBOpmPan pan, uint8_t fl,
                          uint8_t con)
{
	xb_opm_write(OPM_CH_PAN_FL_CON + channel, pan | (fl << 3) | con);
}

void xb_opm_set_oct_note(uint8_t channel, uint8_t octave, XBOpmNote note)
{
	xb_opm_write(OPM_CH_OCT_NOTE + channel, note | (octave << 4 ));
}

void xb_opm_set_kc(uint8_t channel, uint8_t kc)
{
	xb_opm_write(OPM_CH_OCT_NOTE + channel, kc);
}

void xb_opm_set_key_fraction(uint8_t channel, uint8_t fraction)
{
	xb_opm_write(OPM_CH_KF + channel, fraction << 2);
}

void xb_opm_set_pms_ams(uint8_t channel, uint8_t pms, uint8_t ams)
{
	xb_opm_write(OPM_CH_PMS_AMS + channel, (pms << 4) | ams);
}

void xb_opm_set_dt1_mul(uint8_t channel, uint8_t op, uint8_t dt1, uint8_t mul)
{
	xb_opm_write(OPM_CH_DT1_MUL + channel + (8 * op), (dt1 << 4) | mul);
}

void xb_opm_set_tl(uint8_t channel, uint8_t op, uint8_t tl)
{
	xb_opm_write(OPM_CH_TL + channel + (8 * op), tl);
}

void xb_opm_set_ks_ar(uint8_t channel, uint8_t op, uint8_t ks, uint8_t ar)
{
	xb_opm_write(OPM_CH_KS_AR + channel + (8 * op), ar | (ks << 6));
}

void xb_opm_set_ame_d1r(uint8_t channel, uint8_t op, uint8_t ame, uint8_t d1r)
{
	xb_opm_write(OPM_CH_AME_D1R + channel + (8 * op), (ame ? 0x80 : 0x00) | d1r);
}

void xb_opm_set_dt2_d2r(uint8_t channel, uint8_t op, uint8_t dt2, uint8_t d2r)
{
	xb_opm_write(OPM_CH_DT2_D2R + channel + (8 * op), (dt2 << 6) | d2r);
}

void xb_opm_set_d1l_rr(uint8_t channel, uint8_t op, uint8_t d1l, uint8_t rr)
{
	xb_opm_write(OPM_CH_D1L_RR + channel + (8 * op), rr | (d1l << 4));
}
