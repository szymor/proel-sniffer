.PHONY: all clean test flash

TARGET=atmega16
PROJECT=main
PROJECT_TEST=unittest

all: $(PROJECT) $(PROJECT).hex test

clean:
	-rm -rf $(PROJECT) $(PROJECT).hex $(PROJECT_TEST)

test: $(PROJECT_TEST)

flash:
	avrdude -c usbasp -p m16 -U flash:w:main.hex -U lfuse:w:0x1E:m -U hfuse:w:0xD9:m

$(PROJECT_TEST): unittest.c fsm.c
	gcc -o $@ $^

$(PROJECT): main.c fsm.c
	avr-gcc -o $@ -mmcu=$(TARGET) -O2 -DF_CPU=16000000UL $^

%.hex: %
	avr-objcopy -F elf32-avr $< $@
