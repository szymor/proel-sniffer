#!/bin/env python3

import sys

if len(sys.argv) < 2:
	print(f"Usage: {sys.argv[0]} <analog.csv>")
	exit(1)

f = open(sys.argv[1])

ch, t0, tinc, eol = f.readline().split(",")
t0 = t0.split()[2][:-1]
t0 = float(t0)
tinc = tinc.split()[2][:-1]
tinc = float(tinc)

state = 1
for line in f:
	val = float(line.split(",")[0])
	if state == 1 and val < 1.5:
		state = 0
		print(f"{t0}, {state}")
	elif state == 0 and val > 3.5:
		state = 1
		print(f"{t0}, {state}")
	t0 = t0 + tinc

f.close()
