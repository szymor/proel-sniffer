#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdint.h>
#include <stdio.h>

#include "fsm.h"

union Num
{
	uint32_t v32;
	struct
	{
		uint16_t lsp;
		uint16_t msp;
	};
};

void serial_init(void);

#define CYCLE_SIZE		(256)
#define CYCLE_MASK		(0xff)

#define GREEN_PORT		(PORTB)
#define GREEN_PIN		(0)
#define RED_PORT		(PORTB)
#define RED_PIN			(1)
#define PDCTRL_PORT		(PORTB)
#define PDCTRL_PIN		(1)

// start, mod 8 prescaler (2.304 MHz)
#define timer_start() do { TCCR1B = _BV(CS11); } while (0);
#define timer_stop() do { TCCR1B = 0; } while (0);
#define cycle_forward(x) do { x = (x + 1) & CYCLE_MASK; } while (0);

static volatile uint16_t count = 0;

static volatile union Num ts[CYCLE_SIZE];
static volatile uint8_t cbegin = 0;
static volatile uint8_t cend = 0;

ISR(TIMER1_OVF_vect)
{
	++count;
}

ISR(INT0_vect)
{
	ts[cend].lsp = TCNT1;
	TCNT1 = 0;
	ts[cend].msp = count;
	count = 0;
	ts[cend].lsp &= 0xfffe;
	cycle_forward(cend);
}

ISR(INT1_vect)
{
	ts[cend].lsp = TCNT1;
	TCNT1 = 0;
	ts[cend].msp = count;
	count = 0;
	ts[cend].lsp |= 0x0001;
	cycle_forward(cend);
}

void flat_callback(uint8_t flatno)
{
	PORTB = ~flatno;
	printf("m:publish(topic..\"flat\", %d, 2, 0)\r\n", flatno);
}

void main(void)
{
	serial_init();

	// enable interrupts
	sei();

	// enable timer 1 overflow interrupt
	TIMSK1 = _BV(TOIE1);
	// normal timer 1 operation
	TCCR1A = 0x00;
	timer_start();

	// default INT0 and INT1 config as tri-z input is NOT ok
	PORTD = _BV(PIND2) | _BV(PIND3);
	// configure INT0 as falling edge, INT1 as rising edge
	MCUCR = _BV(ISC01) | _BV(ISC11) | _BV(ISC10);
	// enable external interrupt
	EIMSK = _BV(INT0) | _BV(INT1);

	fsm_set_cb(flat_callback);
	fsm_reset();

	// pd control, green and red leds are output
	PORTB = 0x07;
	DDRB = 0x07;

	while (1)
	{
		// heartbeat
		if (count & 0x0010)
		{
			GREEN_PORT |= _BV(GREEN_PIN);
		}
		else
		{
			GREEN_PORT &= ~_BV(GREEN_PIN);
		}
		// reset the fsm after several seconds (just in case)
		if (count & 0x0800)
		{
#ifdef DEBUG
			const char state2string[][8] = {
				"idle",
				"rstart",
				"rend",
				"fstart",
				"fend",
				"rtstart",
				"rtend"
			};
			// publish debug info
			printf("m:publish(topic..\"debug/last_state\", \"%s\", 2, 0)\r\n", state2string[fsm_get_debug_property(DP_STATE)]);
			printf("m:publish(topic..\"debug/reset\", \"%s\", 2, 0)\r\n", state2string[fsm_get_debug_property(DP_RESET_PERIOD)]);
			printf("m:publish(topic..\"debug/end\", \"%s\", 2, 0)\r\n", state2string[fsm_get_debug_property(DP_END_PERIOD)]);
			printf("m:publish(topic..\"debug/flat_low\", \"%s\", 2, 0)\r\n", state2string[fsm_get_debug_property(DP_FLAT_LOW_PERIOD)]);
			printf("m:publish(topic..\"debug/flat_high\", \"%s\", 2, 0)\r\n", state2string[fsm_get_debug_property(DP_FLAT_HIGH_PERIOD)]);
#endif
			count = 0;
			fsm_reset();
			// publish (mocked) idle voltage
			printf("m:publish(topic..\"idle_voltage\", \"%d.0\", 2, 0)\r\n", 8);
		}
		// process the cyclic buffer of events
		if (cbegin != cend)
		{
			fsm_push_event(ts[cbegin].lsp & 1, (ts[cbegin].v32 & 0xfffffffe) * 868 / 1000);
			cycle_forward(cbegin);
		}
	}
}
