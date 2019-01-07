file?=main.c


all: prog.hex
	avrdude -V -B 9600 -p atmega128 -c jtagmkI -P /dev/ttyUSB0 -U "flash:w:prog.hex:i"

prog.hex: prog.elf
	avr-objcopy -j .text -O ihex prog.elf prog.hex

prog.elf: ${file}
	avr-gcc -Wall -DF_CPU=13000000 -mmcu=atmega128 -O2 ${file} -o prog.elf

clean:
	rm *.elf *.hex
