#include <avr/io.h>
#include <util/delay.h>

#define BAUD 38400
#define MYUBRR F_CPU/8/BAUD-1

void USART_Init( unsigned int ubrr )
{
/* Set baud rate */
UBRRH = (unsigned char)(ubrr>>8);
UBRRL = (unsigned char)ubrr;
/* Enable receiver and transmitter */
UCSRB = (1<<RXEN)|(1<<TXEN);
/* Set frame format: 8data, 2stop bit */
UCSRC = (1<<USBS)|(3<<UCSZ0);
}

void USART_Transmit( unsigned char data )
{
/* Wait for empty transmit buffer */
while ( !( UCSRA & (1<<UDRE)) )
;
/* Put data into buffer, sends the data */
UDR = data;
}

unsigned char USART_Receive( void )
{
/* Wait for data to be received */
while ( !(UCSRA & (1<<RXC)) )
;
/* Get and return received data from buffer */
return UDR;
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