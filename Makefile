

.PHONY: all
all: prog

prog:
	avr-gcc -mmcu=atmega128  SPI_example.c -o prog.elf
	avr-objcopy -j .text -O ihex prog.elf prog.hex


install: prog
	avrdude -v -p atmega128 -c jtagmkI -P /dev/ttyACM0 -D -U "flash:w:prog.hex:i"


clean:
	rm *.elf
