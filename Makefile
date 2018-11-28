
.PHONY: all install
all: install


install: prog.hex
	avrdude -v -p atmega128 -c jtagmkI -P /dev/ttyUSB0 -D -U "flash:w:prog.hex:i"


prog.hex: main.c
	avr-gcc -mmcu=atmega128 -O2 main.c -o prog.elf
	avr-objcopy -j .text -O ihex prog.elf prog.hex

clean:
	rm *.elf *.hex
