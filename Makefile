

.PHONY: all
all: prog

prog:
	avr-gcc -mmcu=m128  main.c -o prog.elf
	avr-objcopy -j .text -O ihex prog.elf prog.hex


install: prog
	avrdude -v -p m128 -c jtagmkI -P /dev/ttyACM0 -D -U "flash:w:prog.hex:i"


clean:
	rm *.elf
