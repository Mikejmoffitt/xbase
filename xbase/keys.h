#ifndef XB_KEYS_H
#define XB_KEYS_H

#include <stdbool.h>
#include <stdint.h>

// Names for all keys on the X68000 keyboard.
// They are ordered by the group and bit data associated with iocs_bitsns().
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

// Bitfield for modifier keys that have been pressed.
// TODO: Mark as explicit uint16_t storage with C23
typedef enum XBKeyFlags
{
	XB_KEY_MOD_SHIFT     = 0x0001,  // Shift was held during this event.
	XB_KEY_MOD_CTRL      = 0x0002,  // Control was held during this event.
	XB_KEY_MOD_IS_REPEAT = 0x4000,  // This event comes from key repeat.
	XB_KEY_MOD_KEY_UP    = 0x8000,  // This event is a key release.
} XBKeyFlags;

// Struct representing a key event (key down / up).
typedef struct XBKeyEvent
{
	XBKey name;
	XBKeyFlags modifiers;
} XBKeyEvent;

//
// Initialization and Configuration
//

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

#endif  // XB_KEYS_H
