// unit tests for the Proel Protocol Finite State Machine

#include <stdio.h>

#include "fsm.h"

static int expected[16];
static int exnum;
static int cnt;

void check_cb(uint8_t flatno)
{
	if (flatno == expected[cnt])
	{
		printf("  [PASSED %d/%d] Flat %d\n", cnt + 1, exnum, flatno);
	}
	else
	{
		printf("  [FAILED %d/%d] Flat %d, expected %d\n", cnt + 1, exnum, flatno, expected[cnt]);
	}

	++cnt;
}

void run_test(char *title, char *csv_path)
{
	puts(title);
	fsm_reset();
	fsm_set_cb(check_cb);
	FILE *csv = fopen(csv_path, "r");
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
	fclose(csv);
}

int main(int argc, char *argv[])
{
	printf("== PPFSM unit tests ==\n\n");

	cnt = 0;
	exnum = 1;
	expected[0] = 44;
	run_test("basic waveform", "../lua/testdata/basic.csv");

	cnt = 0;
	exnum = 1;
	expected[0] = 44;
	run_test("double ringbell", "../lua/testdata/double_ringbell.csv");

	cnt = 0;
	exnum = 1;
	expected[0] = 44;
	run_test("basic with noise", "../lua/testdata/basic_with_noise.csv");

	cnt = 0;
	exnum = 3;
	expected[0] = 44;
	expected[1] = 38;
	expected[2] = 44;
	run_test("three in a row", "../lua/testdata/three-in-a-row.csv");

	cnt = 0;
	exnum = 1;
	expected[0] = 44;
	run_test("ds0", "../lua/testdata/ds0.csv");

	cnt = 0;
	exnum = 1;
	expected[0] = 44;
	run_test("ds1", "../lua/testdata/ds1.csv");

	cnt = 0;
	exnum = 1;
	expected[0] = 44;
	run_test("ds2", "../lua/testdata/ds2.csv");

	cnt = 0;
	exnum = 1;
	expected[0] = 44;
	run_test("ds3", "../lua/testdata/ds3.csv");

	cnt = 0;
	exnum = 1;
	expected[0] = 44;
	run_test("ds4", "../lua/testdata/ds4.csv");

	cnt = 0;
	exnum = 1;
	expected[0] = 44;
	run_test("ds5", "../lua/testdata/ds5.csv");

	cnt = 0;
	exnum = 1;
	expected[0] = 38;
	run_test("ds6", "../lua/testdata/ds6.csv");

	return 0;
}
