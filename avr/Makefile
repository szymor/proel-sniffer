.PHONY: all clean test flash

TARGET=atmega328pb
PROJECT=main
PROJECT_TEST=unittest

all: $(PROJECT) $(PROJECT).hex test

clean:
	-rm -rf $(PROJECT) $(PROJECT).hex $(PROJECT_TEST)

test: $(PROJECT_TEST)

flash:
	avrdude -c usbasp -p m16 -U flash:w:main.hex -U lfuse:w:0xC1:m -U hfuse:w:0x99:m -U efuse:w:0xFC:m

$(PROJECT_TEST): unittest.c fsm.c
	gcc -g -o $@ $^

$(PROJECT): main.c fsm.c serial.c
	avr-gcc -o $@ -mmcu=$(TARGET) -O2 -DDEBUG -DF_CPU=18432000UL $^

%.hex: %
	avr-objcopy -F elf32-avr $< $@
