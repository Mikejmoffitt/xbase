/*

XBase Joypad Functions (joy)
c. Michael Moffitt 2021-2026

The X68000 has two controller ports for joypads. A standard joypad has a two-
dimensional directional input as well as two buttons, the same as on the MSX.

After initializing with xb_joy_init(), call xb_joy_poll() at the top of
your game or program's main loop. This function will capture the state of the
joypad, as well as generate variables which indicate a positive or negative
edge for each button (to detect the instant of a press or release).

Button data for both players is accessible through the XBJoy array
called g_xb_joystate. Data is valid after calling xb_joy_poll().

While the standard configuration as described before is the most typical, some
other controller variants exist:

* Standard:
 ___________________________________________
|      _                                    |
|     | |                                   |
|  ___| |___                                |
| |___   ___|                  ___    ___   |
|     | |                     |   |  |   |  |
|     |_|                     |___|  |___|  |
|___________________________________________|

* Extended (superset of Standard, fully compatible)
 ___________________________________________
|      _                                    |
|     | |                                   |
|  ___| |___                                |
| |___   ___|                  ___    ___   |
|     | |                     |   |  |   |  |
|     |_|      [SEL] [START]  |___|  |___|  |
|___________________________________________|

* CPSF-MD:
 ___________________________________________
|      _                      __   __   __  |
|     | |                    |  | |  | |  | |
|  ___| |___                 |__| |__| |__| |
| |___   ___|                 __   __   __  |
|     | |                    |  | |  | |  | |
|     |_|         [START]    |__| |__| |__| |
|___________________________________________|


Support for Standard/Extended is complete. Support for CPSF-MD six-button
controllers has not yet been implemented.

As the extended controller implements the start button with a left+right macro,
and select with an up+down macro, the direction inputs will be filtered out
when this "impossible" combination is detected, and the respective extended
button will be substituted. As a limitation of the design, it is not possible
to expect players to input left/right while also pressing the start key, nor
up/down while pressing the select key.

The default mode is XB_JOY_MODE_STANDARD.

*/
#pragma once

#ifndef __ASSEMBLER__
#include <stdint.h>
#endif

#include "xbase/macro.h"

#define XB_JOY_COUNT 2

// Bit numbers for the joy data.
#define XB_BTN_UP_BIT     0
#define XB_BTN_DOWN_BIT   1
#define XB_BTN_LEFT_BIT   2
#define XB_BTN_RIGHT_BIT  3
// bit 4 is skipped
#define XB_BTN_A_BIT      5
#define XB_BTN_B_BIT      6
// Extended additions.
#define XB_BTN_START_BIT  7
#define XB_BTN_SELECT_BIT 8
// CPSF-MD additions.
#define XB_BTN_C_BIT      9
#define XB_BTN_X_BIT     10
#define XB_BTN_Y_BIT     11
#define XB_BTN_Z_BIT     12

// Masks for use with bitwise operations (andi is faster than btst)
#define XB_BTN_UP     XB_BITVAL(XB_BTN_UP_BIT)
#define XB_BTN_DOWN   XB_BITVAL(XB_BTN_DOWN_BIT)
#define XB_BTN_LEFT   XB_BITVAL(XB_BTN_LEFT_BIT)
#define XB_BTN_RIGHT  XB_BITVAL(XB_BTN_RIGHT_BIT)
#define XB_BTN_A      XB_BITVAL(XB_BTN_A_BIT)
#define XB_BTN_B      XB_BITVAL(XB_BTN_B_BIT)
#define XB_BTN_START  XB_BITVAL(XB_BTN_START_BIT)
#define XB_BTN_SELECT XB_BITVAL(XB_BTN_SELECT_BIT)
#define XB_BTN_C      XB_BITVAL(XB_BTN_C_BIT)
#define XB_BTN_X      XB_BITVAL(XB_BTN_X_BIT)
#define XB_BTN_Y      XB_BITVAL(XB_BTN_Y_BIT)
#define XB_BTN_Z      XB_BITVAL(XB_BTN_Z_BIT)

#define XB_JOY_MODE_STANDARD 0x00
#define XB_JOY_MODE_6BUTTON  0x01

//
// Player state struct. Set mode to XB_JOY_MODE_* as desired, and test for
// inputs with bitwise operations on any fields after calling xb_joy_poll().
//
#ifdef __ASSEMBLER__
	.struct 0
XBJoy.mode:            ds.w 1
XBJoy.now:             ds.w 1
XBJoy.prev:            ds.w 1
XBJoy.pos:             ds.w 1
XBJoy.neg:             ds.w 1
XBJoy.len:

	.extern	g_xb_joystate
#else
typedef struct XBJoy
{
	uint16_t mode;  // XBJoyMode
	uint16_t now;   // The current data.
	uint16_t prev;  // The previous sampling period's data.
	uint16_t pos;   // Newly pressed buttons.
	uint16_t neg;   // Newly released buttons.
} XBJoy;

extern XBJoy g_xb_joystate[XB_JOY_COUNT];
#endif

#ifdef __ASSEMBLER__
	.global	xb_joy_init
	.global	xb_joy_poll
#else
void xb_joy_init(void);
void xb_joy_poll(void);
#endif
