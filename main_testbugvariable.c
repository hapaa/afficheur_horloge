#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>

#define BAUD 38400
#define MYUBRR F_CPU/16/BAUD-1

uint16_t compteur_secondes;
uint16_t compteur_usart;

const static uint16_t chiffres[30] = {992, 544, 992,
                          0, 0, 992,
                            736, 672, 928,
                             992, 672, 672,
                              992, 128, 224,
                               928, 672, 736,
                                896, 640, 992,
                                 992, 32, 32,
                                  992, 672, 992,
                                   992, 160, 224};

//DDRE |= (1<<DDE2); METTRE PE2 EN OUTPUT, SUPPOSEMENT EQUIVALENT A METTRE XCKO SUR PE2
//PORTE &= ~(1<<PORTE2); FORCER INPUT XCK0 A 0, SUPPOSEMENT EMPECHER MODE CONFIG

void USART_Init (unsigned int ubrr) {
/* Set baud rate */
UBRR0H = (unsigned char)(ubrr>>8);
UBRR0L = (unsigned char)ubrr;
/* Enable receiver and transmitter */
UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
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
//while ( !(UCSR0A & (1<<RXC0)) );
/* Get and return received data from buffer */
return UDR0;
}

void uart_send(char *str) {

    while (*str != '\0') {
      if (UCSR0A & _BV(UDRE0)){
        UDR0 = *(str++);
      }

    }
}

void SPI_MasterInit(void) {
    PORTB|=(1<<DDB0);

    DDRE |= (1<<DDE4);
    PORTE |= (1<<PORTE4);

    DDRE |= (1<<DDE5);
    PORTE &= ~(1<<PORTE5);

    /* Set MOSI and SCK output, all others input */
    DDRB = (1<<DDB2)|(1<<DDB1);
    /* Enable SPI, Master, set clock rate fck/16 */
    SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}


void SPI_MasterTransmit(char cData) {
    /* Start transmission */
    SPDR = cData;
    /* Wait for transmission complete */
    while(!(SPSR & (1<<SPIF)));
}



ISR(USART0_RX_vect) {
  // Action
    compteur_usart++;
    UDR0;
}

void set_interrupt(void) {

    //etre sûr que TWEN de TWCR est à 0

    DDRD &= ~(1<<DDD0); //Met PORTD0 en Input

    PORTD |= (1<<PORTD0); //Mettre le pull-up resistor

    EICRA |= (1<<ISC01); //Active le mode détection de fronts montants du INT0

    EIMSK |= (1<<INT0); //Met le bit correspondant à l'interruption 0 sur 1 dans le EIMSK

    sei();
    //SREG |= (1<<I); //Met le bit à 1 du global interrupt du status register, permet d'activer les interruptions

}

// Fonction permettant de mettre à jour un chiffre
void update_chiffre(uint16_t position_debut, uint16_t pos, uint16_t matrice[])
{
  matrice[position_debut] = chiffres[0+pos];
  matrice[position_debut+1] = chiffres[1+pos];
  matrice[position_debut+2] = chiffres[2+pos];
}

int main(void)
{
compteur_secondes = 0;
SPI_MasterInit();
set_interrupt();
USART_Init(MYUBRR);

uint16_t secondes_unite = 0;
while(1){

      char string[64];
      itoa(chiffres[secondes_unite], string, 10);  //convert integer to string, radix=1
      uart_send(string);

      if (secondes_unite<=10)
      {
        secondes_unite++;
      }
      else
      {
        secondes_unite=0;
      }


}

}
