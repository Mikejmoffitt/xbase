#pragma once

#ifndef __ASSEMBLER__
#include <stdbool.h>
#include <stdint.h>
#endif

#include "xbase/macro.h"


// Enums for all keys on the X68000 keyboard.
// They are ordered by the group and bit data associated with iocs_bitsns().
	XB_ENUMSTART
	XB_ENUMNEXT(XB_KEY_6)
	XB_ENUMNEXT(XB_KEY_5)
	XB_ENUMNEXT(XB_KEY_4)
	XB_ENUMNEXT(XB_KEY_3)
	XB_ENUMNEXT(XB_KEY_2)
	XB_ENUMNEXT(XB_KEY_1)
	XB_ENUMNEXT(XB_KEY_ESC)

	XB_ENUMNEXT(XB_KEY_BS)
	XB_ENUMNEXT(XB_KEY_PIPE)
	XB_ENUMNEXT(XB_KEY_TILDE)
	XB_ENUMNEXT(XB_KEY_MINUS)
	XB_ENUMNEXT(XB_KEY_0)
	XB_ENUMNEXT(XB_KEY_9)
	XB_ENUMNEXT(XB_KEY_8)
	XB_ENUMNEXT(XB_KEY_7)

	XB_ENUMNEXT(XB_KEY_U)
	XB_ENUMNEXT(XB_KEY_Y)
	XB_ENUMNEXT(XB_KEY_T)
	XB_ENUMNEXT(XB_KEY_R)
	XB_ENUMNEXT(XB_KEY_E)
	XB_ENUMNEXT(XB_KEY_W)
	XB_ENUMNEXT(XB_KEY_Q)
	XB_ENUMNEXT(XB_KEY_TAB)

	XB_ENUMNEXT(XB_KEY_S)
	XB_ENUMNEXT(XB_KEY_A)
	XB_ENUMNEXT(XB_KEY_CR)
	XB_ENUMNEXT(XB_KEY_OPEN_BRACKET)
	XB_ENUMNEXT(XB_KEY_AT)
	XB_ENUMNEXT(XB_KEY_P)
	XB_ENUMNEXT(XB_KEY_O)
	XB_ENUMNEXT(XB_KEY_I)

	XB_ENUMNEXT(XB_KEY_SEMICOLON)
	XB_ENUMNEXT(XB_KEY_L)
	XB_ENUMNEXT(XB_KEY_K)
	XB_ENUMNEXT(XB_KEY_J)
	XB_ENUMNEXT(XB_KEY_H)
	XB_ENUMNEXT(XB_KEY_G)
	XB_ENUMNEXT(XB_KEY_F)
	XB_ENUMNEXT(XB_KEY_D)

	XB_ENUMNEXT(XB_KEY_N)
	XB_ENUMNEXT(XB_KEY_B)
	XB_ENUMNEXT(XB_KEY_V)
	XB_ENUMNEXT(XB_KEY_C)
	XB_ENUMNEXT(XB_KEY_X)
	XB_ENUMNEXT(XB_KEY_Z)
	XB_ENUMNEXT(XB_KEY_CLOSED_BRACKET)
	XB_ENUMNEXT(XB_KEY_COLON)

	XB_ENUMNEXT(XB_KEY_DEL)
	XB_ENUMNEXT(XB_KEY_HOME)
	XB_ENUMNEXT(XB_KEY_SPACE)
	XB_ENUMNEXT(XB_KEY_UNDERSCORE)
	XB_ENUMNEXT(XB_KEY_FORWARD_SLASH)
	XB_ENUMNEXT(XB_KEY_PERIOD)
	XB_ENUMNEXT(XB_KEY_COMMA)
	XB_ENUMNEXT(XB_KEY_M)

	XB_ENUMNEXT(XB_KEY_CLR)
	XB_ENUMNEXT(XB_KEY_DOWN)
	XB_ENUMNEXT(XB_KEY_RIGHT)
	XB_ENUMNEXT(XB_KEY_UP)
	XB_ENUMNEXT(XB_KEY_LEFT)
	XB_ENUMNEXT(XB_KEY_UNDO)
	XB_ENUMNEXT(XB_KEY_R_DOWN)
	XB_ENUMNEXT(XB_KEY_R_UP)

	XB_ENUMNEXT(XB_KEY_NUMPAD_4)
	XB_ENUMNEXT(XB_KEY_NUMPAD_PLUS)
	XB_ENUMNEXT(XB_KEY_NUMPAD_9)
	XB_ENUMNEXT(XB_KEY_NUMPAD_8)
	XB_ENUMNEXT(XB_KEY_NUMPAD_7)
	XB_ENUMNEXT(XB_KEY_NUMPAD_MINUS)
	XB_ENUMNEXT(XB_KEY_NUMPAD_MULTIPLY)
	XB_ENUMNEXT(XB_KEY_NUMPAD_DIVIDE)

	XB_ENUMNEXT(XB_KEY_NUMPAD_0)
	XB_ENUMNEXT(XB_KEY_ENTER)
	XB_ENUMNEXT(XB_KEY_NUMPAD_3)
	XB_ENUMNEXT(XB_KEY_NUMPAD_2)
	XB_ENUMNEXT(XB_KEY_NUMPAD_1)
	XB_ENUMNEXT(XB_KEY_NUMPAD_EQUALS)
	XB_ENUMNEXT(XB_KEY_NUMPAD_6)
	XB_ENUMNEXT(XB_KEY_NUMPAD_5)

	XB_ENUMNEXT(XB_KEY_XF3)
	XB_ENUMNEXT(XB_KEY_XF2)
	XB_ENUMNEXT(XB_KEY_XF1)
	XB_ENUMNEXT(XB_KEY_HELP)
	XB_ENUMNEXT(XB_KEY_TOROKU)
	XB_ENUMNEXT(XB_KEY_KIGO)
	XB_ENUMNEXT(XB_KEY_NUMPAD_DECIMAL)
	XB_ENUMNEXT(XB_KEY_NUMPAD_COMMA)

	XB_ENUMNEXT(XB_KEY_HIRA)
	XB_ENUMNEXT(XB_KEY_INS)
	XB_ENUMNEXT(XB_KEY_CAPS)
	XB_ENUMNEXT(XB_KEY_CODE)
	XB_ENUMNEXT(XB_KEY_ROMA)
	XB_ENUMNEXT(XB_KEY_KANA)
	XB_ENUMNEXT(XB_KEY_XF5)
	XB_ENUMNEXT(XB_KEY_XF4)

	XB_ENUMNEXT(XB_KEY_F5)
	XB_ENUMNEXT(XB_KEY_F4)
	XB_ENUMNEXT(XB_KEY_F3)
	XB_ENUMNEXT(XB_KEY_F2)
	XB_ENUMNEXT(XB_KEY_F1)
	XB_ENUMNEXT(XB_KEY_COPY)
	XB_ENUMNEXT(XB_KEY_BREAK)
	XB_ENUMNEXT(XB_KEY_ZENKAKU)

	XB_ENUMNEXT(XB_KEY_F10)
	XB_ENUMNEXT(XB_KEY_F9)
	XB_ENUMNEXT(XB_KEY_F8)
	XB_ENUMNEXT(XB_KEY_F7)
	XB_ENUMNEXT(XB_KEY_F6)

	XB_ENUMNEXT(XB_KEY_OPT2)
	XB_ENUMNEXT(XB_KEY_OPT1)
	XB_ENUMNEXT(XB_KEY_CTRL)
	XB_ENUMNEXT(XB_KEY_SHIFT)

	XB_ENUMNEXT(XB_KEY_COUNT)

	XB_ENUMEND

