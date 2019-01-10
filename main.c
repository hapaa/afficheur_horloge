#include <stdio.h>
  #include <avr/io.h>
  #include <util/delay.h>
  #include <avr/interrupt.h>
  #include <stdlib.h>
  #include <string.h>

  #define BAUD 38400
  #define MYUBRR F_CPU/16/BAUD-1
  volatile uint16_t detection_hall;
  volatile uint16_t compteur_usart;
  volatile uint16_t compteur_secondes;
  volatile uint16_t temps;
  volatile uint16_t compteur_debug;
  unsigned char changement_mode;

  static const uint16_t pos_unite_sec = 47;
  static const uint16_t pos_dizaine_sec = 51;
  static const uint16_t pos_unite_min = 57;
  static const uint16_t pos_dizaine_min = 1;
  static const uint16_t pos_unite_heure = 7;
  static const uint16_t pos_dizaine_heure = 11;
  uint16_t aiguille = 0;

  //DDRE |= (1<<DDE2); METTRE PE2 EN OUTPUT, SUPPOSEMENT EQUIVALENT A METTRE XCKO SUR PE2
  //PORTE &= ~(1<<PORTE2); FORCER INPUT XCK0 A 0, SUPPOSEMENT EMPECHER MODE CONFIG

  void USART_Init(unsigned int ubrr)
  {/* Initialisation de l'USART */
    // Initisialisation de la Baudrate à la valeur ubrr
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;
    // Autorisation de la réception et de l'envoi
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
    // Set frame format: 8data, 2stop bit
    UCSR0C = (1 << USBS0) | (1 << UCSZ00) | (1 << UCSZ01);
  }


  void USART_Transmit(unsigned char data)
  { /* Transmission de l'USART */
    // Attend que le buffer transmit soit vide
      while (!(UCSR0A & (1 << UDRE0)));
    // Met la données dans un buffer et l'envoi
    UDR0 = data;
  }


  unsigned char USART_Receive(void)
  {/* Réception de l'USART */
    //Retourne les données reçues
    return UDR0;
  }

  void uart_send(char *str)
  { /* Envoi une information via l'USART */
    while (*str != '\0')
    {
      if (UCSR0A & _BV(UDRE0))
      {
        UDR0 = *(str++);
      }
    }
  }

  void SPI_MasterInit(void)
  { /* Initialisation de la transmission SPI pour les LEDS */
    PORTB |= (1 << DDB0);

    DDRE |= (1 << DDE4);
    PORTE |= (1 << PORTE4);

    DDRE |= (1 << DDE5);
    PORTE &= ~(1 << PORTE5);

    /* Set MOSI and SCK output, all others input */
    DDRB = (1 << DDB2) | (1 << DDB1);
    /* Enable SPI, Master, set clock rate fck/16 */
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
  }


  void SPI_MasterTransmit(char cData)
  { /* Début de la tranmission SPI*/
    SPDR = cData;
    // Attend que la transmission soit complète
    while (!(SPSR & (1 << SPIF)));
  }


  void Control_LEDS(uint8_t value1, uint8_t value2)
  {/* Permet d'allumer les LEDS voulues */

    PORTE |= (1 << PORTE4);

    SPI_MasterTransmit(value1);
    SPI_MasterTransmit(value2);

    PORTE |= (1 << PORTE5);

    PORTE &= ~(1 << PORTE5);

    PORTE &= ~(1 << PORTE4);
  }

  void Changement_LEDS(uint8_t *value1, uint8_t *value2)
  { /* Fonction de debug permet d'alterner allumage et éteignage LEDS */
    if (*value2 == 255)
    {
      *value1 = 0;
      *value2 = 0;
    }
    else
    {
      *value1 = 0;
      *value2 = 255;
    }
  }

  ISR(INT0_vect)
  {/* Interuption due à l'effet hall*/
    detection_hall++;
  }

  ISR(USART0_RX_vect)
  {/* Interuption due à la réception d'un élément usart*/
    compteur_usart++;
    changement_mode = UDR0;
  }

  ISR(TIMER0_COMP_vect)
  {/* Interuption due à l'overflow sur le timer 0
   Permet de compter les secondes */
    compteur_secondes++;
  }

  ISR(TIMER1_COMPA_vect)
  {/* Interuption due à l'overflow sur le timer 1
    Permet de compter précisemment le temps passé en unités arbitraires*/
    temps++;
  }

  ISR(TIMER3_COMPA_vect)
  {/* Interuption due à l'overflow sur le timer 3
    Permet de compter précisemment le temps passé en unités arbitraires
    Sert pour le debug et mesurer le temps d'execution d'une fonction */
    compteur_debug++;
  }

  void set_interrupt(void)
  { /* Autoriser les interuptions */
    // S'assurer que TWEN de TWCR est à 0

    DDRD &= ~(1 << DDD0); // Met PORTD0 en Input

    PORTD |= (1 << PORTD0); // Mettre le pull-up resistor

    EICRA |= (1 << ISC01); // Active le mode détection de fronts descendant du INT0

    EIMSK |= (1 << INT0); // Met le bit correspondant à l'interruption 0 sur 1 dans le EIMSK

    sei();
  }

  void init_secondes(void)
  {/* Initialisation du compteur de secondes en 8bits*/

    // Mode de comparaison avec une valeur mise dans un registre
    TIMSK |= (1 << OCIE0);

    //Configurer prescaler à 32
    TCCR0 |= (1 << CS01) | (1 << CS00);

    // Valeur à comparer au timer
    uint8_t valeur = 250;
    OCR0 = valeur;
  }

  void init_temps(void)
  {/* Initialisation du compteur de temps en 16bits*/

    // Mode de comparaison avec une valeur mise dans un registre A
    TIMSK |= (1 << OCIE1A);

    // Configurer prescalar CS10 sans prescalar
    // Mode OCRnA Bottom  (Update) Top (flag) WGM
    TCCR1A |= (1 << WGM11) | (1 << WGM10);
    TCCR1B |= (1 << CS10) | (1 << WGM12) | (1 << WGM13);

    // Valeur à comparer au timer
    uint16_t valeur = 200;

    uint8_t valeur_low = valeur;
    uint8_t valeur_high = valeur >> 8;

    OCR1AH = valeur_high;
    OCR1AL = valeur_low;
  }

  void init_temps_changement_mode(uint16_t valeur)
  {/* Change l'overflow du compteur temps */

    uint8_t valeur_low = valeur;
    uint8_t valeur_high = valeur >> 8;

    OCR1AH = valeur_high;
    OCR1AL = valeur_low;
  }

  void init_debug(void)
  {/* Initialisation du compteur de debug en 16bits*/

    // Mode de comparaison avec une valeur mise dans un registre A
    ETIMSK |= (1 << OCIE3A);

    // Configurer prescalar CS30 sans prescalar
    // Mode OCRnA Bottom  (Update) Top (flag) WGM
    TCCR3A |= (1 << WGM31) | (1 << WGM30);
    TCCR3B |= (1 << CS30) | (1 << WGM32) | (1 << WGM33);

    // Valeur à comparer au timer
    uint16_t valeur = 10000;

    uint8_t valeur_low = valeur;
    uint8_t valeur_high = valeur >> 8;

    OCR3AH = valeur_high;
    OCR3AL = valeur_low;
  }

  void update_chiffre(uint16_t position_debut, uint16_t pos, uint16_t matrice[])
  { /* Fonction permettant de mettre à jour un chiffre dans la V2 */
    if (pos == 0)
    {
      matrice[position_debut] = 992;
      matrice[position_debut + 1] = 544;
      matrice[position_debut + 2] = 992;
    }
    if (pos == 1)
    {
      matrice[position_debut] = 0;
      matrice[position_debut + 1] = 0;
      matrice[position_debut + 2] = 992;
    }
    if (pos == 2)
    {
      matrice[position_debut] = 736;
      matrice[position_debut + 1] = 672;
      matrice[position_debut + 2] = 928;
    }
    if (pos == 3)
    {
      matrice[position_debut] = 992;
      matrice[position_debut + 1] = 672;
      matrice[position_debut + 2] = 672;
    }

    if (pos == 4)
    {
      matrice[position_debut] = 992;
      matrice[position_debut + 1] = 128;
      matrice[position_debut + 2] = 224;
    }
    if (pos == 5)
    {
      matrice[position_debut] = 928;
      matrice[position_debut + 1] = 672;
      matrice[position_debut + 2] = 736;
    }

    if (pos == 6)
    {
      matrice[position_debut] = 896;
      matrice[position_debut + 1] = 640;
      matrice[position_debut + 2] = 992;
    }
    if (pos == 7)
    {
      matrice[position_debut] = 992;
      matrice[position_debut + 1] = 32;
      matrice[position_debut + 2] = 32;
    }
    if (pos == 8)
    {
      matrice[position_debut] = 992;
      matrice[position_debut + 1] = 672;
      matrice[position_debut + 2] = 992;
    }
    if (pos == 9)
    {
      matrice[position_debut] = 992;
      matrice[position_debut + 1] = 160;
      matrice[position_debut + 2] = 224;
    }
  }

  void update_cadrant_BG( uint16_t numero, uint16_t matrice[])
  { /* Fonction permettant de mettre à jour le cadrant en bas à gauche pour la V3 */
    if (numero == 0)
    {
      matrice[ 0 ] =  0 ;

matrice[ 1 ] =  0 ;

matrice[ 2 ] =  0 ;

matrice[ 3 ] =  1024 ;

matrice[ 4 ] =  768 ;

matrice[ 5 ] =  1152 ;

matrice[ 6 ] =  832 ;

matrice[ 7 ] =  1696 ;

matrice[ 8 ] =  2128 ;

matrice[ 9 ] =  1024 ;

matrice[ 10 ] =  2080 ;

matrice[ 11 ] =  5128 ;

matrice[ 12 ] =  6176 ;

matrice[ 13 ] =  12288 ;

matrice[ 14 ] =  2052 ;

matrice[ 15 ] =  4112 ;

matrice[ 16 ] =  5120 ;

matrice[ 17 ] =  3072 ;

matrice[ 18 ] =  1032 ;

matrice[ 19 ] =  512 ;

matrice[ 20 ] =  1024 ;

matrice[ 21 ] =  276 ;

matrice[ 22 ] =  544 ;

matrice[ 23 ] =  840 ;

matrice[ 24 ] =  128 ;

matrice[ 25 ] =  784 ;

matrice[ 26 ] =  96 ;

matrice[ 27 ] =  896 ;

matrice[ 28 ] =  0 ;

matrice[ 29 ] =  0 ;
    }
    if (numero == 1)
    {
      matrice[0] = 0;
      matrice[1] = 0;
      matrice[2] = 0;
      matrice[3] = 0;
      matrice[4] = 0;

      matrice[5] = 0;
      matrice[6] = 512;
      matrice[7] = 512;
      matrice[8] = 0;
      matrice[9] = 1024;

      matrice[10] = 512;
      matrice[11] = 1280;
      matrice[12] = 3072;
      matrice[13] = 4736;
      matrice[14] = 128;

      matrice[15] = 256;
      matrice[16] = 128;
      matrice[17] = 64;
      matrice[18] = 128;
      matrice[19] = 32;

      matrice[20] = 0;
      matrice[21] = 64;
      matrice[22] = 32;
      matrice[23] = 64;
      matrice[24] = 0;

      matrice[25] = 0;
      matrice[26] = 96;
      matrice[27] = 128;
      matrice[28] = 0;
      matrice[29] = 0;
    }
    if (numero == 2)
    {
      matrice[ 0 ] =  0 ;

matrice[ 1 ] =  0 ;

matrice[ 2 ] =  0 ;

matrice[ 3 ] =  1024 ;

matrice[ 4 ] =  512 ;

matrice[ 5 ] =  1024 ;

matrice[ 6 ] =  576 ;

matrice[ 7 ] =  1568 ;

matrice[ 8 ] =  2128 ;

matrice[ 9 ] =  1024 ;

matrice[ 10 ] =  2080 ;

matrice[ 11 ] =  5256 ;

matrice[ 12 ] =  6240 ;

matrice[ 13 ] =  12416 ;

matrice[ 14 ] =  16548 ;

matrice[ 15 ] =  4368 ;

matrice[ 16 ] =  12928 ;

matrice[ 17 ] =  7232 ;

matrice[ 18 ] =  5512 ;

matrice[ 19 ] =  2560 ;

matrice[ 20 ] =  1280 ;

matrice[ 21 ] =  2308 ;

matrice[ 22 ] =  1536 ;

matrice[ 23 ] =  8 ;

matrice[ 24 ] =  0 ;

matrice[ 25 ] =  16 ;

matrice[ 26 ] =  96 ;

matrice[ 27 ] =  896 ;

matrice[ 28 ] =  1024 ;

matrice[ 29 ] =  0 ;
    }
    if (numero == 3)
    {
      matrice[ 0 ] =  0 ;

matrice[ 1 ] =  1024 ;

matrice[ 2 ] =  896 ;

matrice[ 3 ] =  1120 ;

matrice[ 4 ] =  768 ;

matrice[ 5 ] =  1152 ;

matrice[ 6 ] =  840 ;

matrice[ 7 ] =  1696 ;

matrice[ 8 ] =  2116 ;

matrice[ 9 ] =  1024 ;

matrice[ 10 ] =  2080 ;

matrice[ 11 ] =  5256 ;

matrice[ 12 ] =  6208 ;

matrice[ 13 ] =  12416 ;

matrice[ 14 ] =  16518 ;

matrice[ 15 ] =  4368 ;

matrice[ 16 ] =  8832 ;

matrice[ 17 ] =  1024 ;

matrice[ 18 ] =  264 ;

matrice[ 19 ] =  512 ;

matrice[ 20 ] =  0 ;

matrice[ 21 ] =  20 ;

matrice[ 22 ] =  32 ;

matrice[ 23 ] =  72 ;

matrice[ 24 ] =  128 ;

matrice[ 25 ] =  784 ;

matrice[ 26 ] =  1120 ;

matrice[ 27 ] =  896 ;

matrice[ 28 ] =  1024 ;

matrice[ 29 ] =  0 ;
    }
    if (numero == 4)
    {
      matrice[ 0 ] =  0 ;

matrice[ 1 ] =  0 ;

matrice[ 2 ] =  0 ;

matrice[ 3 ] =  1024 ;

matrice[ 4 ] =  768 ;

matrice[ 5 ] =  1152 ;

matrice[ 6 ] =  832 ;

matrice[ 7 ] =  160 ;

matrice[ 8 ] =  80 ;

matrice[ 9 ] =  0 ;

matrice[ 10 ] =  32 ;

matrice[ 11 ] =  136 ;

matrice[ 12 ] =  96 ;

matrice[ 13 ] =  128 ;

matrice[ 14 ] =  164 ;

matrice[ 15 ] =  272 ;

matrice[ 16 ] =  128 ;

matrice[ 17 ] =  1088 ;

matrice[ 18 ] =  1160 ;

matrice[ 19 ] =  512 ;

matrice[ 20 ] =  1024 ;

matrice[ 21 ] =  260 ;

matrice[ 22 ] =  512 ;

matrice[ 23 ] =  776 ;

matrice[ 24 ] =  0 ;

matrice[ 25 ] =  768 ;

matrice[ 26 ] =  0 ;

matrice[ 27 ] =  768 ;

matrice[ 28 ] =  0 ;

matrice[ 29 ] =  0 ;
    }

    if (numero == 5)
    {
      matrice[ 0 ] =  1 ;

matrice[ 1 ] =  1024 ;

matrice[ 2 ] =  896 ;

matrice[ 3 ] =  1120 ;

matrice[ 4 ] =  784 ;

matrice[ 5 ] =  1152 ;

matrice[ 6 ] =  832 ;

matrice[ 7 ] =  1696 ;

matrice[ 8 ] =  2128 ;

matrice[ 9 ] =  1024 ;

matrice[ 10 ] =  2080 ;

matrice[ 11 ] =  5248 ;

matrice[ 12 ] =  6240 ;

matrice[ 13 ] =  12416 ;

matrice[ 14 ] =  16546 ;

matrice[ 15 ] =  4352 ;

matrice[ 16 ] =  8832 ;

matrice[ 17 ] =  1088 ;

matrice[ 18 ] =  1408 ;

matrice[ 19 ] =  2560 ;

matrice[ 20 ] =  1280 ;

matrice[ 21 ] =  2308 ;

matrice[ 22 ] =  1536 ;

matrice[ 23 ] =  520 ;

matrice[ 24 ] =  1024 ;

matrice[ 25 ] =  528 ;

matrice[ 26 ] =  1120 ;

matrice[ 27 ] =  896 ;

matrice[ 28 ] =  1024 ;

matrice[ 29 ] =  0 ;
    }
  }


  void update_cadrant_HG( uint16_t numero, uint16_t matrice[])
  { /* Fonction permettant de mettre à jour le cadrant en haut à gauche pour la V3 */
    if (numero == 0)
    {
      matrice[ 30 ] =  0 ;

matrice[ 31 ] =  0 ;

matrice[ 32 ] =  384 ;

matrice[ 33 ] =  96 ;

matrice[ 34 ] =  272 ;

matrice[ 35 ] =  128 ;

matrice[ 36 ] =  264 ;

matrice[ 37 ] =  128 ;

matrice[ 38 ] =  260 ;

matrice[ 39 ] =  256 ;

matrice[ 40 ] =  512 ;

matrice[ 41 ] =  256 ;

matrice[ 42 ] =  1024 ;

matrice[ 43 ] =  1536 ;

matrice[ 44 ] =  2054 ;

matrice[ 45 ] =  512 ;

matrice[ 46 ] =  5120 ;

matrice[ 47 ] =  3072 ;

matrice[ 48 ] =  1032 ;

matrice[ 49 ] =  512 ;

matrice[ 50 ] =  1024 ;

matrice[ 51 ] =  276 ;

matrice[ 52 ] =  544 ;

matrice[ 53 ] =  840 ;

matrice[ 54 ] =  128 ;

matrice[ 55 ] =  784 ;

matrice[ 56 ] =  96 ;

matrice[ 57 ] =  896 ;

matrice[ 58 ] =  0 ;

matrice[ 59 ] =  0 ;
    }
    if (numero == 1)
    {
      matrice[ 30 ] =  0 ;

matrice[ 31 ] =  0 ;

matrice[ 32 ] =  128 ;

matrice[ 33 ] =  96 ;

matrice[ 34 ] =  16 ;

matrice[ 35 ] =  0 ;

matrice[ 36 ] =  8 ;

matrice[ 37 ] =  32 ;

matrice[ 38 ] =  20 ;

matrice[ 39 ] =  0 ;

matrice[ 40 ] =  32 ;

matrice[ 41 ] =  0 ;

matrice[ 42 ] =  96 ;

matrice[ 43 ] =  0 ;

matrice[ 44 ] =  160 ;

matrice[ 45 ] =  0 ;

matrice[ 46 ] =  128 ;

matrice[ 47 ] =  64 ;

matrice[ 48 ] =  1408 ;

matrice[ 49 ] =  512 ;

matrice[ 50 ] =  1280 ;

matrice[ 51 ] =  256 ;

matrice[ 52 ] =  512 ;

matrice[ 53 ] =  0 ;

matrice[ 54 ] =  0 ;

matrice[ 55 ] =  0 ;

matrice[ 56 ] =  0 ;

matrice[ 57 ] =  0 ;

matrice[ 58 ] =  0 ;

matrice[ 59 ] =  0 ;
    }
    if (numero == 2)
    {
      matrice[ 30 ] =  0 ;

matrice[ 31 ] =  0 ;

matrice[ 32 ] =  896 ;

matrice[ 33 ] =  96 ;

matrice[ 34 ] =  784 ;

matrice[ 35 ] =  0 ;

matrice[ 36 ] =  776 ;

matrice[ 37 ] =  512 ;

matrice[ 38 ] =  260 ;

matrice[ 39 ] =  1280 ;

matrice[ 40 ] =  512 ;

matrice[ 41 ] =  1408 ;

matrice[ 42 ] =  1088 ;

matrice[ 43 ] =  640 ;

matrice[ 44 ] =  162 ;

matrice[ 45 ] =  4352 ;

matrice[ 46 ] =  12416 ;

matrice[ 47 ] =  6240 ;

matrice[ 48 ] =  5248 ;

matrice[ 49 ] =  2080 ;

matrice[ 50 ] =  1024 ;

matrice[ 51 ] =  2128 ;

matrice[ 52 ] =  1568 ;

matrice[ 53 ] =  576 ;

matrice[ 54 ] =  1152 ;

matrice[ 55 ] =  784 ;

matrice[ 56 ] =  1120 ;

matrice[ 57 ] =  896 ;

matrice[ 58 ] =  1024 ;

matrice[ 59 ] =  0 ;
    }
  }


  void update_cadrant_HD( uint16_t numero, uint16_t matrice[])
  { /* Fonction permettant de mettre à jour le cadrant en haut à droite pour la V3 */
    if (numero == 0)
    {
      matrice[ 60 ] =  0 ;

matrice[ 61 ] =  0 ;

matrice[ 62 ] =  0 ;

matrice[ 63 ] =  1024 ;

matrice[ 64 ] =  768 ;

matrice[ 65 ] =  1152 ;

matrice[ 66 ] =  832 ;

matrice[ 67 ] =  1696 ;

matrice[ 68 ] =  2128 ;

matrice[ 69 ] =  1024 ;

matrice[ 70 ] =  2080 ;

matrice[ 71 ] =  5128 ;

matrice[ 72 ] =  6176 ;

matrice[ 73 ] =  12288 ;

matrice[ 74 ] =  2052 ;

matrice[ 75 ] =  4112 ;

matrice[ 76 ] =  5120 ;

matrice[ 77 ] =  3072 ;

matrice[ 78 ] =  1032 ;

matrice[ 79 ] =  512 ;

matrice[ 80 ] =  1024 ;

matrice[ 81 ] =  276 ;

matrice[ 82 ] =  544 ;

matrice[ 83 ] =  840 ;

matrice[ 84 ] =  128 ;

matrice[ 85 ] =  784 ;

matrice[ 86 ] =  96 ;

matrice[ 87 ] =  896 ;

matrice[ 88 ] =  0 ;

matrice[ 89 ] =  0 ;

    }

    if (numero == 1){
      matrice[ 60 ] =  0 ;

  matrice[ 61 ] =  0 ;

  matrice[ 62 ] =  0 ;

  matrice[ 63 ] =  0 ;

  matrice[ 64 ] =  0 ;

  matrice[ 65 ] =  0 ;

  matrice[ 66 ] =  0 ;

  matrice[ 67 ] =  1024 ;

  matrice[ 68 ] =  2048 ;

  matrice[ 69 ] =  1024 ;

  matrice[ 70 ] =  2560 ;

  matrice[ 71 ] =  1280 ;

  matrice[ 72 ] =  1024 ;

  matrice[ 73 ] =  640 ;

  matrice[ 74 ] =  128 ;

  matrice[ 75 ] =  256 ;

  matrice[ 76 ] =  128 ;

  matrice[ 77 ] =  64 ;

  matrice[ 78 ] =  136 ;

  matrice[ 79 ] =  32 ;

  matrice[ 80 ] =  0 ;

  matrice[ 81 ] =  80 ;

  matrice[ 82 ] =  32 ;

  matrice[ 83 ] =  64 ;

  matrice[ 84 ] =  128 ;

  matrice[ 85 ] =  256 ;

  matrice[ 86 ] =  0 ;

  matrice[ 87 ] =  0 ;

  matrice[ 88 ] =  0 ;

  matrice[ 89 ] =  0 ;
    }
    if (numero == 2)
    {
      matrice[ 60 ] =  0 ;

matrice[ 61 ] =  1024 ;

matrice[ 62 ] =  128 ;

matrice[ 63 ] =  1120 ;

matrice[ 64 ] =  16 ;

matrice[ 65 ] =  1152 ;

matrice[ 66 ] =  72 ;

matrice[ 67 ] =  1184 ;

matrice[ 68 ] =  2132 ;

matrice[ 69 ] =  256 ;

matrice[ 70 ] =  2080 ;

matrice[ 71 ] =  4488 ;

matrice[ 72 ] =  4160 ;

matrice[ 73 ] =  12928 ;

matrice[ 74 ] =  2182 ;

matrice[ 75 ] =  4864 ;

matrice[ 76 ] =  5760 ;

matrice[ 77 ] =  3072 ;

matrice[ 78 ] =  1288 ;

matrice[ 79 ] =  512 ;

matrice[ 80 ] =  1024 ;

matrice[ 81 ] =  20 ;

matrice[ 82 ] =  32 ;

matrice[ 83 ] =  72 ;

matrice[ 84 ] =  128 ;

matrice[ 85 ] =  784 ;

matrice[ 86 ] =  96 ;

matrice[ 87 ] =  896 ;

matrice[ 88 ] =  0 ;

matrice[ 89 ] =  0 ;
    }
    if (numero == 3)
    {
      matrice[ 60 ] =  0 ;

matrice[ 61 ] =  1024 ;

matrice[ 62 ] =  512 ;

matrice[ 63 ] =  1024 ;

matrice[ 64 ] =  512 ;

matrice[ 65 ] =  1024 ;

matrice[ 66 ] =  512 ;

matrice[ 67 ] =  1536 ;

matrice[ 68 ] =  2116 ;

matrice[ 69 ] =  1024 ;

matrice[ 70 ] =  2080 ;

matrice[ 71 ] =  5248 ;

matrice[ 72 ] =  6208 ;

matrice[ 73 ] =  12416 ;

matrice[ 74 ] =  18566 ;

matrice[ 75 ] =  4352 ;

matrice[ 76 ] =  12928 ;

matrice[ 77 ] =  5120 ;

matrice[ 78 ] =  1288 ;

matrice[ 79 ] =  2560 ;

matrice[ 80 ] =  1024 ;

matrice[ 81 ] =  2324 ;

matrice[ 82 ] =  1568 ;

matrice[ 83 ] =  840 ;

matrice[ 84 ] =  1152 ;

matrice[ 85 ] =  784 ;

matrice[ 86 ] =  1120 ;

matrice[ 87 ] =  896 ;

matrice[ 88 ] =  1024 ;

matrice[ 89 ] =  0 ;

    }

    if (numero == 4)
    {
      matrice[ 60 ] =  0 ;

matrice[ 61 ] =  0 ;

matrice[ 62 ] =  0 ;

matrice[ 63 ] =  1024 ;

matrice[ 64 ] =  768 ;

matrice[ 65 ] =  1152 ;

matrice[ 66 ] =  832 ;

matrice[ 67 ] =  160 ;

matrice[ 68 ] =  64 ;

matrice[ 69 ] =  0 ;

matrice[ 70 ] =  32 ;

matrice[ 71 ] =  256 ;

matrice[ 72 ] =  4096 ;

matrice[ 73 ] =  12928 ;

matrice[ 74 ] =  2176 ;

matrice[ 75 ] =  4864 ;

matrice[ 76 ] =  5760 ;

matrice[ 77 ] =  3072 ;

matrice[ 78 ] =  1280 ;

matrice[ 79 ] =  512 ;

matrice[ 80 ] =  1024 ;

matrice[ 81 ] =  256 ;

matrice[ 82 ] =  512 ;

matrice[ 83 ] =  768 ;

matrice[ 84 ] =  0 ;

matrice[ 85 ] =  768 ;

matrice[ 86 ] =  0 ;

matrice[ 87 ] =  768 ;

matrice[ 88 ] =  0 ;

matrice[ 89 ] =  0 ;
    }
    if (numero == 5)
    {
      matrice[ 60 ] =  1 ;

matrice[ 61 ] =  1024 ;

matrice[ 62 ] =  896 ;

matrice[ 63 ] =  1120 ;

matrice[ 64 ] =  768 ;

matrice[ 65 ] =  1152 ;

matrice[ 66 ] =  64 ;

matrice[ 67 ] =  1184 ;

matrice[ 68 ] =  2116 ;

matrice[ 69 ] =  256 ;

matrice[ 70 ] =  2080 ;

matrice[ 71 ] =  4480 ;

matrice[ 72 ] =  4160 ;

matrice[ 73 ] =  8832 ;

matrice[ 74 ] =  16518 ;

matrice[ 75 ] =  768 ;

matrice[ 76 ] =  1664 ;

matrice[ 77 ] =  3072 ;

matrice[ 78 ] =  5384 ;

matrice[ 79 ] =  2560 ;

matrice[ 80 ] =  1024 ;

matrice[ 81 ] =  2324 ;

matrice[ 82 ] =  1568 ;

matrice[ 83 ] =  840 ;

matrice[ 84 ] =  1152 ;

matrice[ 85 ] =  784 ;

matrice[ 86 ] =  1120 ;

matrice[ 87 ] =  896 ;

matrice[ 88 ] =  1024 ;

matrice[ 89 ] =  0 ;
    }
    if (numero == 6)
    {
      matrice[ 60 ] =  0 ;

matrice[ 61 ] =  1024 ;

matrice[ 62 ] =  896 ;

matrice[ 63 ] =  1120 ;

matrice[ 64 ] =  784 ;

matrice[ 65 ] =  1152 ;

matrice[ 66 ] =  72 ;

matrice[ 67 ] =  1184 ;

matrice[ 68 ] =  2132 ;

matrice[ 69 ] =  256 ;



matrice[ 70 ] =  2080 ;

matrice[ 71 ] =  4488 ;

matrice[ 72 ] =  4160 ;

matrice[ 73 ] =  8832 ;

matrice[ 74 ] =  134 ;

matrice[ 75 ] =  768 ;

matrice[ 76 ] =  1664 ;

matrice[ 77 ] =  3072 ;

matrice[ 78 ] =  1288 ;

matrice[ 79 ] =  512 ;

matrice[ 80 ] =  1024 ;

matrice[ 81 ] =  276 ;

matrice[ 82 ] =  544 ;

matrice[ 83 ] =  840 ;

matrice[ 84 ] =  128 ;

matrice[ 85 ] =  784 ;

matrice[ 86 ] =  96 ;

matrice[ 87 ] =  896 ;

matrice[ 88 ] =  0 ;

matrice[ 89 ] =  0 ;

    }
    if (numero == 7)
    {
      matrice[ 60 ] =  0 ;

matrice[ 61 ] =  0 ;

matrice[ 62 ] =  0 ;

matrice[ 63 ] =  1024 ;

matrice[ 64 ] =  512 ;

matrice[ 65 ] =  1024 ;

matrice[ 66 ] =  512 ;

matrice[ 67 ] =  1536 ;

matrice[ 68 ] =  2048 ;

matrice[ 69 ] =  1024 ;

matrice[ 70 ] =  2048 ;

matrice[ 71 ] =  5120 ;

matrice[ 72 ] =  7168 ;

matrice[ 73 ] =  5632 ;

matrice[ 74 ] =  2048 ;

matrice[ 75 ] =  768 ;

matrice[ 76 ] =  1664 ;

matrice[ 77 ] =  1024 ;

matrice[ 78 ] =  384 ;

matrice[ 79 ] =  512 ;

matrice[ 80 ] =  256 ;

matrice[ 81 ] =  320 ;

matrice[ 82 ] =  128 ;

matrice[ 83 ] =  320 ;

matrice[ 84 ] =  128 ;

matrice[ 85 ] =  256 ;

matrice[ 86 ] =  64 ;

matrice[ 87 ] =  384 ;

matrice[ 88 ] =  0 ;

matrice[ 89 ] =  0 ;
    }

    if (numero == 8)
    {
      matrice[ 60 ] =  0 ;

matrice[ 61 ] =  1024 ;

matrice[ 62 ] =  896 ;

matrice[ 63 ] =  1120 ;

matrice[ 64 ] =  784 ;

matrice[ 65 ] =  1152 ;

matrice[ 66 ] =  72 ;

matrice[ 67 ] =  1184 ;

matrice[ 68 ] =  2132 ;

matrice[ 69 ] =  256 ;

matrice[ 70 ] =  2080 ;

matrice[ 71 ] =  4488 ;

matrice[ 72 ] =  4160 ;

matrice[ 73 ] =  12928 ;

matrice[ 74 ] =  2182 ;

matrice[ 75 ] =  4864 ;

matrice[ 76 ] =  5760 ;

matrice[ 77 ] =  3072 ;

matrice[ 78 ] =  1288 ;

matrice[ 79 ] =  512 ;

matrice[ 80 ] =  1024 ;

matrice[ 81 ] =  276 ;

matrice[ 82 ] =  544 ;

matrice[ 83 ] =  840 ;

matrice[ 84 ] =  128 ;

matrice[ 85 ] =  784 ;

matrice[ 86 ] =  96 ;

matrice[ 87 ] =  896 ;

matrice[ 88 ] =  0 ;

matrice[ 89 ] =  0 ;
    }

    if (numero == 9)
    {
      matrice[ 60 ] =  0 ;

matrice[ 61 ] =  1024 ;

matrice[ 62 ] =  896 ;

matrice[ 63 ] =  1120 ;

matrice[ 64 ] =  768 ;

matrice[ 65 ] =  1152 ;

matrice[ 66 ] =  64 ;

matrice[ 67 ] =  1184 ;

matrice[ 68 ] =  2116 ;

matrice[ 69 ] =  256 ;

matrice[ 70 ] =  2080 ;

matrice[ 71 ] =  4480 ;

matrice[ 72 ] =  4160 ;

matrice[ 73 ] =  12928 ;

matrice[ 74 ] =  2182 ;

matrice[ 75 ] =  4864 ;

matrice[ 76 ] =  5760 ;

matrice[ 77 ] =  3072 ;

matrice[ 78 ] =  1288 ;

matrice[ 79 ] =  512 ;

matrice[ 80 ] =  1024 ;

matrice[ 81 ] =  276 ;

matrice[ 82 ] =  544 ;

matrice[ 83 ] =  840 ;

matrice[ 84 ] =  128 ;

matrice[ 85 ] =  784 ;

matrice[ 86 ] =  96 ;

matrice[ 87 ] =  896 ;

matrice[ 88 ] =  0 ;

matrice[ 89 ] =  0 ;

    }

  }

  void update_cadrant_BD( uint16_t numero, uint16_t matrice[])
  { /* Fonction permettant de mettre à jour le cadrant en bas à gauche pour la V3 */
    if (numero == 0)
    {
      matrice[ 90 ] =  0 ;

matrice[ 91 ] =  0 ;

matrice[ 92 ] =  0 ;

matrice[ 93 ] =  1024 ;

matrice[ 94 ] =  768 ;

matrice[ 95 ] =  1152 ;

matrice[ 96 ] =  832 ;

matrice[ 97 ] =  1696 ;

matrice[ 98 ] =  2128 ;

matrice[ 99 ] =  1024 ;

matrice[ 100 ] =  2080 ;

matrice[ 101 ] =  5128 ;

matrice[ 102 ] =  6176 ;

matrice[ 103 ] =  12288 ;

matrice[ 104 ] =  16416 ;

matrice[ 105 ] =  4112 ;

matrice[ 106 ] =  8192 ;

matrice[ 107 ] =  4192 ;

matrice[ 108 ] =  4224 ;

matrice[ 109 ] =  2080 ;

matrice[ 110 ] =  256 ;

matrice[ 111 ] =  2368 ;

matrice[ 112 ] =  1664 ;

matrice[ 113 ] =  768 ;

matrice[ 114 ] =  1024 ;

matrice[ 115 ] =  0 ;

matrice[ 116 ] =  0 ;

matrice[ 117 ] =  0 ;

matrice[ 118 ] =  0 ;

matrice[ 119 ] =  0 ;
    }
    if (numero == 1)
    {
      matrice[ 90 ] =  0 ;

matrice[ 91 ] =  0 ;

matrice[ 92 ] =  0 ;

matrice[ 93 ] =  0 ;

matrice[ 94 ] =  0 ;

matrice[ 95 ] =  128 ;

matrice[ 96 ] =  64 ;

matrice[ 97 ] =  160 ;

matrice[ 98 ] =  64 ;

matrice[ 99 ] =  256 ;

matrice[ 100 ] =  0 ;

matrice[ 101 ] =  384 ;

matrice[ 102 ] =  0 ;

matrice[ 103 ] =  640 ;

matrice[ 104 ] =  0 ;

matrice[ 105 ] =  768 ;

matrice[ 106 ] =  9728 ;

matrice[ 107 ] =  7168 ;

matrice[ 108 ] =  5120 ;

matrice[ 109 ] =  2048 ;

matrice[ 110 ] =  0 ;

matrice[ 111 ] =  2048 ;

matrice[ 112 ] =  1024 ;

matrice[ 113 ] =  0 ;

matrice[ 114 ] =  0 ;

matrice[ 115 ] =  0 ;

matrice[ 116 ] =  0 ;

matrice[ 117 ] =  0 ;

matrice[ 118 ] =  0 ;

matrice[ 119 ] =  0 ;
    }
    if (numero == 2)
    {
      matrice[ 90 ] =  0 ;

matrice[ 91 ] =  1024 ;

matrice[ 92 ] =  896 ;

matrice[ 93 ] =  1120 ;

matrice[ 94 ] =  784 ;

matrice[ 95 ] =  1152 ;

matrice[ 96 ] =  584 ;

matrice[ 97 ] =  1568 ;

matrice[ 98 ] =  2068 ;

matrice[ 99 ] =  1024 ;

matrice[ 100 ] =  2560 ;

matrice[ 101 ] =  5384 ;

matrice[ 102 ] =  3072 ;

matrice[ 103 ] =  1664 ;

matrice[ 104 ] =  16516 ;

matrice[ 105 ] =  768 ;

matrice[ 106 ] =  8832 ;

matrice[ 107 ] =  4160 ;

matrice[ 108 ] =  4480 ;

matrice[ 109 ] =  2080 ;

matrice[ 110 ] =  256 ;

matrice[ 111 ] =  2112 ;

matrice[ 112 ] =  1184 ;

matrice[ 113 ] =  832 ;

matrice[ 114 ] =  1152 ;

matrice[ 115 ] =  768 ;

matrice[ 116 ] =  1024 ;

matrice[ 117 ] =  0 ;

matrice[ 118 ] =  0 ;

matrice[ 119 ] =  0 ;
    }
    if (numero == 3)
    {
      matrice[ 90 ] =  1 ;

matrice[ 91 ] =  0 ;

matrice[ 92 ] =  0 ;

matrice[ 93 ] =  1024 ;

matrice[ 94 ] =  768 ;

matrice[ 95 ] =  1152 ;

matrice[ 96 ] =  832 ;

matrice[ 97 ] =  1696 ;

matrice[ 98 ] =  80 ;

matrice[ 99 ] =  0 ;

matrice[ 100 ] =  2080 ;

matrice[ 101 ] =  5128 ;

matrice[ 102 ] =  7200 ;

matrice[ 103 ] =  13824 ;

matrice[ 104 ] =  16388 ;

matrice[ 105 ] =  4880 ;

matrice[ 106 ] =  8832 ;

matrice[ 107 ] =  4096 ;

matrice[ 108 ] =  4488 ;

matrice[ 109 ] =  2048 ;

matrice[ 110 ] =  256 ;

matrice[ 111 ] =  2052 ;

matrice[ 112 ] =  1024 ;

matrice[ 113 ] =  8 ;

matrice[ 114 ] =  1024 ;

matrice[ 115 ] =  0 ;

matrice[ 116 ] =  1024 ;

matrice[ 117 ] =  0 ;

matrice[ 118 ] =  1024 ;

matrice[ 119 ] =  0 ;
    }
    if (numero == 4)
    {
      matrice[ 90 ] =  0 ;

matrice[ 91 ] =  1024 ;

matrice[ 92 ] =  512 ;



matrice[ 93 ] =  1024 ;

matrice[ 94 ] =  528 ;

matrice[ 95 ] =  1024 ;

matrice[ 96 ] =  520 ;

matrice[ 97 ] =  1536 ;

matrice[ 98 ] =  2064 ;

matrice[ 99 ] =  1024 ;

matrice[ 100 ] =  2560 ;

matrice[ 101 ] =  5384 ;

matrice[ 102 ] =  7200 ;

matrice[ 103 ] =  13952 ;

matrice[ 104 ] =  16416 ;

matrice[ 105 ] =  4880 ;

matrice[ 106 ] =  8704 ;

matrice[ 107 ] =  96 ;

matrice[ 108 ] =  128 ;

matrice[ 109 ] =  32 ;

matrice[ 110 ] =  256 ;

matrice[ 111 ] =  64 ;

matrice[ 112 ] =  128 ;

matrice[ 113 ] =  0 ;

matrice[ 114 ] =  0 ;

matrice[ 115 ] =  0 ;

matrice[ 116 ] =  0 ;

matrice[ 117 ] =  0 ;

matrice[ 118 ] =  0 ;

matrice[ 119 ] =  0 ;
    }
    if (numero == 5)
    {
      matrice[ 90 ] =  1 ;

matrice[ 91 ] =  1024 ;

matrice[ 92 ] =  896 ;

matrice[ 93 ] =  1120 ;

matrice[ 94 ] =  784 ;

matrice[ 95 ] =  128 ;

matrice[ 96 ] =  72 ;

matrice[ 97 ] =  32 ;

matrice[ 98 ] =  2068 ;

matrice[ 99 ] =  1024 ;

matrice[ 100 ] =  2560 ;

matrice[ 101 ] =  5384 ;

matrice[ 102 ] =  7168 ;

matrice[ 103 ] =  13952 ;

matrice[ 104 ] =  16518 ;

matrice[ 105 ] =  4880 ;

matrice[ 106 ] =  8832 ;

matrice[ 107 ] =  4192 ;

matrice[ 108 ] =  4488 ;

matrice[ 109 ] =  2080 ;

matrice[ 110 ] =  256 ;

matrice[ 111 ] =  2132 ;

matrice[ 112 ] =  1184 ;

matrice[ 113 ] =  72 ;

matrice[ 114 ] =  1152 ;

matrice[ 115 ] =  16 ;

matrice[ 116 ] =  1120 ;

matrice[ 117 ] =  128 ;

matrice[ 118 ] =  1024 ;

matrice[ 119 ] =  0 ;
    }
    if (numero == 6)
    {
      matrice[ 90 ] =  0 ;

matrice[ 91 ] =  1024 ;

matrice[ 92 ] =  896 ;

matrice[ 93 ] =  1120 ;

matrice[ 94 ] =  784 ;

matrice[ 95 ] =  128 ;

matrice[ 96 ] =  72 ;

matrice[ 97 ] =  32 ;

matrice[ 98 ] =  2068 ;

matrice[ 99 ] =  1024 ;

matrice[ 100 ] =  2560 ;

matrice[ 101 ] =  5384 ;

matrice[ 102 ] =  7168 ;

matrice[ 103 ] =  13952 ;

matrice[ 104 ] =  16516 ;

matrice[ 105 ] =  4880 ;

matrice[ 106 ] =  8832 ;

matrice[ 107 ] =  4192 ;

matrice[ 108 ] =  4488 ;

matrice[ 109 ] =  2080 ;

matrice[ 110 ] =  256 ;

matrice[ 111 ] =  2128 ;

matrice[ 112 ] =  1184 ;

matrice[ 113 ] =  832 ;

matrice[ 114 ] =  1152 ;

matrice[ 115 ] =  768 ;

matrice[ 116 ] =  1024 ;

matrice[ 117 ] =  0 ;

matrice[ 118 ] =  0 ;

matrice[ 119 ] =  0 ;
    }
    if (numero == 7)
    {
      matrice[ 90 ] =  0 ;

matrice[ 91 ] =  0 ;

matrice[ 92 ] =  896 ;

matrice[ 93 ] =  96 ;

matrice[ 94 ] =  784 ;

matrice[ 95 ] =  128 ;

matrice[ 96 ] =  840 ;

matrice[ 97 ] =  672 ;

matrice[ 98 ] =  336 ;

matrice[ 99 ] =  1280 ;

matrice[ 100 ] =  544 ;

matrice[ 101 ] =  1288 ;

matrice[ 102 ] =  3104 ;

matrice[ 103 ] =  5632 ;

matrice[ 104 ] =  2048 ;

matrice[ 105 ] =  4624 ;

matrice[ 106 ] =  13312 ;

matrice[ 107 ] =  6144 ;

matrice[ 108 ] =  4096 ;

matrice[ 109 ] =  0 ;

matrice[ 110 ] =  0 ;

matrice[ 111 ] =  0 ;

matrice[ 112 ] =  0 ;

matrice[ 113 ] =  0 ;

matrice[ 114 ] =  0 ;

matrice[ 115 ] =  0 ;

matrice[ 116 ] =  0 ;

matrice[ 117 ] =  0 ;

matrice[ 118 ] =  0 ;

matrice[ 119 ] =  0 ;
    }
    if (numero == 8)
    {
      matrice[ 90 ] =  0 ;

matrice[ 91 ] =  1024 ;

matrice[ 92 ] =  896 ;

matrice[ 93 ] =  1120 ;

matrice[ 94 ] =  784 ;

matrice[ 95 ] =  1152 ;

matrice[ 96 ] =  584 ;

matrice[ 97 ] =  1568 ;

matrice[ 98 ] =  2068 ;

matrice[ 99 ] =  1024 ;

matrice[ 100 ] =  2560 ;

matrice[ 101 ] =  5384 ;

matrice[ 102 ] =  7168 ;

matrice[ 103 ] =  13952 ;

matrice[ 104 ] =  16516 ;

matrice[ 105 ] =  4880 ;

matrice[ 106 ] =  8832 ;

matrice[ 107 ] =  4192 ;

matrice[ 108 ] =  4488 ;

matrice[ 109 ] =  2080 ;

matrice[ 110 ] =  256 ;

matrice[ 111 ] =  2128 ;

matrice[ 112 ] =  1184 ;

matrice[ 113 ] =  832 ;

matrice[ 114 ] =  1152 ;

matrice[ 115 ] =  768 ;

matrice[ 116 ] =  1024 ;

matrice[ 117 ] =  0 ;

matrice[ 118 ] =  0 ;

matrice[ 119 ] =  0 ;
    }
    if (numero == 9)
    {
      matrice[ 90 ] =  0 ;
      matrice[ 91 ] =  1024 ;
      matrice[ 92 ] =  896 ;
      matrice[ 93 ] =  1120 ;
      matrice[ 94 ] =  784 ;
      matrice[ 95 ] =  1152 ;
      matrice[ 96 ] =  584 ;
      matrice[ 97 ] =  1568 ;
      matrice[ 98 ] =  2068 ;
      matrice[ 99 ] =  1024 ;
      matrice[ 100 ] =  2560 ;
      matrice[ 101 ] =  5384 ;
      matrice[ 102 ] =  7168 ;
matrice[ 103 ] =  13952 ;

matrice[ 104 ] =  16516 ;

matrice[ 105 ] =  4880 ;

matrice[ 106 ] =  8832 ;

matrice[ 107 ] =  4192 ;

matrice[ 108 ] =  4488 ;

matrice[ 109 ] =  2080 ;

matrice[ 110 ] =  256 ;

matrice[ 111 ] =  2128 ;

matrice[ 112 ] =  1184 ;

matrice[ 113 ] =  64 ;

matrice[ 114 ] =  1152 ;

matrice[ 115 ] =  0 ;

matrice[ 116 ] =  1024 ;

matrice[ 117 ] =  0 ;

matrice[ 118 ] =  0 ;

matrice[ 119 ] =  0 ;
    }
  }
  int main(void)
  {
    uint16_t mode = 1;
    // Initialisation des compteurs
    compteur_usart = 0;
    compteur_secondes = 0;
    temps = 0;
    changement_mode = 'e';

    // Initialisation du temps souhaité
    uint16_t secondes = 0;
    uint16_t minutes = 59;
    uint16_t heures = 15;
    uint16_t heures_aiguille = 0;

    uint16_t temp_secondes = 0;
    uint16_t temp_minutes = 0;
    uint16_t temp_heures = 0;

    uint16_t heures_dizaine = 1;
    uint16_t heures_unite = 5;
    uint16_t minutes_dizaine = 5;
    uint16_t minutes_unite = 9;
    uint16_t secondes_dizaine = 0;
    uint16_t secondes_unite = 0;

    uint16_t chemin = 0;
    // Verification de la validité des temps
    if (secondes >= 60)
    {
      secondes -= 60;
    }
    if (minutes >= 60)
    {
      minutes -= 60;
    }
    if (heures >= 24)
    {
      heures -= 24;
    }
    if (heures >= 12)
    {
      heures_aiguille = heures - 12;
    }
    else
    {
      heures_aiguille = heures;
    } // POUR AIGUILLE

    // Initialisation des communications et interuptions
    SPI_MasterInit();
    uint16_t pas = 3;
    set_interrupt();
    init_secondes();
    init_temps();
    init_debug();
    USART_Init(MYUBRR);

    uint8_t value1 = 0;
    uint8_t value2 = 0;

    //---------------INITIALISATION VARIABLES V2-------------//
    uint16_t  matrice_V2[60] = {0, 0, 0, 0, 0,
                               320, 0, 0, 0, 0,
                               0, 0, 0, 0, 0,
                               0, 0, 0, 0, 0,
                               0, 0, 4064, 256, 4064,
                               0, 896, 1344, 832, 0,
                               4064, 0, 4064, 0, 896,
                               1088, 896, 0, 0, 0,
                               0, 0, 0, 0, 0,
                               0, 0, 0, 0, 0,
                               0, 0, 0, 0, 0,
                               320, 0, 0, 0, 0};

    // Creation d'un pointeur pour parcourir la matrice
    uint16_t *pt_matrice_parcours;
    pt_matrice_parcours = matrice_V2;

    //---------------INITIALISATION VARIABLES V3-------------//

      uint16_t matrice_V3[120] = {0, 0, 0, 0, 0,
                                0, 0, 0, 0, 0,
                                0, 0, 0, 0, 0,
                                0, 0, 0, 0, 0,
                                0, 0, 0, 0, 0,
                                0, 0, 0, 0, 0,
                                0, 0, 128, 96, 16,
                                0, 8, 32, 20, 0,
                                32, 0, 96, 0, 160,
                                0, 128, 64, 1408, 512,
                                1280, 256, 512, 0, 0,
                                0, 0, 0, 0, 0,
                                0, 0, 0, 0, 0,
                                0, 0, 0, 0, 0,
                                0, 0, 0, 0, 0,
                                0, 0, 0, 0, 0,
                                0, 0, 0, 0, 0,
                                0, 0, 0, 0, 0,
                                0, 0, 0, 0, 0,
                                0, 0, 0, 0, 0,
                                0, 0, 0, 0, 0,
                                0, 0, 0, 0, 0,
                                0, 0, 0, 0, 0,
                                0, 0, 0, 0, 0};

    // Initialisation variables compteurs debug
    uint16_t tfinwhile = 0;
    uint16_t twhile = 0;
    uint16_t compteur_while = 0;
    uint16_t tdebutwhile = compteur_debug;
    for (;;)
    { /* Début de la boucle infinie */
      if (compteur_while++ >= 10000)
      {/* Compteur permettant de savoir le temps pour effectuer la boucle for */
        tfinwhile = compteur_debug;
        twhile = tfinwhile - tdebutwhile;
        tdebutwhile = compteur_debug;
        compteur_while = 0;
      }

      if (detection_hall >= 1)
      { /*Detection de l'effet hall et calcul du temps du tour et du pas /60 */
        detection_hall = 0;
        if (mode == 1)
        {
          pas = temps / 60;
        }
        if (mode == 2)
        {
          pas = temps / 60;
          pt_matrice_parcours = matrice_V2;

        }
        if (mode == 3)
        {
          pas = temps / 120;
          pt_matrice_parcours = matrice_V3;

        }
        temps = 0;
        chemin = 0;

      }

      if (changement_mode == 'd')
      { /* Permet de demander le résultat du compteur debug */
        char string[64];
        itoa(twhile, string, 10); //convert integer to string, radix=1
        sprintf(string + strlen(string), "\n");

        uart_send(string);
        changement_mode = 'e';
      }

      if (changement_mode == 'a')
      {/* Permet de passer au mode V1 */
        mode = 1;
        temps = 0;
        init_temps_changement_mode(200);
        compteur_secondes = 0;
        secondes = 0;
        secondes_unite = 0;
        secondes_dizaine = 0;
        if (heures > 12)
        {
          heures_aiguille = heures - 12;
        }
        changement_mode = 'e';
      }

      if (changement_mode == 'b')
      {/* Permet de passer au mode V2 */
        mode = 2;
        temps = 0;
        init_temps_changement_mode(1000);
        compteur_secondes = 0;
        secondes = 0;
        secondes_unite = 0;
        secondes_dizaine = 0;
        pt_matrice_parcours = matrice_V2;
        changement_mode = 'e';
      }

      if (changement_mode == 'c')
      {/* Permet de passer au mode V3 */
        mode = 3;
        temps = 0;
        init_temps_changement_mode(1000);
        compteur_secondes = 0;
        secondes = 0;
        secondes_unite = 0;
        secondes_dizaine = 0;
        pt_matrice_parcours = matrice_V3;
        changement_mode = 'e';
      }

      // Mise à jour de l'heure
      if ((secondes + 1) * 1625 == compteur_secondes)
      {
        secondes++;
        secondes_unite++;
        if (secondes_unite >= 10)
        {
          secondes_unite = 0;
          secondes_dizaine++;
        }
        if (secondes >= 60)
        {
          compteur_secondes = 0;
          secondes_unite = 0;
          secondes_dizaine = 0;
          secondes = 0;
          minutes++;
          minutes_unite++;
          if (minutes_unite >= 10)
          {
            minutes_unite = 0;
            minutes_dizaine++;
          }
          if (minutes >= 60)
          {
            minutes = 0;
            minutes_unite = 0;
            minutes_dizaine = 0;
            heures++;
            heures_unite++;
            if (heures_unite >= 10)
            {
              heures_unite = 0;
              heures_dizaine++;
            }
            if (heures >= 24)
            {
              heures = 0;
              heures_unite = 0;
              heures_dizaine = 0;
            }
            if (heures >= 12)
            {
              heures_aiguille = heures - 12;
            }
            else
            {
              heures_aiguille = heures;
            }
          }
        }
      }

      // V1 : Appel fonction aiguille
      if (mode == 1)
      {
        if (secondes >= 30)
        {
          temp_secondes = secondes + 30 - 60;
        }
        else
        {
          temp_secondes = secondes + 30;
        }

        if (minutes >= 30)
        {
          temp_minutes = minutes + 30 - 60;
        }
        else
        {
          temp_minutes = minutes + 30;
        }

        if (heures_aiguille >= 6)
        {
          temp_heures = heures_aiguille + 6 - 12;
        }
        else
        {
          temp_heures = heures_aiguille + 6;
        }

        if (pas * temp_secondes <= temps && pas * temp_secondes + 6 >= temps)
        {
          Control_LEDS(0, 252);
        }
        else
        {
          if (pas * temp_minutes <= temps && pas * temp_minutes + 6 >= temps)
          {
            Control_LEDS(0, 252);
          }
          else
          {
            if (pas * 5 * temp_heures <= temps && pas * 5 * temp_heures + 6 >= temps)
            {
              Control_LEDS(0, 7);
            }
            else
            {
              Control_LEDS(0, 0);
            }
          }
        }
      }

      // V2 : Appel fonction chiffres tordus
      if (mode == 2)
      {
      update_chiffre(pos_unite_sec, secondes_unite, matrice_V2);
      update_chiffre(pos_dizaine_sec, secondes_dizaine, matrice_V2);

      update_chiffre(pos_unite_min, minutes_unite, matrice_V2);
      update_chiffre(pos_dizaine_min, minutes_dizaine, matrice_V2);

      update_chiffre(pos_unite_heure, heures_unite, matrice_V2);
      update_chiffre(pos_dizaine_heure, heures_dizaine, matrice_V2);

        if (pas * chemin <= temps && pas * chemin + 1 >= temps)
        {
          pt_matrice_parcours++;
          chemin++;
          if (chemin == 60)
          {
            chemin = 0;
            pt_matrice_parcours = matrice_V2;
          }
        }
        value1 = *pt_matrice_parcours;
        value2 = *pt_matrice_parcours >> 8;
        Control_LEDS(value2, value1);
      }

      // V3 : Appel fonction chiffres droits
      if (mode == 3)
      {
        // Mettre à jour les différents cadrants
        update_cadrant_HG(heures_dizaine, matrice_V3);
        update_cadrant_HD(heures_unite, matrice_V3);
        update_cadrant_BG(minutes_dizaine, matrice_V3);
        update_cadrant_BD(minutes_unite, matrice_V3);
        if (pas * chemin <= temps && pas * chemin + 1 >= temps)
        {
          pt_matrice_parcours++;
          chemin++;
          if (chemin == 120)
          {
            chemin = 0;
            pt_matrice_parcours = matrice_V3;
          }
        }
        value1 = *pt_matrice_parcours;
        value2 = *pt_matrice_parcours >> 8;

        if(chemin >=60)
        {
          if (secondes*2+60 >= chemin)
          {
            value2|=128;
          }
        }
        else
        {
          if(secondes >= 30)
          {
            if((secondes-30)*2 >= chemin)
            {
              value2|=128;
            }
          }
        }
        Control_LEDS(value2, value1);
      }
    }
    }
