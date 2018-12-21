#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
<<<<<<< HEAD

#define BAUD 38400
#define MYUBRR F_CPU / 16 / BAUD - 1
int compteur;
=======
#include <stdlib.h>
#include <string.h>

#define BAUD 38400
#define MYUBRR F_CPU/16/BAUD-1
<<<<<<< HEAD
int detection_hall;
>>>>>>> 34ea91b378d3600105db8fac1d57cb02c8ce9ca9
int compteur_usart;
int compteur_secondes;
int temps;
int compteur_debug;
=======

uint16_t detection_hall;
uint16_t compteur_usart;
uint16_t compteur_secondes;
uint16_t temps;
uint16_t compteur_debug;

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
>>>>>>> 5f8da9c699e8861185083444fff73dc9443b334a

//DDRE |= (1<<DDE2); METTRE PE2 EN OUTPUT, SUPPOSEMENT EQUIVALENT A METTRE XCKO SUR PE2
//PORTE &= ~(1<<PORTE2); FORCER INPUT XCK0 A 0, SUPPOSEMENT EMPECHER MODE CONFIG

<<<<<<< HEAD
void USART_Init(unsigned int ubrr)
{
    /* Set baud rate */
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;
    /* Enable receiver and transmitter */
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
    /* Set frame format: 8data, 2stop bit */
    UCSR0C = (1 << USBS0) | (1 << UCSZ00) | (1 << UCSZ01);
=======
void USART_Init (unsigned int ubrr) {
/* Set baud rate */
UBRR0H = (unsigned char)(ubrr>>8);
UBRR0L = (unsigned char)ubrr;
/* Enable receiver and transmitter */
UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(1<<RXCIE0);
/* Set frame format: 8data, 2stop bit */
UCSR0C = (1<<USBS0)|(1<<UCSZ00)|(1<<UCSZ01);
>>>>>>> 5f8da9c699e8861185083444fff73dc9443b334a
}

<<<<<<< HEAD
void USART_Transmit(unsigned char data)
{
    /* Wait for empty transmit buffer */
    //while ( !( UCSR0A & (1<<UDRE0)) );
    /* Put data into buffer, sends the data */
    UDR0 = data;
=======

void USART_Transmit( unsigned char data ) {
/* Wait for empty transmit buffer */
while ( !( UCSR0A & (1<<UDRE0)) );
/* Put data into buffer, sends the data */
UDR0 = data;
>>>>>>> 34ea91b378d3600105db8fac1d57cb02c8ce9ca9
}

unsigned char USART_Receive(void)
{
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

<<<<<<< HEAD
void SPI_MasterInit(void)
{
    PORTB |= (1 << DDB0);

    DDRE |= (1 << DDE4);
    PORTE |= (1 << PORTE4);

    DDRE |= (1 << DDE5);
    PORTE &= ~(1 << PORTE5);

=======
void SPI_MasterInit(void) {
    PORTB|=(1<<DDB0);

    DDRE |= (1<<DDE4);
    PORTE |= (1<<PORTE4);

    DDRE |= (1<<DDE5);
    PORTE &= ~(1<<PORTE5);

>>>>>>> 5f8da9c699e8861185083444fff73dc9443b334a
    /* Set MOSI and SCK output, all others input */
    DDRB = (1 << DDB2) | (1 << DDB1);
    /* Enable SPI, Master, set clock rate fck/16 */
<<<<<<< HEAD
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

void SPI_MasterTransmit(char cData)
{
=======
    SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
}


void SPI_MasterTransmit(char cData) {
>>>>>>> 5f8da9c699e8861185083444fff73dc9443b334a
    /* Start transmission */
    SPDR = cData;
    /* Wait for transmission complete */
    while (!(SPSR & (1 << SPIF)))
        ;
}

<<<<<<< HEAD
void Control_LEDS(uint8_t value1, uint8_t value2)
{

    PORTE |= (1 << PORTE4);
=======

void Control_LEDS(uint8_t value1, uint8_t value2) {

    PORTE |= (1<<PORTE4);
>>>>>>> 5f8da9c699e8861185083444fff73dc9443b334a

    SPI_MasterTransmit(value1);
    SPI_MasterTransmit(value2);

    PORTE |= (1 << PORTE5);

    PORTE &= ~(1 << PORTE5);

    PORTE &= ~(1 << PORTE4);
}

<<<<<<< HEAD
<<<<<<< HEAD
ISR(INT0_vect)
{
=======
void Changement_LEDS(uint8_t* value1, uint8_t* value2){
=======
void Changement_LEDS(uint8_t* value1, uint8_t* value2) {
>>>>>>> 5f8da9c699e8861185083444fff73dc9443b334a
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

<<<<<<< HEAD
ISR(INT0_vect){
>>>>>>> 34ea91b378d3600105db8fac1d57cb02c8ce9ca9
    //Faire le bidule
=======
ISR(INT0_vect) {
    // Action
>>>>>>> 5f8da9c699e8861185083444fff73dc9443b334a
    detection_hall++;
}

<<<<<<< HEAD
ISR(USART0_RX_vect)
{
    //Faire le bidule
=======
ISR(USART0_RX_vect) {
  // Action
>>>>>>> 5f8da9c699e8861185083444fff73dc9443b334a
    compteur_usart++;
    UDR0;
}

ISR(TIMER0_COMP_vect) {
  // Action
    compteur_secondes++;
}

<<<<<<< HEAD
ISR(TIMER1_COMPA_vect)
{
    //Faire le bidule
=======
ISR(TIMER1_COMPA_vect) {
  // Action
>>>>>>> 5f8da9c699e8861185083444fff73dc9443b334a
    temps++;
}

ISR(TIMER3_COMPA_vect) {
  // Action
    compteur_debug++;
}

<<<<<<< HEAD
void set_interrupt(void)
{
=======
void set_interrupt(void) {
>>>>>>> 5f8da9c699e8861185083444fff73dc9443b334a

    //etre sûr que TWEN de TWCR est à 0

    DDRD &= ~(1 << DDD0); //Met PORTD0 en Input

    PORTD |= (1 << PORTD0); //Mettre le pull-up resistor

<<<<<<< HEAD
    EICRA |= (1 << ISC01) | (1 << ISC00); //Active le mode détection de fronts montants du INT0
=======
    EICRA |= (1<<ISC01); //Active le mode détection de fronts montants du INT0
>>>>>>> 34ea91b378d3600105db8fac1d57cb02c8ce9ca9

    EIMSK |= (1 << INT0); //Met le bit correspondant à l'interruption 0 sur 1 dans le EIMSK

    sei();
    //SREG |= (1<<I); //Met le bit à 1 du global interrupt du status register, permet d'activer les interruptions
}

<<<<<<< HEAD
<<<<<<< HEAD
void init_horloge(void)
{
    TIMSK |= (1 << OCIE1A);

    //Configurer
    TCCR1A |= (1 << WGM11) | (1 << WGM10);
    TCCR1B |= (1 << CS11) | (1 << CS10) | (1 << WGM12) | (1 << WGM13);
=======
void init_secondes(void){
=======
void init_secondes(void) {
>>>>>>> 5f8da9c699e8861185083444fff73dc9443b334a
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
<<<<<<< HEAD
  uint16_t  valeur = 1000;
>>>>>>> 34ea91b378d3600105db8fac1d57cb02c8ce9ca9
=======
  uint16_t  valeur = 1000; //Pour umérique
>>>>>>> 5f8da9c699e8861185083444fff73dc9443b334a

    //Valeur
    uint16_t valeur = 40625;

    uint8_t valeur_low = valeur;
    uint8_t valeur_high = valeur >> 8;

    OCR1AH = valeur_high;
    OCR1AL = valeur_low;
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
void update_chiffre(uint16_t position_debut, uint16_t pos, uint16_t matrice[])
{
  matrice[position_debut] = chiffres[0+pos];
  matrice[position_debut+1] = chiffres[1+pos];
  matrice[position_debut+2] = chiffres[2+pos];
}




int main(void)
{
<<<<<<< HEAD
<<<<<<< HEAD
    /*USART_Init(MYUBRR);
while (1){
USART_Transmit(USART_Receive()+1);
}*/
    compteur = 0;
    compteur_usart = 0;
    int compteur_usart_precedent = 0;
    SPI_MasterInit();

    int compteur_precedent = 0;
    int compteur_timer_precedent = 0;
    set_interrupt();
    init_horloge();
    USART_Init(MYUBRR);

    uint8_t value1 = 0;
    uint8_t value2 = 0;

    while (1)
    {
        if (compteur_precedent != compteur)
        {
            if (value1 == 255)
            {
                value1 = 0;
                value2 = 0;
            }
            else
            {
                value1 = 255;
                value2 = 255;
            }
            compteur_precedent = compteur;
        }
        Control_LEDS(value1, value2);

        if (compteur_usart_precedent != compteur_usart)
        {
            unsigned char test = 'o';
            USART_Receive();

            USART_Transmit(test);
            compteur_usart_precedent = compteur_usart;
        }

        if (compteur_timer_precedent * 5 == compteur_timer)
        {
            if (value1 == 255)
            {
                value1 = 0;
                value2 = 0;
            }
            else
            {
                value1 = 255;
                value2 = 255;
            }

            compteur_timer_precedent++;
        }
    }
=======
=======
uint16_t version = 2;
>>>>>>> 5f8da9c699e8861185083444fff73dc9443b334a
compteur_usart = 0;
uint16_t compteur_usart_precedent = 0;

compteur_secondes = 0;
temps=0;
SPI_MasterInit();
uint16_t pas=3;
//int compteur_debug_precedent=0;
set_interrupt();
init_secondes();
init_temps();
USART_Init(MYUBRR);


uint8_t value1 = 0;
uint8_t value2 = 0;
// Heure choisie
uint16_t secondes =30;
uint16_t minutes = 59;
uint16_t heures = 14;


// -------------------- V1 Aiguilles INITIALISATION --------------------
uint16_t heures_aiguille=0;

if(version==1){

secondes+=30;
minutes+=30;
heures+=6;

if(secondes>=60){
  secondes-=60;
}

if(minutes>=60)
{
  minutes-=60;
}

if (heures>=24)
{
  heures-=24;
}

if(heures>=12)
{
  heures_aiguille=heures-12;
}
else
{
  heures_aiguille=heures;
}
}

// -------------------- V2 Numérique INITIALISATION --------------------

uint16_t  matrice[60] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 4064, 256, 4064, 0, 896, 1344, 832, 0,
                         4064, 0, 4064, 0, 896, 1088, 896, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


//uint16_t points[3] = {0, 320, 0};

uint16_t chemin = 0;
// Creation d'un pointeur pour parcourir la matrice
uint16_t* pt_matrice_parcours;
pt_matrice_parcours = matrice;


uint16_t pos_dizaine_min = 1;
//uint16_t pos_points_gauche = 4;
uint16_t pos_unite_heure = 7;
uint16_t pos_dizaine_heure = 11;

uint16_t pos_unite_sec = 47;
uint16_t pos_dizaine_sec = 51;
// uint16_t pos_points_droit = 54;
uint16_t pos_unite_min = 57;

uint16_t secondes_unite = 0;
uint16_t secondes_dizaine=3;
uint16_t minutes_unite = 9;
uint16_t minutes_dizaine = 5;
uint16_t heures_unite = 4;
uint16_t heures_dizaine = 1;

update_chiffre(pos_unite_sec, 3*0, matrice);
update_chiffre(pos_dizaine_sec, 3*3, matrice);
update_chiffre(pos_unite_min, 3*9, matrice);
update_chiffre(pos_dizaine_min, 3*5, matrice);
update_chiffre(pos_unite_heure, 3*4, matrice);
update_chiffre(pos_dizaine_heure,3*1, matrice);

//update_chiffre(pos_points_droit, 0, points, matrice);
//update_chiffre(pos_points_gauche, 0,  points, matrice);


uint16_t numero_afficher = 0;
uint16_t pos_numero = pos_unite_sec;

Control_LEDS(value2, value1);
while(1){

// Detection de l'effet hall et calcul du temps du tour et du pas pour /60
  if( detection_hall>=1) {

    //Changement_LEDS(pt_value1,pt_value2);
   char string[64];
   itoa(secondes_unite, string, 10);  //convert integer to string, radix=1
   uart_send(string);

// ------------- TEST BUG à ENLEVER ------------------ //

  /*char string1[64];
   char string2[64];

           itoa(chiffres[0], string2, 10);
     itoa(chiffres[secondes_unite], string1, 10);  //convert integer to string, radix=1

      uart_send(string2);
      uart_send(string1);*/
//------------------------------------
    detection_hall=0;
    pas=temps/60;
    temps=0;
  }

// Detection d'un envoi usart
  if(compteur_usart_precedent!=compteur_usart) {

      char str_usart[64];

      itoa(20000, str_usart, 10);  //convert integer to string, radix=10


      uart_send(str_usart);

      compteur_usart_precedent=compteur_usart;
      temps=0;
    }

// -------------------- V1 Aiguilles FONCTIONNEMENT --------------------
if(version==1) {
// Calcul de l'heure souhaitee
  if(secondes*1625==compteur_secondes)
  {
      //Changement_LEDS(pt_value1,pt_value2);
      secondes++;
      if (secondes==60)
      {
        compteur_secondes=0;
        secondes=0;
        minutes++;
        if(minutes==60)
        {
          minutes=0;
          heures++;

          if(heures==24)
          {
            heures=0;
          }
          if(heures>=12)
          {
            heures_aiguille=heures-12;
          }
          else
          {
            heures_aiguille=heures;
          }
        }
      }
    }

if(pas*secondes<=temps && pas*secondes+20>=temps)
  {
      Control_LEDS(255,0);
    }
  else
  {
    if(pas*minutes<=temps && pas*minutes+20>=temps){
          Control_LEDS(255,0);
        }
        else{
            if(pas*5*heures_aiguille<=temps && pas*5*heures_aiguille+20>=temps){
              Control_LEDS(0,7);
              }
            else{
              Control_LEDS(0,0);
                }
              }
  }
}

// -------------------- V2 Numerique FONCTIONNEMENT --------------------
if(version==2) {
// Calcul de l'heure souhaitee
if(numero_afficher==0)
{
  matrice[pos_numero] = chiffres[0];
  matrice[pos_numero + 1] = chiffres[1];
  matrice[pos_numero + 2] = chiffres[2];
}
if(numero_afficher==1)
{
  matrice[pos_numero] = chiffres[3];
  matrice[pos_numero + 1] = chiffres[4];
  matrice[pos_numero + 2] = chiffres[5];
}
if(numero_afficher==2)
{
  matrice[pos_numero] = chiffres[6];
  matrice[pos_numero + 1] = chiffres[7];
  matrice[pos_numero + 2] = chiffres[8];
}
if(numero_afficher==3)
{
  matrice[pos_numero] = chiffres[9];
  matrice[pos_numero + 1] = chiffres[10];
  matrice[pos_numero + 2] = chiffres[11];
}
if(numero_afficher==4)
{
  matrice[pos_numero] = chiffres[12];
  matrice[pos_numero + 1] = chiffres[13];
  matrice[pos_numero + 2] = chiffres[14];
}
if(numero_afficher==5)
{
  matrice[pos_numero] = chiffres[15];
  matrice[pos_numero + 1] = chiffres[16];
  matrice[pos_numero + 2] = chiffres[17];
}
if(numero_afficher==6)
{
  matrice[pos_numero] = chiffres[18];
  matrice[pos_numero + 1] = chiffres[19];
  matrice[pos_numero + 2] = chiffres[20];
}
if(numero_afficher==7)
{
  matrice[pos_numero] = chiffres[21];
  matrice[pos_numero + 1] = chiffres[22];
  matrice[pos_numero + 2] = chiffres[23];
}
if(numero_afficher==8)
{
  matrice[pos_numero] = chiffres[24];
  matrice[pos_numero + 1] = chiffres[25];
  matrice[pos_numero + 2] = chiffres[26];
}
if(numero_afficher==9)
{
  matrice[pos_numero] = chiffres[27];
  matrice[pos_numero + 1] = chiffres[28];
  matrice[pos_numero + 2] = chiffres[29];
}

if(!secondes_unite)
{
  matrice[pos_unite_sec] = chiffres[0];
  matrice[pos_unite_sec + 1] = chiffres[1];
  matrice[pos_unite_sec + 2] = chiffres[2];
}
if(!secondes_dizaine)
{
  matrice[pos_dizaine_sec] = chiffres[0];
  matrice[pos_dizaine_sec + 1] = chiffres[1];
  matrice[pos_dizaine_sec + 2] = chiffres[2];
}
if(!minutes_unite)
{
  matrice[pos_unite_min] = chiffres[0];
  matrice[pos_unite_min + 1] = chiffres[1];
  matrice[pos_unite_min + 2] = chiffres[2];
}
if(!minutes_dizaine)
{
  matrice[pos_dizaine_min] = chiffres[0];
  matrice[pos_dizaine_min + 1] = chiffres[1];
  matrice[pos_dizaine_min + 2] = chiffres[2];
}
if(!heures_unite)
{
  matrice[pos_unite_heure] = chiffres[0];
  matrice[pos_unite_heure + 1] = chiffres[1];
  matrice[pos_unite_heure + 2] = chiffres[2];
}
if(!heures_dizaine)
{
  matrice[pos_dizaine_heure] = chiffres[0];
  matrice[pos_dizaine_heure + 1] = chiffres[1];
  matrice[pos_dizaine_heure + 2] = chiffres[2];
}


  if((secondes+1)*1625==compteur_secondes)
  {
      //Changement_LEDS(pt_value1,pt_value2);
      secondes++;
      secondes_unite++;
      numero_afficher = secondes_unite;
      pos_numero = pos_unite_sec;
      if(secondes_unite >=10)
      {
        secondes_unite = 0;
        secondes_dizaine++;
        numero_afficher = secondes_dizaine;
        pos_numero = pos_dizaine_sec;
      }
      if (secondes>=60)
      {
        compteur_secondes=0;
        secondes_unite=0;
        secondes_dizaine=0;
        secondes=0;
        minutes++;
        minutes_unite++;
        numero_afficher = minutes_unite;
        pos_numero = pos_unite_min;
        if(minutes_unite >=10)
        {
          minutes_unite = 0;
          minutes_dizaine++;
          numero_afficher = minutes_dizaine;
          pos_numero = pos_dizaine_min;
        }
        if(minutes>=60)
        {
          minutes=0;
          minutes_unite=0;
          minutes_dizaine=0;
          heures++;
          heures_unite++;
          numero_afficher = heures_unite;
          pos_numero = pos_unite_heure;
        if(heures_unite >=10)
        {
          heures_unite = 0;
          heures_dizaine++;
          numero_afficher = heures_dizaine;
          pos_numero = pos_dizaine_heure;
        }

          if(heures>=24)
          {
            heures=0;
            heures_unite=0;
            heures_dizaine=0;
            numero_afficher = heures_unite;
            pos_numero = pos_unite_heure;
          }
        }

      }

}
  if(pas*chemin <= temps  && pas*chemin+1 >=temps)
  {
  pt_matrice_parcours++;
  chemin++;
  if (chemin==60)
  {chemin=0;
  pt_matrice_parcours = matrice;}

}
value1 = *pt_matrice_parcours;
value2 = *pt_matrice_parcours>>8;
<<<<<<< HEAD
  Control_LEDS(value2, value1);
>>>>>>> 34ea91b378d3600105db8fac1d57cb02c8ce9ca9
=======
Control_LEDS(value2, value1);
}

}

>>>>>>> 5f8da9c699e8861185083444fff73dc9443b334a
}
