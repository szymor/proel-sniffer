// unit tests for the Proel Protocol Finite State Machine

#include <stdio.h>

#include "fsm.h"

static uint8_t flat_number = 0;

void store_flatno(uint8_t flatno)
{
	flat_number = flatno;
}

int main(int argc, char *argv[])
{
	printf("PPFSM unit tests\n");

	// basic waveform
	fsm_reset();
	fsm_set_cb(store_flatno);
	flat_number = 0;
	FILE *csv = fopen("../lua/testdata/basic.csv", "r");
	double last_ts = 0;
	while (!feof(csv))
	{
		double timestamp;
		int value;
		if (2 == fscanf(csv, "%lf,%d", &timestamp, &value))
		{
			uint32_t period = (timestamp - last_ts) * 1000000;
			last_ts = timestamp;

			fsm_push_event(value, period);
		}
	}
	int expected = 44;
	if (flat_number == expected)
	{
		printf("[PASSED] Flat %d\n", flat_number);
	}
	else
	{
		printf("[FAILED] Flat %d, expected %d\n", flat_number, expected);
	}
	fclose(csv);

	// double ringbell
	fsm_reset();
	fsm_set_cb(store_flatno);
	flat_number = 0;
	csv = fopen("../lua/testdata/double_ringbell.csv", "r");
	while (!feof(csv))
	{
		double timestamp;
		int value;
		if (2 == fscanf(csv, "%lf,%d", &timestamp, &value))
		{
			uint32_t period = (timestamp - last_ts) * 1000000;
			last_ts = timestamp;

			fsm_push_event(value, period);
		}
	}
	expected = 44;
	if (flat_number == expected)
	{
		printf("[PASSED] Flat %d\n", flat_number);
	}
	else
	{
		printf("[FAILED] Flat %d, expected %d\n", flat_number, expected);
	}
	fclose(csv);

	return 0;
}
