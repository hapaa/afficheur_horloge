#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>


#define BAUD 38400
#define MYUBRR F_CPU/16/BAUD-1


void USART_Init (unsigned int ubrr)
{
/* Set baud rate */
UBRR1H = (unsigned char)(ubrr>>8);
UBRR1L = (unsigned char)ubrr;
/* Enable receiver and transmitter */
UCSR1B = (1<<RXEN)|(1<<TXEN);
/* Set frame format: 8data, 2stop bit */
UCSR1C = (1<<USBS)|(3<<UCSZ1);
}


void USART_Transmit( unsigned char data ) {
/* Wait for empty transmit buffer */
while ( !( UCSR1A & (1<<UDRE1)) );
/* Put data into buffer, sends the data */
UDR1 = data;
}


unsigned char USART_Receive( void ) {
/* Wait for data to be received */
while ( !(UCSR1A & (1<<RXC1)) );
/* Get and return received data from buffer */
return UDR1;
}


int main()
{

USART_Init(MYUBRR);
while (1){
USART_Transmit(USART_Receive()+1);
  }
}
