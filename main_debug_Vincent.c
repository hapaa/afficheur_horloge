#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>

#define BAUD 38400
#define MYUBRR F_CPU/16/BAUD-1
uint16_t detection_hall;
uint16_t compteur_usart;

static const uint16_t chiffres[30] = {992, 544, 992,
                                      0, 0, 992,
                                      736, 672, 928,
                                      992, 672, 672,
                                      992, 128, 224,
                                      928, 672, 736,
                                      896, 640, 992,
                                      992, 32, 32,
                                      992, 672, 992,
                                      992, 160, 224};


void USART_Init (unsigned int ubrr){
/* Set baud rate */
UBRR0H = (unsigned char)(ubrr>>8);
UBRR0L = (unsigned char)ubrr;
/* Enable receiver and transmitter */
UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
/* Set frame format: 8data, 2stop bit */
UCSR0C = (1<<USBS0)|(1<<UCSZ00)|(1<<UCSZ01);
}

/*void USART_Transmit( unsigned char data ) {
// Wait for empty transmit buffer
while ( !( UCSR0A & (1<<UDRE0)) );
//Put data into buffer, sends the data
UDR0 = data;
}*/

/*unsigned char USART_Receive( void ) {
//Wait for data to be received
//while ( !(UCSR0A & (1<<RXC0)) );
// Get and return received data from buffer
return UDR0;
}*/

void uart_send(char *str) {

    while (*str != '\0') {
      if (UCSR0A & _BV(UDRE0)){
        UDR0 = *(str++);
      }

    }
}

/*void SPI_MasterInit(void){
    PORTB|=(1<<DDB0);

    DDRE |= (1<<DDE4);
    PORTE |= (1<<PORTE4);

    DDRE |= (1<<DDE5);
    PORTE &= ~(1<<PORTE5);



    // Set MOSI and SCK output, all others input
    DDRB = (1<<DDB2)|(1<<DDB1);
    // Enable SPI, Master, set clock rate fck/16
    SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}*/

/*void SPI_MasterTransmit(char cData){
    // Start transmission
    SPDR = cData;
    // Wait for transmission complete
    while(!(SPSR & (1<<SPIF)));
} *///peut etre à mettre je sais pas

/*ISR(INT0_vect){
    //Faire le bidule
    detection_hall++;
    if(detection_hall==0)
    {
      detection_hall=1;
    }
    else
    {
      detection_hall=0;
    }
}

ISR(USART0_RX_vect){
    //Faire le bidule
    compteur_usart++;
    //UDR0;
}*/

/*void set_interrupt(void){

    //etre sûr que TWEN de TWCR est à 0

    DDRD &= ~(1<<DDD0); //Met PORTD0 en Input

    PORTD |= (1<<PORTD0); //Mettre le pull-up resistor

    EICRA |= (1<<ISC01); //Active le mode détection de fronts montants du INT0

    EIMSK |= (1<<INT0); //Met le bit correspondant à l'interruption 0 sur 1 dans le EIMSK

    sei();
    //SREG |= (1<<I); //Met le bit à 1 du global interrupt du status register, permet d'activer les interruptions

}*/


int main(void)
{
compteur_usart = 0;
//uint16_t compteur_usart_precedent = 0;
detection_hall=0;
//uint16_t variable_de_parcours =0;
//SPI_MasterInit(); //peut etre à mettre je sais pas
//set_interrupt();
USART_Init(MYUBRR);

//---------------INITIALISATION VARIABLES V2-------------//
/*uint16_t  matrice[60] = {0, 0, 0, 0, 0, 320, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 4064, 256, 4064, 0, 896, 1344, 832, 0,
                         4064, 0, 4064, 0, 896, 1088, 896, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 320, 0, 0, 0, 0};*/

char string_de_variable[10];
char string_de_matrice_constante[10];
char string_de_matrice_variable[10];
char string_de_chiffre_constante[10];
char string_de_chiffre_variable[10];

itoa(1, string_de_variable, 10);
sprintf(string_de_variable+strlen(string_de_variable),"\n");

itoa(2, string_de_matrice_variable, 10);
sprintf(string_de_matrice_variable+strlen(string_de_matrice_variable),"\n");

itoa(3, string_de_chiffre_constante, 10);
sprintf(string_de_chiffre_constante+strlen(string_de_chiffre_constante),"\n");

itoa(4, string_de_chiffre_variable, 10);
sprintf(string_de_chiffre_variable+strlen(string_de_chiffre_variable),"\n");
itoa(5, string_de_matrice_constante, 10);
sprintf(string_de_matrice_constante+strlen(string_de_matrice_constante),"\n");


 uart_send(string_de_variable);
 uart_send(string_de_matrice_variable);
 uart_send(string_de_chiffre_constante);
 uart_send(string_de_chiffre_variable);
 uart_send(string_de_matrice_constante);

while(1){
}


// Creation d'un pointeur pour parcourir la matrice
/*uint16_t* pt_matrice_parcours;

pt_matrice_parcours = matrice;*/

/*
  if(variable_de_parcours<=10)
  {
    variable_de_parcours++;
  }
  else
  {
    variable_de_parcours=0;
  }

  if(detection_hall>=1)
  {

    char string[64];

    itoa(secondes_unite, string, 10);  //convert integer to string, radix=1
    sprintf(string+strlen(string),"\n");

    uart_send(string);
    detection_hall=0;
  }

  if(compteur_usart_precedent!=compteur_usart)
  {
      char str_usart[64];

      itoa(20000, str_usart, 10);  //convert integer to string, radix=10


      uart_send(str_usart);

      compteur_usart_precedent=compteur_usart;

    }


}*/

}
