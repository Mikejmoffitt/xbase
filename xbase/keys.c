#include "xbase/keys.h"
#include "macro.h"

#include <string.h>
#include <iocs.h>

#define XB_KEYS_DEFAULT_REPEAT_DELAY 12
#define XB_KEYS_DEFAULT_REPEAT_RATE 1
#define XB_KEYS_KEY_EVENT_QUEUE_DEPTH 32

typedef struct XBKeys
{
	// State of all keys in iocs_bitsns() bitfield format
	uint8_t key_bits[15];
	uint8_t key_bits_prev[15];

	// Key event FIFO
	XBKeyEvent key_events[XB_KEYS_KEY_EVENT_QUEUE_DEPTH];
	uint16_t key_w;  // Next write index
	uint16_t key_r;  // Next read index

	// Key repeat state
	XBKey repeat_key;      // Key name for currently repeating key
	int16_t repeat_cnt;     // Counter that increments when key is held
	int16_t repeat_delay;   // Delay setting
	int16_t repeat_period;  // Repeat rate setting

	// Key sensitivity list
	XBKey sense_list[XB_KEY_INVALID];
	uint16_t sense_list_size;
} XBKeys;

static XBKeys s_keys;

//
// Group and mask data for key enum values.
//

typedef struct XBKeyID
{
	uint8_t group;
	uint8_t mask;
} XBKeyID;