#define XB_KEY_LIST_TERMINATE 0xFF

#define XB_KEY_MOD_SHIFT     XB_BITVAL(0)  // Shift was held during this event.
#define XB_KEY_MOD_CTRL      XB_BITVAL(1)  // Control was held during this event.
#define XB_KEY_MOD_IS_REPEAT XB_BITVAL(6)  // This event comes from key repeat.
#define XB_KEY_MOD_KEY_UP    XB_BITVAL(7)  // This event is a key release.

// Struct representing a key event (key down / up).
#ifdef __ASSEMBLER__
	.struct 0
XBKeyEvent.name:        ds.b 1
XBKeyEvent.modifiers:   ds.b 1
XBKeyEvent.len:
#else
typedef struct XBKeyEvent
{
	uint8_t name;
	uint8_t modifiers;
} XBKeyEvent;
#endif  // __ASSEMBLER__

#ifndef __ASSEMBLER__
typedef uint8_t XBKey;
#endif  // __ASSEMBLER__

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
void xb_keys_init(const uint8_t *sense_list);

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
bool xb_key_on(uint8_t key);
// Returns true if the key was just pressed in the last polling period.
bool xb_key_down(uint8_t key);
// Retruns true if the key was just released in the last polling period.
bool xb_key_up(uint8_t key);

#endif
