

.PHONY: all
all: prog

prog:
	avr-gcc -mmcu=atmega328p  main.c -o prog.elf
	avr-objcopy -j .text -O ihex prog.elf prog.hex


install: prog
	avrdude -v -p atmega328p -c arduino -P /dev/ttyACM0 -D -U "flash:w:prog.hex:i"


clean:
	rm *.elf
