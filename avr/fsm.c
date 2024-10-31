#include "fsm.h"

#include <stddef.h>
#include <stdio.h>

enum State
{
	STATE_IDLE,
	STATE_RESET_START,
	STATE_RESET_END,
	STATE_FLAT_START,
	STATE_FLAT_END,
	STATE_RINGTONE,
	STATE_END
};

static enum State state = STATE_IDLE;
static fsm_cb flat_cb = NULL;
static uint8_t flatno = 0;

void fsm_reset(void)
{
	state = STATE_IDLE;
	flat_cb = NULL;
}

void fsm_set_cb(fsm_cb callback)
{
	flat_cb = callback;
}

void fsm_push_event(uint8_t raising, uint32_t period)
{
	// reset ring tone state if waiting too long
	if (state == STATE_RINGTONE && period > 1000000)
	{
		state = STATE_IDLE;
	}
	// -----------------------------------------

	if (state == STATE_IDLE)
	{
		if (!raising)
		{
			state = STATE_RESET_START;
		}
	}
	else if (state == STATE_RESET_START)
	{
		if (raising && period > 200000 && period < 500000)
		{
			flatno = 0;
			state = STATE_RESET_END;
		}
		else
		{
			state = STATE_IDLE;
		}
	}
	else if (state == STATE_RESET_END)
	{
		if (!raising && period > 180000 && period < 250000)
		{
			state = STATE_FLAT_START;
		}
		else
		{
			state = STATE_IDLE;
		}
	}
	else if (state == STATE_FLAT_START)
	{
		if (raising && period > 5 && period < 30)
		{
			++flatno;
			state = STATE_FLAT_END;
		}
		else
		{
			state = STATE_IDLE;
		}
	}
	else if (state == STATE_FLAT_END)
	{
		if (!raising && period > 150 && period < 300)
		{
			state = STATE_FLAT_START;
		}
		else if (!raising && period > 30000 && period < 1000000)
		{
			if (flat_cb != NULL)
			{
				flat_cb(flatno);
			}
			state = STATE_RINGTONE;
		}
		else
		{
			state = STATE_IDLE;
		}
	}
	else if (state == STATE_RINGTONE)
	{
		// do nothing
	}
}
