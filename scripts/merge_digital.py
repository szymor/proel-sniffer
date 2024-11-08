#!/bin/env python3

import sys

if len(sys.argv) < 3:
	print(f"Usage: {sys.argv[0]} <timebreak> <digital1.csv> [<digital2.csv ...]")
	exit(1)

timebreak = float(sys.argv[1])
t0 = 0
last = t0

for path in sys.argv[2:]:
	with open(path) as f:
		for line in f:
			ts, val = line.split(",")
			ts = float(ts)
			val = int(val)
			ts = ts + t0
			print(f"{ts}, {val}")
			last = ts
		t0 = last + timebreak
