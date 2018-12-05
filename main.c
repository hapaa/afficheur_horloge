#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>


#define BAUD 38400
#define MYUBRR F_CPU/16/BAUD-1

//DDRE |= (1<<DDE2); METTRE PE2 EN OUTPUT, SUPPOSEMENT EQUIVALENT A METTRE XCKO SUR PE2
//PORTE &= ~(1<<PORTE2); FORCER INPUT XCK0 A 0, SUPPOSEMENT EMPECHER MODE CONFIG

void USART_Init (unsigned int ubrr)
{
/* Set baud rate */
UBRR0H = (unsigned char)(ubrr>>8);
UBRR0L = (unsigned char)ubrr;
/* Enable receiver and transmitter */
UCSR0B = (1<<RXEN0)|(1<<TXEN0);
/* Set frame format: 8data, 2stop bit */
UCSR0C = (1<<USBS0)|(1<<UCSZ00)|(1<<UCSZ01);
}


void USART_Transmit( unsigned char data ) {
/* Wait for empty transmit buffer */
while ( !( UCSR0A & (1<<UDRE0)) );
/* Put data into buffer, sends the data */
UDR0 = data;
}


unsigned char USART_Receive( void ) {
/* Wait for data to be received */
while ( !(UCSR0A & (1<<RXC0)) );
/* Get and return received data from buffer */
return UDR0;
}


int main()
{
USART_Init(MYUBRR);
while (1){
USART_Transmit(USART_Receive()+1);
  }
}
