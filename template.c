uint16_t  matrice[30] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0};

uint16_t un[3] = {0, 0, 992};
uint16_t deux[3] = {736, 672, 928};
uint16_t trois[3] = {672, 672, 992};
uint16_t quatre[3] = {896, 128, 992};
// cinq = deux inverse
uint16_t six[3] = {992, 160, 224};
uint16_t sept[3] = {512, 512, 992};
uint16_t huit[3] = {992, 672, 992};
// neuf = six inverse
uint16_t zero[3] = {992, 544, 992};

uint16_t points[3] = {0, 320, 0};


// Creation d'un pointeur pour parcourir la matrice
uint16_t* pt_matrice_parcours;

pt_matrice_parcours = matrice;


// Fonction permettant de mettre à jour un chiffre
void maj_chiffre(int* position_debut, uint16_t chiffre)
{
  position_debut* = chiffre[0];
  position_debut ++;
  position_debut* = chiffre[1];
  position_debut ++;
  position_debut* = chiffre[2];
}

// Parcours la matrice à chaque pas de temps

// Afficher leds correspondantes

int i = 0;
while(1)
{
  if(pas*i <= temps  && pas*i+1 >=temps)
  {
    pt_matrice_parcours++;
    i++;
    if (i==61)
    {i=0;}
  }
  Control_LEDS(pt_matrice_parcours*);
}

void Control_LEDS(uint16_t value)
{


    PORTE |= (1<<PORTE4);

    uint8_t value1 = value;
    uint8_t value2 = value>>8;
    SPI_MasterTransmit(value1);
    SPI_MasterTransmit(value2);

    PORTE |= (1<<PORTE5);

    PORTE &= ~(1<<PORTE5);

    PORTE &= ~(1<<PORTE4);
}
