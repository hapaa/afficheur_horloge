#include <avr/io.h>
#include <util/delay.h>

//Master : on envoie des données
//Slave : on va t'interroger sur tes données

//On est maitre (processeur), notre puce est esclave
//Mais il arrive que le processeur soit esclave

void SPI_MasterInit(void)
{
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
while(!(SPSR & (1<<SPIF)))
;
}