static const XBKeyID kkey_table[] =
{
	[XB_KEY_6] = {0x0, 0x80},
	[XB_KEY_5] = {0x0, 0x40},
	[XB_KEY_4] = {0x0, 0x20},
	[XB_KEY_3] = {0x0, 0x10},
	[XB_KEY_2] = {0x0, 0x08},
	[XB_KEY_1] = {0x0, 0x04},
	[XB_KEY_ESC] = {0x0, 0x02},

	[XB_KEY_BS] = {0x1, 0x80},
	[XB_KEY_PIPE] = {0x1, 0x40},
	[XB_KEY_TILDE] = {0x1, 0x20},
	[XB_KEY_MINUS] = {0x1, 0x10},
	[XB_KEY_0] = {0x1, 0x08},
	[XB_KEY_9] = {0x1, 0x04},
	[XB_KEY_8] = {0x1, 0x02},
	[XB_KEY_7] = {0x1, 0x01},

	[XB_KEY_U] = {0x2, 0x80},
	[XB_KEY_Y] = {0x2, 0x40},
	[XB_KEY_T] = {0x2, 0x20},
	[XB_KEY_R] = {0x2, 0x10},
	[XB_KEY_E] = {0x2, 0x08},
	[XB_KEY_W] = {0x2, 0x04},
	[XB_KEY_Q] = {0x2, 0x02},
	[XB_KEY_TAB] = {0x2, 0x01},

	[XB_KEY_S] = {0x3, 0x80},
	[XB_KEY_A] = {0x3, 0x40},
	[XB_KEY_CR] = {0x3, 0x20},
	[XB_KEY_OPEN_BRACKET] = {0x3, 0x10},
	[XB_KEY_AT] = {0x3, 0x08},
	[XB_KEY_P] = {0x3, 0x04},
	[XB_KEY_O] = {0x3, 0x02},
	[XB_KEY_I] = {0x3, 0x01},

	[XB_KEY_SEMICOLON] = {0x4, 0x80},
	[XB_KEY_L] = {0x4, 0x40},
	[XB_KEY_K] = {0x4, 0x20},
	[XB_KEY_J] = {0x4, 0x10},
	[XB_KEY_H] = {0x4, 0x08},
	[XB_KEY_G] = {0x4, 0x04},
	[XB_KEY_F] = {0x4, 0x02},
	[XB_KEY_D] = {0x4, 0x01},

	[XB_KEY_N] = {0x5, 0x80},
	[XB_KEY_B] = {0x5, 0x40},
	[XB_KEY_V] = {0x5, 0x20},
	[XB_KEY_C] = {0x5, 0x10},
	[XB_KEY_X] = {0x5, 0x08},
	[XB_KEY_Z] = {0x5, 0x04},
	[XB_KEY_CLOSED_BRACKET] = {0x5, 0x02},
	[XB_KEY_COLON] = {0x5, 0x01},

	[XB_KEY_DEL] = {0x6, 0x80},
	[XB_KEY_HOME] = {0x6, 0x40},
	[XB_KEY_SPACE] = {0x6, 0x20},
	[XB_KEY_UNDERSCORE] = {0x6, 0x10},
	[XB_KEY_FORWARD_SLASH] = {0x6, 0x08},
	[XB_KEY_PERIOD] = {0x6, 0x04},
	[XB_KEY_COMMA] = {0x6, 0x02},
	[XB_KEY_M] = {0x6, 0x01},

	[XB_KEY_CLR] = {0x7, 0x80},
	[XB_KEY_DOWN] = {0x7, 0x40},
	[XB_KEY_RIGHT] = {0x7, 0x20},
	[XB_KEY_UP] = {0x7, 0x10},
	[XB_KEY_LEFT] = {0x7, 0x08},
	[XB_KEY_UNDO] = {0x7, 0x04},
	[XB_KEY_R_DOWN] = {0x7, 0x02},
	[XB_KEY_R_UP] = {0x7, 0x01},

	[XB_KEY_NUMPAD_4] = {0x8, 0x80},
	[XB_KEY_NUMPAD_PLUS] = {0x8, 0x40},
	[XB_KEY_NUMPAD_9] = {0x8, 0x20},
	[XB_KEY_NUMPAD_8] = {0x8, 0x10},
	[XB_KEY_NUMPAD_7] = {0x8, 0x08},
	[XB_KEY_NUMPAD_MINUS] = {0x8, 0x04},
	[XB_KEY_NUMPAD_MULTIPLY] = {0x8, 0x02},
	[XB_KEY_NUMPAD_DIVIDE] = {0x8, 0x01},

	[XB_KEY_NUMPAD_0] = {0x9, 0x80},
	[XB_KEY_ENTER] = {0x9, 0x40},
	[XB_KEY_NUMPAD_3] = {0x9, 0x20},
	[XB_KEY_NUMPAD_2] = {0x9, 0x10},
	[XB_KEY_NUMPAD_1] = {0x9, 0x08},
	[XB_KEY_NUMPAD_EQUALS] = {0x9, 0x04},
	[XB_KEY_NUMPAD_6] = {0x9, 0x02},
	[XB_KEY_NUMPAD_5] = {0x9, 0x01},

	[XB_KEY_XF3] = {0xA, 0x80},
	[XB_KEY_XF2] = {0xA, 0x40},
	[XB_KEY_XF1] = {0xA, 0x20},
	[XB_KEY_HELP] = {0xA, 0x10},
	[XB_KEY_TOROKU] = {0xA, 0x08},
	[XB_KEY_KIGO] = {0xA, 0x04},
	[XB_KEY_NUMPAD_DECIMAL] = {0xA, 0x02},
	[XB_KEY_NUMPAD_COMMA] = {0xA, 0x01},

	[XB_KEY_HIRA] = {0xB, 0x80},
	[XB_KEY_INS] = {0xB, 0x40},
	[XB_KEY_CAPS] = {0xB, 0x20},
	[XB_KEY_CODE] = {0xB, 0x10},
	[XB_KEY_ROMA] = {0xB, 0x08},
	[XB_KEY_KANA] = {0xB, 0x04},
	[XB_KEY_XF5] = {0xB, 0x02},
	[XB_KEY_XF4] = {0xB, 0x01},

	[XB_KEY_F5] = {0xC, 0x80},
	[XB_KEY_F4] = {0xC, 0x40},
	[XB_KEY_F3] = {0xC, 0x20},
	[XB_KEY_F2] = {0xC, 0x10},
	[XB_KEY_F1] = {0xC, 0x08},
	[XB_KEY_COPY] = {0xC, 0x04},
	[XB_KEY_BREAK] = {0xC, 0x02},
	[XB_KEY_ZENKAKU] = {0xC, 0x01},

	[XB_KEY_F10] = {0xD, 0x10},
	[XB_KEY_F9] = {0xD, 0x08},
	[XB_KEY_F8] = {0xD, 0x04},
	[XB_KEY_F7] = {0xD, 0x02},
	[XB_KEY_F6] {0xD, 0x01},

	[XB_KEY_OPT2] = {0xE, 0x08},
	[XB_KEY_OPT1] = {0xE, 0x04},
	[XB_KEY_CTRL] = {0xE, 0x02},
	[XB_KEY_SHIFT] = {0xE, 0x01},
};

//
// Key state helpers
//

static inline bool key_is_held(XBKey key)
{
	const XBKeyID *id = &kkey_table[key];
	return s_keys.key_bits[id->group] & id->mask;
}

static inline bool key_posedge(XBKey key)
{
	const XBKeyID *id = &kkey_table[key];
	const bool now = s_keys.key_bits[id->group] & id->mask;
	const bool prev = s_keys.key_bits_prev[id->group] & id->mask;
	return now && !prev;
}

