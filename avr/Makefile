.PHONY: all clean test

TARGET=main
TEST_TARGET=unittest

all: test

clean:
	rm -rf $(TEST_TARGET)

test: $(TEST_TARGET)

$(TEST_TARGET): unittest.c fsm.c
	gcc -o $@ $^
