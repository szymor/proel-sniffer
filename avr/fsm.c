#include "fsm.h"

#include <stddef.h>
#include <stdio.h>

static enum State state = STATE_IDLE;
static fsm_cb flat_cb = NULL;
static uint8_t flatno = 0;

void fsm_reset(void)
{
	state = STATE_IDLE;
}

enum State fsm_get_state(void)
{
	return state;
}

void fsm_set_cb(fsm_cb callback)
{
	flat_cb = callback;
}

void fsm_push_event(uint8_t rising, uint32_t period)
{
	if (state == STATE_IDLE)
	{
		if (!rising)
		{
			state = STATE_RESET_START;
		}
	}
	else if (state == STATE_RESET_START)
	{
		if (rising && period > 200000 && period < 500000)
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
		if (!rising && period > 180000 && period < 250000)
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
		if (rising && period > 5 && period < 30)
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
		if (!rising && period > 150 && period < 300)
		{
			state = STATE_FLAT_START;
		}
		else if (!rising && period > 30000 && period < 1000000)
		{
			if (flat_cb != NULL)
			{
				flat_cb(flatno);
			}
			state = STATE_RINGTONE_START;
		}
		else
		{
			state = STATE_IDLE;
		}
	}
	else if (state == STATE_RINGTONE_START)
	{
		// start of ringtone OR reset-like end
		if (rising && period > 200000 && period < 500000)
		{
			state = STATE_IDLE;
		}
		else
		{
			state = STATE_RINGTONE;
		}
	}
	else if (state == STATE_RINGTONE)
	{
		if (!rising)
		{
			if (period > 8000000)
			{
				//printf("Ring!\n");
				// even a few seconds may pass between ringtone and end signal
				state = STATE_RESET_START;
			}
			else if (period > 50000)
			{
				//printf("Ring!\n");
				state = STATE_RINGTONE_START;
			}
		}
		else
		{
			if (period > 50000)
			{
				state = STATE_IDLE;
			}
		}
	}
}
