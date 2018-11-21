#include <avr/io.h>
#include <util/delay.h>

#define BAUD 38400
#define MYUBRR F_CPU/8/BAUD-1

void USART_Init( unsigned int ubrr)
{
  /*Set baud rate */
  UBRR0H = (unsigned char)(ubrr>>8);
  UBRR0L = (unsigned char)ubrr;
  /*Enable receiver and transmitter */
  UCSR0B = (1<<RXEN0)|(1<<TXEN0);
  /* Set frame format: 8data, 2stop bit */
  UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

void USART_Transmit( unsigned char data )
{
/* Wait for empty transmit buffer */
while ( !( UCSR0A & (1<<UDRE0)) );
/* Put data into buffer, sends the data */
UDR0 = data;
}

unsigned char USART_Receive( void )
{
/* Wait for data to be received */
while ( !(UCSR0A & (1<<RXC0)) );
/* Get and return received data from buffer */
return UDR0;
}

int main(){

    USART_Init(MYUBRR);
    // Active et allume la broche PB5 (led)
    /*DDRB |= _BV(PB5);
    int i;
    for (i=0;i<5;i++){
      //Met le PB5 à 1
      PORTB |=_BV(PB5);
      //Attendre 1 seconde. Attention indiquer la fréquence de notre microcontroleur
      _delay_ms(1000);
      //Met le PB5 à 0
      PORTB &=~_BV(PB5);
      //Attendre 1 seconde. Attention indiquer la fréquence de notre microcontroleur
      _delay_ms(1000);
    }*/

    while(1)
    {
      USART_Transmit(USART_Receive()+1);
    }
}
