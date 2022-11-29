#include "xbase/joy.h"
#include "xbase/memmap.h"
#include <string.h>

XBJoyBits g_xb_joy[XB_JOY_COUNT];       // Key data from the last poll.
XBJoyBits g_xb_joy_pos[XB_JOY_COUNT];   // Keys just pressed.
XBJoyBits g_xb_joy_neg[XB_JOY_COUNT];   // Keys just released.
XBJoyBits g_xb_joy_prev[XB_JOY_COUNT];  // The previous poll's keys.

static XBJoyMode s_mode[XB_JOY_COUNT] = {XB_JOY_MODE_EXTENDED, XB_JOY_MODE_EXTENDED};

void xb_joy_init(uint8_t player, XBJoyMode mode)
{
	memset(g_xb_joy, 0, sizeof(g_xb_joy));
	memset(g_xb_joy_pos, 0, sizeof(g_xb_joy_pos));
	memset(g_xb_joy_neg, 0, sizeof(g_xb_joy_neg));
	memset(g_xb_joy_prev, 0, sizeof(g_xb_joy_prev));
}

void xb_joy_poll(void)
{
	for (uint16_t i = 0; i < XB_JOY_COUNT; i++)
	{
		g_xb_joy_prev[i] = g_xb_joy[i];

		uint8_t pad = ((volatile uint8_t *)(XB_JOY_BASE))[i * 2];

		if (s_mode[i] == XB_JOY_MODE_EXTENDED)
		{
			if ((pad & XB_KEY_UP) && (pad & XB_KEY_DOWN))
			{
				pad &= ~(XB_KEY_UP | XB_KEY_DOWN);
				pad |= XB_KEY_SELECT;
			}
			if ((pad & XB_KEY_LEFT) && (pad & XB_KEY_RIGHT))
			{
				pad &= ~(XB_KEY_LEFT | XB_KEY_RIGHT);
				pad |= XB_KEY_START;
			}
		}

		// TODO: Support for CPSF-MD, etc.

		g_xb_joy[i] = pad;
		g_xb_joy_pos[i] = g_xb_joy[i] & ~g_xb_joy_prev[i];
		g_xb_joy_neg[i] = g_xb_joy_prev[i] & ~g_xb_joy[i];
	}
}
