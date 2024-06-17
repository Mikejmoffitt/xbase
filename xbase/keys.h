#pragma once

#ifndef __ASSEMBLER__
#include <stdbool.h>
#include <stdint.h>
#endif

// Enums for all keys on the X68000 keyboard.
// They are ordered by the group and bit data associated with iocs_bitsns().
#ifdef __ASSEMBLER__
	.struct	0
XB_KEY_6:				ds.b 1
XB_KEY_5:				ds.b 1
XB_KEY_4:				ds.b 1
XB_KEY_3:				ds.b 1
XB_KEY_2:				ds.b 1
XB_KEY_1:				ds.b 1
XB_KEY_ESC:				ds.b 1

XB_KEY_BS:				ds.b 1
XB_KEY_PIPE:			ds.b 1
XB_KEY_TILDE:			ds.b 1
XB_KEY_MINUS:			ds.b 1
XB_KEY_0:				ds.b 1
XB_KEY_9:				ds.b 1
XB_KEY_8:				ds.b 1
XB_KEY_7:				ds.b 1

XB_KEY_U:				ds.b 1
XB_KEY_Y:				ds.b 1
XB_KEY_T:				ds.b 1
XB_KEY_R:				ds.b 1
XB_KEY_E:				ds.b 1
XB_KEY_W:				ds.b 1
XB_KEY_Q:				ds.b 1
XB_KEY_TAB:				ds.b 1

XB_KEY_S:				ds.b 1
XB_KEY_A:				ds.b 1
XB_KEY_CR:				ds.b 1
XB_KEY_OPEN_BRACKET:	ds.b 1
XB_KEY_AT:				ds.b 1
XB_KEY_P:				ds.b 1
XB_KEY_O:				ds.b 1
XB_KEY_I:				ds.b 1

XB_KEY_SEMICOLON:		ds.b 1
XB_KEY_L:				ds.b 1
XB_KEY_K:				ds.b 1
XB_KEY_J:				ds.b 1
XB_KEY_H:				ds.b 1
XB_KEY_G:				ds.b 1
XB_KEY_F:				ds.b 1
XB_KEY_D:				ds.b 1

XB_KEY_N:				ds.b 1
XB_KEY_B:				ds.b 1
XB_KEY_V:				ds.b 1
XB_KEY_C:				ds.b 1
XB_KEY_X:				ds.b 1
XB_KEY_Z:				ds.b 1
XB_KEY_CLOSED_BRACKET:	ds.b 1
XB_KEY_COLON:			ds.b 1

XB_KEY_DEL:				ds.b 1
XB_KEY_HOME:			ds.b 1
XB_KEY_SPACE:			ds.b 1
XB_KEY_UNDERSCORE:		ds.b 1
XB_KEY_FORWARD_SLASH:	ds.b 1
XB_KEY_PERIOD:			ds.b 1
XB_KEY_COMMA:			ds.b 1
XB_KEY_M:				ds.b 1

XB_KEY_CLR:				ds.b 1
XB_KEY_DOWN:			ds.b 1
XB_KEY_RIGHT:			ds.b 1
XB_KEY_UP:				ds.b 1
XB_KEY_LEFT:			ds.b 1
XB_KEY_UNDO:			ds.b 1
XB_KEY_R_DOWN:			ds.b 1
XB_KEY_R_UP:			ds.b 1

XB_KEY_NUMPAD_4:		ds.b 1
XB_KEY_NUMPAD_PLUS:		ds.b 1
XB_KEY_NUMPAD_9:		ds.b 1
XB_KEY_NUMPAD_8:		ds.b 1
XB_KEY_NUMPAD_7:		ds.b 1
XB_KEY_NUMPAD_MINUS:	ds.b 1
XB_KEY_NUMPAD_MULTIPLY:	ds.b 1
XB_KEY_NUMPAD_DIVIDE:	ds.b 1

