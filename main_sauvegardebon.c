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

ISR(TIMER0_COMP_vect){
    //Faire le bidule
    compteur_secondes++;
}

ISR(TIMER1_COMPA_vect){
    //Faire le bidule
    temps++;
}

ISR(TIMER3_COMPA_vect){
    //Faire le bidule
    compteur_debug++;
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
compteur_usart = 0;
int compteur_usart_precedent = 0;

compteur_secondes = 0;
/*int secondes =0;
int minutes = 50;
int heures = 15;
//int heures_aiguille=0;
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
  //heures_aiguille=heures-12;
}
else
{
  //heures_aiguille=heures;
}*/ // POUR AIGUILLE


temps=0;


SPI_MasterInit();

//detection_hall=0;

//int temps_precedent=0;
int pas=3;

//int compteur_debug_precedent=0;
set_interrupt();
init_secondes();
init_temps();
USART_Init(MYUBRR);


uint8_t value1 = 0;
uint8_t value2 = 0;

/*uint8_t* pt_value1 = &value1;
uint8_t* pt_value2 = &value2;
Changement_LEDS(pt_value1,pt_value2);*/

//---------------INITIALISATION VARIABLES V2-------------//
uint16_t  matrice[60] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 4064, 256, 4064, 0, 896, 1344, 832, 0,
                         4064, 0, 4064, 0, 896, 1088, 896, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/*uint16_t un[3] = {0, 0, 992};
uint16_t deux[3] = {736, 672, 928};
uint16_t trois[3] = {992, 672, 672};
uint16_t quatre[3] = {992, 128, 224};
uint16_t cinq[3] = {928, 672, 736};
uint16_t six[3] = {224, 160, 992};
uint16_t sept[3] = {992, 32, 32};
uint16_t huit[3] = {992, 672, 992};
uint16_t neuf[3] = {992, 160, 224};
uint16_t zero[3] = {992, 544, 992};*/

uint16_t chiffres[30] = {992, 544, 992,
                          0, 0, 992,
                            736, 672, 928,
                             992, 672, 672,
                              992, 128, 224,
                               928, 672, 736,
                                896, 640, 992,
                                 992, 32, 32,
                                  992, 672, 992,
                                   992, 160, 224};



uint16_t points[3] = {0, 320, 0};

int chemin = 0;
// Creation d'un pointeur pour parcourir la matrice
uint16_t* pt_matrice_parcours;

pt_matrice_parcours = matrice;

//uint16_t* pt_dizaine_min = matrice;

int pos_dizaine_min = 1;
int pos_points_gauche = 4;
int pos_unite_heure = 7;
int pos_dizaine_heure = 11;

int pos_unite_sec = 47;
int pos_dizaine_sec = 51;
int pos_points_droit = 54;
int pos_unite_min = 57;

int secondes_unite = 0;
int secondes_dizaine=4;
int minutes_unite = 9;
int minutes_dizaine = 5;
int heures_unite = 4;
int heures_dizaine = 1;

int secondes =30;
int minutes = 59;
int heures = 14;

update_chiffre(pos_unite_sec, 3*0, chiffres, matrice);
update_chiffre(pos_dizaine_sec, 3*3, chiffres, matrice);
update_chiffre(pos_unite_min, 3*9, chiffres, matrice);
update_chiffre(pos_dizaine_min, 3*5, chiffres, matrice);
update_chiffre(pos_unite_heure, 3*4, chiffres, matrice);
update_chiffre(pos_dizaine_heure,3*1, chiffres, matrice);

update_chiffre(pos_points_droit, 0, points, matrice);
update_chiffre(pos_points_gauche, 0,  points, matrice);





int numero_afficher = 0;
int pos_numero = pos_unite_sec;
while(1){


  //matrice[pos_unite_sec+2] = chiffre[2+val_unite_sec*3];
//update_chiffre(pos_unite_sec, val_unite_sec*3, chiffres, matrice);
// Detection de l'effet hall et calcul du temps du tour et du pas pour /60
  if( detection_hall>=1)
  {

    //Changement_LEDS(pt_value1,pt_value2);

   char string[64];

    itoa(secondes_unite, string, 10);  //convert integer to string, radix=1
    sprintf(string+strlen(string),"\n");

    uart_send(string);
    //  char test[] = printf("te", template);
      //USART_Receive();
    detection_hall=0;
    pas=temps/60;
    temps=0;
  }

// Detection d'un envoi usart
  if(compteur_usart_precedent!=compteur_usart)
  {
      //Changement_LEDS(pt_value1,pt_value2);

      //int num = 1234;
      char str_usart[64];

      itoa(20000, str_usart, 10);  //convert integer to string, radix=10

      //char test[] = printf("te", template);

      //USART_Receive();

      uart_send(str_usart);

      compteur_usart_precedent=compteur_usart;
      temps=0;
      //temps_precedent=0;

    }

// V1 : Appel fonction aiguille

// Calcul de l'heure souhaitee
  /*if(secondes*1625==compteur_secondes)
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
          //  heures_aiguille=heures-12;
          }
          else
          {
          //  heures_aiguille=heures;
          }
        }
      }
    }
if(pas*secondes<=temps && pas*secondes+6>=temps)
  {
      Control_LEDS(255,0);
    }
  else
  {
    if(pas*minutes<=temps && pas*minutes+6>=temps){
          Control_LEDS(255,0);
        }
        else{
            if(pas*5*heures_aiguille<=temps && pas*5*heures_aiguille+6>=temps){
              Control_LEDS(0,7);
              }
            else{
              Control_LEDS(0,0);
                }
              }
  }*/

// V2 : matrice et chiffres
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
          if(heures>=12)
          {
          //  heures_aiguille=heures-12;
          }
          else
          {
          //  heures_aiguille=heures;
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
  Control_LEDS(value2, value1);
}

}