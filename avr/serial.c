#include <avr/io.h>
#include <stdio.h>

static int sendc(char c, FILE *f);
static FILE defstream = FDEV_SETUP_STREAM(sendc, NULL, _FDEV_SETUP_RW);

void serial_init(void)
{
#define BAUD_TOL 3
#define BAUD 115200
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
	while ( !( UCSRA & _BV(UDRE) ) );
	UDR = c;
	return 0;
}