XB_KEY_NUMPAD_0:		ds.b 1
XB_KEY_ENTER:			ds.b 1
XB_KEY_NUMPAD_3:		ds.b 1
XB_KEY_NUMPAD_2:		ds.b 1
XB_KEY_NUMPAD_1:		ds.b 1
XB_KEY_NUMPAD_EQUALS:	ds.b 1
XB_KEY_NUMPAD_6:		ds.b 1
XB_KEY_NUMPAD_5:		ds.b 1

XB_KEY_XF3:				ds.b 1
XB_KEY_XF2:				ds.b 1
XB_KEY_XF1:				ds.b 1
XB_KEY_HELP:			ds.b 1
XB_KEY_TOROKU:			ds.b 1
XB_KEY_KIGO:			ds.b 1
XB_KEY_NUMPAD_DECIMAL:	ds.b 1
XB_KEY_NUMPAD_COMMA:	ds.b 1

XB_KEY_HIRA:			ds.b 1
XB_KEY_INS:				ds.b 1
XB_KEY_CAPS:			ds.b 1
XB_KEY_CODE:			ds.b 1
XB_KEY_ROMA:			ds.b 1
XB_KEY_KANA:			ds.b 1
XB_KEY_XF5:				ds.b 1
XB_KEY_XF4:				ds.b 1

XB_KEY_F5:				ds.b 1
XB_KEY_F4:				ds.b 1
XB_KEY_F3:				ds.b 1
XB_KEY_F2:				ds.b 1
XB_KEY_F1:				ds.b 1
XB_KEY_COPY:			ds.b 1
XB_KEY_BREAK:			ds.b 1
XB_KEY_ZENKAKU:			ds.b 1

XB_KEY_F10:				ds.b 1
XB_KEY_F9:				ds.b 1
XB_KEY_F8:				ds.b 1
XB_KEY_F7:				ds.b 1
XB_KEY_F6:				ds.b 1

XB_KEY_OPT2:			ds.b 1
XB_KEY_OPT1:			ds.b 1
XB_KEY_CTRL:			ds.b 1
XB_KEY_SHIFT:			ds.b 1

XB_KEY_INVALID:
#else
typedef enum XBKey
{
	XB_KEY_6,
	XB_KEY_5,
	XB_KEY_4,
	XB_KEY_3,
	XB_KEY_2,
	XB_KEY_1,
	XB_KEY_ESC,

	XB_KEY_BS,
	XB_KEY_PIPE,
	XB_KEY_TILDE,
	XB_KEY_MINUS,
	XB_KEY_0,
	XB_KEY_9,
	XB_KEY_8,
	XB_KEY_7,

	XB_KEY_U,
	XB_KEY_Y,
	XB_KEY_T,
	XB_KEY_R,
	XB_KEY_E,
	XB_KEY_W,
	XB_KEY_Q,
	XB_KEY_TAB,

	XB_KEY_S,
	XB_KEY_A,
	XB_KEY_CR,
	XB_KEY_OPEN_BRACKET,
	XB_KEY_AT,
	XB_KEY_P,
	XB_KEY_O,
	XB_KEY_I,

	XB_KEY_SEMICOLON,
	XB_KEY_L,
	XB_KEY_K,
	XB_KEY_J,
	XB_KEY_H,
	XB_KEY_G,
	XB_KEY_F,
	XB_KEY_D,

	XB_KEY_N,
	XB_KEY_B,
	XB_KEY_V,
	XB_KEY_C,
	XB_KEY_X,
	XB_KEY_Z,
	XB_KEY_CLOSED_BRACKET,
	XB_KEY_COLON,

	XB_KEY_DEL,
	XB_KEY_HOME,
	XB_KEY_SPACE,
	XB_KEY_UNDERSCORE,
	XB_KEY_FORWARD_SLASH,
	XB_KEY_PERIOD,
	XB_KEY_COMMA,
	XB_KEY_M,

	XB_KEY_CLR,
	XB_KEY_DOWN,
	XB_KEY_RIGHT,
	XB_KEY_UP,
	XB_KEY_LEFT,
	XB_KEY_UNDO,
	XB_KEY_R_DOWN,
	XB_KEY_R_UP,

	XB_KEY_NUMPAD_4,
	XB_KEY_NUMPAD_PLUS,
	XB_KEY_NUMPAD_9,
	XB_KEY_NUMPAD_8,
	XB_KEY_NUMPAD_7,
	XB_KEY_NUMPAD_MINUS,
	XB_KEY_NUMPAD_MULTIPLY,
	XB_KEY_NUMPAD_DIVIDE,

	XB_KEY_NUMPAD_0,
	XB_KEY_ENTER,
	XB_KEY_NUMPAD_3,
	XB_KEY_NUMPAD_2,
	XB_KEY_NUMPAD_1,
	XB_KEY_NUMPAD_EQUALS,
	XB_KEY_NUMPAD_6,
	XB_KEY_NUMPAD_5,

	XB_KEY_XF3,
	XB_KEY_XF2,
	XB_KEY_XF1,
	XB_KEY_HELP,
	XB_KEY_TOROKU,
	XB_KEY_KIGO,
	XB_KEY_NUMPAD_DECIMAL,
	XB_KEY_NUMPAD_COMMA,

	XB_KEY_HIRA,
	XB_KEY_INS,
	XB_KEY_CAPS,
	XB_KEY_CODE,
	XB_KEY_ROMA,
	XB_KEY_KANA,
	XB_KEY_XF5,
	XB_KEY_XF4,
	
	XB_KEY_F5,
	XB_KEY_F4,
	XB_KEY_F3,
	XB_KEY_F2,
	XB_KEY_F1,
	XB_KEY_COPY,
	XB_KEY_BREAK,
	XB_KEY_ZENKAKU,

	XB_KEY_F10,
	XB_KEY_F9,
	XB_KEY_F8,
	XB_KEY_F7,
	XB_KEY_F6,

	XB_KEY_OPT2,
	XB_KEY_OPT1,
	XB_KEY_CTRL,
	XB_KEY_SHIFT,

	XB_KEY_INVALID,
} XBKey;
#endif

