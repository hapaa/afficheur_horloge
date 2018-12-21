#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>

#define BAUD 38400
#define MYUBRR F_CPU/16/BAUD-1
int detection_hall;
int compteur_usart;
int compteur_secondes;
int temps;
int compteur_debug;

/*const static uint16_t chiffres[30] = {992, 544, 992,
                          0, 0, 992,
                            736, 672, 928,
                             992, 672, 672,
                              992, 128, 224,
                               928, 672, 736,
                                896, 640, 992,
                                 992, 32, 32,
                                  992, 672, 992,
                                   992, 160, 224};*/

//DDRE |= (1<<DDE2); METTRE PE2 EN OUTPUT, SUPPOSEMENT EQUIVALENT A METTRE XCKO SUR PE2
//PORTE &= ~(1<<PORTE2); FORCER INPUT XCK0 A 0, SUPPOSEMENT EMPECHER MODE CONFIG

void USART_Init (unsigned int ubrr)
{
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

void SPI_MasterInit(void)
{
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


void SPI_MasterTransmit(char cData)
{
    /* Start transmission */
    SPDR = cData;
    /* Wait for transmission complete */
    while(!(SPSR & (1<<SPIF)));
}


void Control_LEDS(uint8_t value1, uint8_t value2)
{


    PORTE |= (1<<PORTE4);

    SPI_MasterTransmit(value1);
    SPI_MasterTransmit(value2);

    PORTE |= (1<<PORTE5);

    PORTE &= ~(1<<PORTE5);

    PORTE &= ~(1<<PORTE4);
}

void Changement_LEDS(uint8_t* value1, uint8_t* value2){
  if(*value2==255)
  {
    *value1=0;
    *value2=0;
  }
  else
  {
    *value1=0;
    *value2=255;
  }
}

ISR(INT0_vect){
    //Faire le bidule
    detection_hall++;
    /*if(detection_hall==0)
    {
      detection_hall=1;
    }
    else
    {
      detection_hall=0;
    }*/
}

ISR(USART0_RX_vect){
    //Faire le bidule
    compteur_usart++;
    UDR0;
}

void set_interrupt(void){

    //etre sûr que TWEN de TWCR est à 0

    DDRD &= ~(1<<DDD0); //Met PORTD0 en Input

    PORTD |= (1<<PORTD0); //Mettre le pull-up resistor

    EICRA |= (1<<ISC01); //Active le mode détection de fronts montants du INT0

    EIMSK |= (1<<INT0); //Met le bit correspondant à l'interruption 0 sur 1 dans le EIMSK

    sei();
    //SREG |= (1<<I); //Met le bit à 1 du global interrupt du status register, permet d'activer les interruptions

}

void init_secondes(void){
  TIMSK |=(1<<OCIE0);

  //Configurer
  TCCR0 |= (1<<CS01)|(1<<CS00);

  uint8_t valeur = 250;
  OCR0 = valeur;

}

void init_temps(void){
  TIMSK |=(1<<OCIE1A);

  //Configurer
  TCCR1A |= (1<<WGM11)|(1<<WGM10);
  TCCR1B |= (1<<CS10)|(1<<WGM12)|(1<<WGM13);

  //Valeur
  //uint16_t  valeur = 200; //Pour aiguilles
  uint16_t  valeur = 1000;

  uint8_t valeur_low = valeur;
  uint8_t valeur_high = valeur>>8;

  OCR1AH=valeur_high;
  OCR1AL=valeur_low;

}

void init_debug(void){
  ETIMSK |=(1<<OCIE3A);

  //Configurer
  TCCR3A |= (1<<WGM31)|(1<<WGM30);
  TCCR3B |= (1<<CS30)|(1<<WGM32)|(1<<WGM33);

  //Valeur
  uint16_t  valeur = 60000;

  uint8_t valeur_low = valeur;
  uint8_t valeur_high = valeur>>8;

  OCR3AH=valeur_high;
  OCR3AL=valeur_low;

}

// Fonction permettant de mettre à jour un chiffre
void update_chiffre(int position_debut, int pos, uint16_t chiffre[], uint16_t matrice[])
{
  matrice[position_debut] = chiffre[0+pos];
  matrice[position_debut+1] = chiffre[1+pos];
  matrice[position_debut+2] = chiffre[2+pos];
}




int main(void)
{

SPI_MasterInit();

//detection_hall=0;

//int temps_precedent=0;

//int compteur_debug_precedent=0;
set_interrupt();
USART_Init(MYUBRR);

//---------------INITIALISATION VARIABLES V2-------------//
/*uint16_t  matrice[60] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                        0, 0, 4064, 256, 4064, 0, 896, 1344, 832, 0,
                        4064, 0, 4064, 0, 896, 1088, 896, 0, 0, 0,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0};*/


int secondes_unite = 0;
while(1){
  if( detection_hall>=1)
  {

    //Changement_LEDS(pt_value1,pt_value2);

   char string[64];

    itoa(secondes_unite, string, 10);  //convert integer to string, radix=1
    sprintf(string+strlen(string),"\n");

    uart_send(string);
    //  char test[] = printf("te", template);
      //USART_Receive();
    //matrice[0] = chiffres[1];
    detection_hall=0;
    temps=0;
  }
  if (secondes_unite<=10)
  {
    secondes_unite++;
  }
  else
  {
    secondes_unite = 0;
  }
}

}
