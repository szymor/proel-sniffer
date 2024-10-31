#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <stdint.h>
#include <stdio.h>

#include "fsm.h"

#define CYCLE_SIZE	(16)
#define CYCLE_MASK	(0x0f)

// start, mod 8 prescaler (2 MHz)
#define timer_start() do { TCCR1B = _BV(CS11); } while (0);
#define timer_stop() do { TCCR1B = 0; } while (0);
// get time in us
#define timer_get() ((((uint32_t)count) << 15) | (((uint32_t)TCNT1) >> 1))
#define timer_clear() do { TCNT1 = 0; count = 0; } while (0);

#define cycle_forward(x) do { x = (x + 1) & CYCLE_MASK; } while (0);

static volatile uint16_t count = 0;

static volatile uint32_t ts[CYCLE_SIZE];
static volatile uint8_t val[CYCLE_SIZE];
static volatile uint8_t cbegin = 0;
static volatile uint8_t cend = 0;

static int sendc(char c, FILE *f);
static FILE defstream = FDEV_SETUP_STREAM(sendc, NULL, _FDEV_SETUP_RW);

void serial_init(void)
{
#define BAUD 19200
#include <util/setbaud.h>
	UBRRH = UBRRH_VALUE;
	UBRRL = UBRRL_VALUE;
	UCSRB = (1 << TXEN) | (1 << RXEN);
#if USE_2X
	UCSRA |= (1 << U2X);
#else
	UCSRA &= ~(1 << U2X);
#endif
#undef BAUD
	stdout = stdin = &defstream;
}

static int sendc(char c, FILE *f)
{
	// wait for empty transmit buffer
	while ( !( UCSRA & (1 << UDRE) ) );
	UDR = c;
	return 0;
}

ISR(TIMER1_OVF_vect)
{
	++count;
}

// higher priority than timer1
ISR(INT0_vect)
{
	val[cend] = PIND & PIND2;
	ts[cend] = timer_get();
	cycle_forward(cend);
}

void flat_callback(uint8_t flatno)
{
	PORTB = ~flatno;
}

void main(void)
{
	serial_init();

	// enable interrupts
	sei();

	// enable timer 1 overflow interrupt
	TIMSK = _BV(TOIE1);
	// normal timer 1 operation
	TCCR1A = 0x00;
	timer_start();

	// default INT0 config as tri-z input is ok
	// configure external interrupt as dual edge
	MCUCR = _BV(ISC00);
	// enable external interrupt
	GICR = _BV(INT0);

	fsm_reset();
	fsm_set_cb(flat_callback);

	uint32_t last_ts = 0;

	DDRB = 0xff;

	printf("Start!\r\n");
	while (1)
	{
		if (cbegin != cend)
		{
			fsm_push_event(val[cbegin], ts[cbegin] - last_ts);
			last_ts = ts[cbegin];
			cycle_forward(cbegin);
		}
	}
}
