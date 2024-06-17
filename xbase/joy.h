/*

XBase Joypad Functions (joy)
c. Michael Moffitt 2021-2022

The X68000 has two controller ports for joypads. A standard joypad has a two-
dimensional directional input as well as two buttons, the same as on the MSX.

After initializing with xb_joy_init(), call xb_joy_poll() at the top of
your game or program's main loop. This function will capture the state of the
joypad, as well as generate variables which indicate a positive or negative
edge for each button (to detect the instant of a press or release).

Button data for both players is accessible through the XBJoyState array
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

#define XB_JOY_COUNT 2

// Bit numbers for the joy data.
#define XB_JOY_UP     0
#define XB_JOY_DOWN   1
#define XB_JOY_LEFT   2
#define XB_JOY_RIGHT  3
// bit 4 is skipped
#define XB_JOY_A      5
#define XB_JOY_B      6
// Extended additions.
#define XB_JOY_START  7
#define XB_JOY_SELECT 8
// CPSF-MD additions.
#define XB_JOY_C      9
#define XB_JOY_X     10
#define XB_JOY_Y     11
#define XB_JOY_Z     12

// Masks for use with bitwise operations (andi is faster than btst)
#define XB_JOY_MASK_UP     BITVAL(XB_JOY_UP)
#define XB_JOY_MASK_DOWN   BITVAL(XB_JOY_DOWN)
#define XB_JOY_MASK_LEFT   BITVAL(XB_JOY_LEFT)
#define XB_JOY_MASK_RIGHT  BITVAL(XB_JOY_RIGHT)
#define XB_JOY_MASK_A      BITVAL(XB_JOY_A)
#define XB_JOY_MASK_B      BITVAL(XB_JOY_B)
#define XB_JOY_MASK_START  BITVAL(XB_JOY_START)
#define XB_JOY_MASK_SELECT BITVAL(XB_JOY_SELECT)
#define XB_JOY_MASK_C      BITVAL(XB_JOY_C)
#define XB_JOY_MASK_X      BITVAL(XB_JOY_X)
#define XB_JOY_MASK_Y      BITVAL(XB_JOY_Y)
#define XB_JOY_MASK_Z      BITVAL(XB_JOY_Z)

#define XB_JOY_MODE_STANDARD 0x00
#define XB_JOY_MODE_6BUTTON  0x01

//
// Player state struct. Set mode to XB_JOY_MODE_* as desired, and test for
// inputs with bitwise operations on any fields after calling xb_joy_poll().
//
#ifdef __ASSEMBLER__
	.struct 0
XBJoyState.mode:	ds.w 1
XBJoyState.now:		ds.w 1
XBJoyState.prev:	ds.w 1
XBJoyState.pos:		ds.w 1
XBJoyState.neg:		ds.w 1
XBJoyState.len:

	.extern	g_xb_joystate
#else
typedef struct XBJoyState
{
	uint16_t mode;  // XBJoyMode
	uint16_t now;   // The current data.
	uint16_t prev;  // The previous sampling period's data.
	uint16_t pos;   // Newly pressed buttons.
	uint16_t neg;   // Newly released buttons.
} XBJoyState;

extern XBJoyState g_xb_joystate[XB_JOY_COUNT];
#endif

#ifdef __ASSEMBLER__
	.global	xb_joy_init
	.global	xb_joy_poll
#else
void xb_joy_init(void);
void xb_joy_poll(void);
#endif
