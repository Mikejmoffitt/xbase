/*

XBase Joypad Functions (joy)
c. Michael Moffitt 2021-2022

The X68000 has two controller ports for joypads. A standard joypad has a two-
dimensional directional input as well as two buttons, the same as on the MSX.

After initializing with xb_joy_init(), call xb_joy_poll() once at the top of
your game or program;s main loop. This function will capture the state of the
joypad, as well as generate variables which indicate a positive or negative
edge for each button (to detect the instant of a press or release).

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

* Extended:
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


Support for Standard and Extended is complete. Support for CPSF-MD six-button
controllers has not yet been implemented.

As the Extended controller implements the start button with a left+right macro,
and select with an up+down macro, the direction inputs will be filtered out
when this "impossible" combination is detected, and the respective extended
button will be substituted. As a limitation of the design, it is not possible
to expect players to input left/right while also pressing the start key, nor
up/down while pressing the select key.

The default mode is XB_JOY_MODE_EXTENDED.

*/

#ifndef XBASE_JOY_H
#define XBASE_JOY_H

#include <stdint.h>

#define XB_JOY_COUNT 2

typedef enum __attribute__((packed)) XBJoyBits
{
	XB_KEY_UP = 0x0001,
	XB_KEY_DOWN = 0x0002,
	XB_KEY_LEFT = 0x0004,
	XB_KEY_RIGHT = 0x0008,
	XB_KEY_A = 0x0010,
	XB_KEY_B = 0x0020,
	// Extended additions.
	XB_KEY_START = 0x0040,
	XB_KEY_SELECT = 0x0080,
	// CPSF-MD additions.
	XB_KEY_C = 0x0100,
	XB_KEY_X = 0x0200,
	XB_KEY_Y = 0x0400,
	XB_KEY_Z = 0x0800,
} XBJoyBits;

typedef enum __attribute__((packed)) XBJoyMode
{
	XB_JOY_MODE_STANDARD,
	XB_JOY_MODE_EXTENDED,  // Backwards compatible with Standard.
	// TODO: XB_JOY_MODE_6BUTTON
} XBJoyMode;

// Joypad state. Data is valid after a call to xb_joy_poll().
extern XBJoyBits g_xb_joy[XB_JOY_COUNT];       // Key data from the last poll.
extern XBJoyBits g_xb_joy_pos[XB_JOY_COUNT];   // Keys just pressed.
extern XBJoyBits g_xb_joy_neg[XB_JOY_COUNT];   // Keys just released.
extern XBJoyBits g_xb_joy_prev[XB_JOY_COUNT];  // The previous poll's keys.

// Clears joypad state, and sets controller type per player.
void xb_joy_init(uint8_t player, XBJoyMode mode);

// Samples inputs and calculates edges. Call once before game logic.
void xb_joy_poll(void);

// Accessors for joypad data.
static inline XBJoyBits xb_joy(uint8_t player) { return g_xb_joy[player]; }
static inline XBJoyBits xb_joy_pos(uint8_t player) { return g_xb_joy_pos[player]; }
static inline XBJoyBits xb_joy_neg(uint8_t player) { return g_xb_joy_neg[player]; }
static inline XBJoyBits xb_joy_prev(uint8_t player) { return g_xb_joy_prev[player]; }

#endif // _X68K_JOY_H