#define XB_KEY_MOD_SHIFT     0x0001  // Shift was held during this event.
#define XB_KEY_MOD_CTRL      0x0002  // Control was held during this event.
#define XB_KEY_MOD_IS_REPEAT 0x4000  // This event comes from key repeat.
#define XB_KEY_MOD_KEY_UP    0x8000  // This event is a key release.

// Struct representing a key event (key down / up).
#ifdef __ASSEMBLER__
	.struct 0
XBKeyEvent.name:		ds.w 1
XBKeyEvent.modifiers:	ds.w 1
XBKeyEvent.len:
#else
typedef struct XBKeyEvent
{
	uint16_t name;
	uint16_t modifiers;
} XBKeyEvent;
#endif

//
// Initialization and Configuration
//

#ifdef __ASSEMBLER__

	.global	xb_keys_init
	.global	xb_keys_set_repeat
	.global	xb_keys_poll
	.global	xb_keys_event_pop
	.global	xb_key_on
	.global	xb_key_down
	.global	xb_key_up
#else
// Initialize key scanner.
// sense_list points to an array of XB_KEY_* values, indicating which keys
// should be scanned for key up/down event generation. The list is terminated
// with an XB_KEY_INVALID entry.
// Passing in NULL will default to all keys being scanned.
void xb_keys_init(const XBKey *sense_list);

// Set the key repeat delay and rate in terms of poll() periods.
// Pass a number <= 0 to reset to the default value.
void xb_keys_set_repeat(int16_t delay, int16_t rate);

//
// Main Interface
//

// Scans the keyboard matrix and populates internal key state data.
// It is recommended that this is executed at the start of your main loop.
void xb_keys_poll(void);

// Returns true if a previously pending key event was written to out.
// Otherwise, returns false.
bool xb_keys_event_pop(XBKeyEvent *out);

//
// Key status checks
//

// Returns true if the key is currently held down.
bool xb_key_on(XBKey key);
// Returns true if the key was just pressed in the last polling period.
bool xb_key_down(XBKey key);
// Retruns true if the key was just released in the last polling period.
bool xb_key_up(XBKey key);

#endif
