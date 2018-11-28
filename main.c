#include <avr/io.h>


int main() {
  // Write value to Pin A
  DDRA = OxFF; // turn on output
  PORTA = 0x00;
  PINA = 0x02;
  
  
  // Read value from Pin A
  DDRA = 0x00; // turn on input
  PORTA = 0xFF; // enable pull-ups
  unsigned char ret = PINA;

  // Print received value through JTAG
  
  
  // USe UART for print
  return 0;
}