static inline bool key_negedge(XBKey key)
{
	const XBKeyID *id = &kkey_table[key];

	const bool now = s_keys.key_bits[id->group] & id->mask;
	const bool prev = s_keys.key_bits_prev[id->group] & id->mask;
	return prev && !now;
}

//
// Initialization and Configuration
//

void xb_keys_init(const XBKey *sense_list)
{
	memset(&s_keys, 0, sizeof(s_keys));
	xb_keys_set_repeat(-1, -1);
	s_keys.repeat_key = XB_KEY_INVALID;

	if (sense_list == NULL)
	{
		for (XBKey i = 0; i < XB_KEY_INVALID; i++)
		{
			s_keys.sense_list[i] = i;
		}
		s_keys.sense_list_size = XB_ARRAYSIZE(s_keys.sense_list);
	}
	else
	{
		uint16_t i = 0;
		while (sense_list[i] != XB_KEY_INVALID &&
		       i < XB_ARRAYSIZE(s_keys.sense_list))
		{
			s_keys.sense_list[i] = sense_list[i];
			i++;
		}
		s_keys.sense_list_size = i;
	}
}

void xb_keys_set_repeat(int16_t delay, int16_t rate)
{
	if (delay <= 0) delay = XB_KEYS_DEFAULT_REPEAT_DELAY;
	if (rate <= 0) rate = XB_KEYS_DEFAULT_REPEAT_RATE;
	s_keys.repeat_delay = delay;
	s_keys.repeat_period = rate;
}

static inline void event_push(XBKey key, bool repeat, bool key_up)
{
	const uint16_t next_w = (s_keys.key_w + 1) % XB_ARRAYSIZE(s_keys.key_events);
	if (next_w == s_keys.key_r) return;  // Queue full.
	// Record new event.
	XBKeyEvent *ev = &s_keys.key_events[s_keys.key_w];
	ev->name = key;
	// Set modifier for modifiers and other data
	ev->modifiers = 0;
	if (key_is_held(XB_KEY_SHIFT)) ev->modifiers |= XB_KEY_MOD_SHIFT;
	if (key_is_held(XB_KEY_CTRL)) ev->modifiers |= XB_KEY_MOD_CTRL;
	if (key_up) ev->modifiers |= XB_KEY_MOD_KEY_UP;
	else if (repeat) ev->modifiers |= XB_KEY_MOD_IS_REPEAT;
	// Move the write index forth
	s_keys.key_w = next_w;
}

//
// Main Interface
//

void xb_keys_poll(void)
{
	// Update key matrix bitfields
	memcpy(s_keys.key_bits_prev, s_keys.key_bits, sizeof(s_keys.key_bits));
	for (uint16_t i = 0; i < XB_ARRAYSIZE(s_keys.key_bits); i++)
	{
		s_keys.key_bits[i] = _iocs_bitsns(i);
	}

	// Generate events
	for (uint16_t i = 0; i < s_keys.sense_list_size; i++)
	{
		const XBKey key = s_keys.sense_list[i];
		const bool posedge = key_posedge(key);
		const bool negedge = key_negedge(key);
		if (!posedge && !negedge) continue;
		event_push(key,
		           /*repeat=*/false,
		           /*key_up=*/negedge);
		// Freshly pressed key resets the key repeat logic
		if (posedge && s_keys.repeat_key != key)
		{
			s_keys.repeat_key = key;
			s_keys.repeat_cnt = -s_keys.repeat_delay;
		}
	}


	// Key repeat logic
	if (s_keys.repeat_key != XB_KEY_INVALID)
	{
		if (key_is_held(s_keys.repeat_key))
		{
			s_keys.repeat_cnt++;
			if (s_keys.repeat_cnt >= s_keys.repeat_period)
			{
				s_keys.repeat_cnt = 0;
				event_push(s_keys.repeat_key,
				           /*repeat=*/true,
				           /*key_up=*/false);
			}
		}
		else
		{
			s_keys.repeat_key = XB_KEY_INVALID;
		}
	}
}

bool xb_keys_event_pop(XBKeyEvent *out)
{
	if (s_keys.key_r == s_keys.key_w) return false;
	const uint16_t next_r = (s_keys.key_r + 1) % XB_ARRAYSIZE(s_keys.key_events);

	*out = s_keys.key_events[s_keys.key_r];

	s_keys.key_r = next_r;
	return true;
}

// Returns true if the key is currently held down.
bool xb_key_on(XBKey key)
{
	return key_is_held(key);
}

// Returns true if the key was just pressed in the last polling period.
bool xb_key_down(XBKey key)
{
	return key_posedge(key);
}

// Retruns true if the key was just released in the last polling period.
bool xb_key_up(XBKey key)
{
	return key_negedge(key);
}
