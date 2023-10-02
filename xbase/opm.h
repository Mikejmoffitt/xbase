/*

XBase Yamaha OPM / YM2151 Hardware Access Functions (opm)
c. Michael Moffitt 2021-2022

This file contains functions for interacting with the OPM sound source.
Register writes are written to a static cache, rather than being sent
immediately to the chip. A final commit function writes any changed
registers to the chip.

*/

#ifndef XB_OPM_H
#define XB_OPM_H

// TODO: Define without negative value shifted left
#define OPM_CLOCK_ADJUST ((-2 << 6) + 5)

#define XB_OPM_VOICE_COUNT 8
#define XB_OPM_OP_COUNT 4

#include <stdint.h>
#include <stdbool.h>

typedef enum __attribute__((packed)) XBOpmReg
{
	OPM_REG_TEST_MODE = 0x01,
	OPM_REG_LFO_RESET = 0x02,
	OPM_REG_KEY_ON = 0x08,
	OPM_REG_NOISE = 0x0F,
	OPM_REG_CLKA_UPPER = 0x10,
	OPM_REG_CLKA_LOWER = 0x11,
	OPM_REG_CLKB = 0x12,
	OPM_REG_TIMER_FLAGS = 0x14,
	OPM_REG_LFO_FREQ = 0x18,
	OPM_REG_LFO_DEPTH = 0x19,
	OPM_REG_CONTROL = 0x1B,
	// These registers are repeated 8 times each, for CH.A-CH.H.
	OPM_CH_PAN_FL_CON = 0x20,
	OPM_CH_OCT_NOTE = 0x28,  // AKA KC
	OPM_CH_KF = 0x30,
	OPM_CH_PMS_AMS = 0x38,
	// These registers are repeated 8 times in an inner set for the channels,
	// and then externally four times for the four operators.
	// For example, 0x40 is CH.A OP1; 0x47 is CH.H OP1. 0x48 is CH.A OP2, etc.
	OPM_CH_DT1_MUL = 0x40,
	OPM_CH_TL = 0x60,
	OPM_CH_KS_AR = 0x80,
	OPM_CH_AME_D1R = 0xA0,
	OPM_CH_DT2_D2R = 0xC0,
	OPM_CH_D1L_RR = 0xE0,

	OPM_REG_MAX = 0xFF
} XBOpmReg;

typedef enum __attribute__((packed)) XBOpmTimerFlag
{
	OPM_TIMER_FLAG_CSM       = 0x80,
	OPM_TIMER_FLAG_F_RESET_B = 0x20,
	OPM_TIMER_FLAG_F_RESET_A = 0x10,
	OPM_TIMER_FLAG_IRQ_EN_B  = 0x08,
	OPM_TIMER_FLAG_IRQ_EN_A  = 0x04,
	OPM_TIMER_FLAG_LOAD_B    = 0x02,
	OPM_TIMER_FLAG_LOAD_A    = 0x01,
} XBOpmTimerFlag;

typedef enum __attribute__((packed)) XBOpmLfoWave
{
	LFO_WAVE_SAW = 0x00,
	LFO_WAVE_RECTANGLE,
	LFO_WAVE_TRIANGLE,
	LFO_WAVE_RAND,
} XBOpmLfoWave;

typedef enum __attribute__((packed)) XBOpmPan
{
	OPM_PAN_NONE = 0x00,
	OPM_PAN_LEFT = 0x40,
	OPM_PAN_RIGHT = 0x80,
	OPM_PAN_BOTH = (OPM_PAN_LEFT | OPM_PAN_RIGHT),
} XBOpmPan;

typedef enum __attribute__((packed)) XBOpmNote
{
	OPM_NOTE_CS = 0x0,
	OPM_NOTE_D  = 0x1,
	OPM_NOTE_DS = 0x2,
	OPM_NOTE_E  = 0x4,
	OPM_NOTE_F  = 0x5,
	OPM_NOTE_FS = 0x6,
	OPM_NOTE_G  = 0x8,
	OPM_NOTE_GS = 0x9,
	OPM_NOTE_A  = 0xA,
	OPM_NOTE_AS = 0xC,
	OPM_NOTE_B  = 0xD,
	OPM_NOTE_C  = 0xE,
	// "invalid" notes
	OPM_NOTE_X1 = 0x7,
	OPM_NOTE_X2 = 0xB,
	OPM_NOTE_X3 = 0xF,
} XBOpmNote;

//
// Interface and config
//

uint8_t opm_status(void);
void xb_opm_set_test_mode(bool en);
void xb_opm_set_lfo_reset(bool en);

//
// Direct registers
//
// These registers are updated immediately.
//

// Writes a value immediately to the OPM and clears associated cached data.
void xb_opm_write(uint8_t addr, uint8_t data);

void xb_opm_set_key_on(uint8_t channel, uint8_t sn);

// Period:  0 - 1023
// Ta(sec) = (64 * (1024 - period)) / CLK
void xb_opm_set_clka_period(uint16_t period);

// Period:  0 - 255
// Tb(sec) = (1024 * (256 - period)) / CLK
void xb_opm_set_clkb_period(uint8_t period);
void xb_opm_set_timer_flags(XBOpmTimerFlag flags);

//
// Cached registers
//
// These functions are helpers that ultimately call xb_opm_set().
//

// Sets a cached register value. Changed data is sent in a batch to the OPM upon
// calling xb_opm_commit().
void xb_opm_set(uint8_t addr, uint8_t data);

// Call when you are done updating cached registers.
void xb_opm_commit(void);

// fnoise(Hz) = 4MHZ / (32 * nfreq)
// Nfreq:   0 - 31
void xb_opm_set_noise(bool en, uint8_t nfreq);
void xb_opm_set_lfo_freq(uint8_t freq);
// Depth:  0 - 127
void xb_opm_set_lfo_am_depth(uint8_t depth);
// Depth:  0 - 127
void xb_opm_set_lfo_pm_depth(uint8_t depth);
void xb_opm_set_control(bool ct1_adpcm_8mhz, bool ct2_fdc_ready,
                        XBOpmLfoWave lfo_wave);
// Channel Enable: Use OPM_PAN_*_ENABLE bitfield
// channel: 0 - 7
// FL:      0 - 7
// con:     0 - 7
void xb_opm_set_lr_fl_con(uint8_t channel, XBOpmPan pan, uint8_t fl,
                                        uint8_t con);
// Channel: 0 - 7
// Octave:  0 - 7
// Note:    XBOpmNote values (0 - F)
void xb_opm_set_oct_note(uint8_t channel, uint8_t octave, XBOpmNote note);
// Channel: 0 - 7
// kc:      Direct KC data
void xb_opm_set_kc(uint8_t channel, uint8_t kc);
// Channel:  0 - 7
// Fraction: 0 - 63
void xb_opm_set_key_fraction(uint8_t channel, uint8_t fraction);
// Channel: 0 - 7
// PMS:     0 - 7
// AMS:     0 - 3
void xb_opm_set_pms_ams(uint8_t channel, uint8_t pms, uint8_t ams);
// Channel: 0 - 7
// Op:      0 - 7
// DT1:     0 - 7
// Mul:     0 - 7
void xb_opm_set_dt1_mul(uint8_t channel, uint8_t op, uint8_t dt1, uint8_t mul);
// Channel: 0 - 7
// Op:      0 - 7
// TL:      0 - 127
void xb_opm_set_tl(uint8_t channel, uint8_t op, uint8_t tl);
// Channel: 0 - 7
// Op:      0 - 7
// KS:      0 - 3
// Ar:      0 - 31
void xb_opm_set_ks_ar(uint8_t channel, uint8_t op, uint8_t ks, uint8_t ar);
// Channel: 0 - 7
// Op:      0 - 7
// AME:     0 - 1
// D1R:     0 - 31
void xb_opm_set_ame_d1r(uint8_t channel, uint8_t op, uint8_t ame, uint8_t d1r);
// Channel: 0 - 7
// Op:      0 - 7
// DT2:     0 - 3
// D2R:     0 - 31
void xb_opm_set_dt2_d2r(uint8_t channel, uint8_t op, uint8_t dt2, uint8_t d2r);
// Channel: 0 - 7
// Op:      0 - 7
// D1L:     0 - 15
// RR:      0 - 15
void xb_opm_set_d1l_rr(uint8_t channel, uint8_t op, uint8_t d1l, uint8_t rr);

#endif  // XB_OPM_H
