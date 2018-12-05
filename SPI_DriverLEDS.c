#include <avr/io.h>
#include <util/delay.h>


void SPI_MasterInit(void)
{
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


void Test_LED(void)
{
    uint8_t value = 0;
    SPI_MasterTransmit(value);
    value |= (1<<5)|(1<<6);
    SPI_MasterTransmit(value);

    PORTE |= (1<<PORTE5);

    PORTE &= ~(1<<PORTE5);

    PORTE &= ~(1<<PORTE4);

    PORTE |= (1<<PORTE4);
}


int main()
{
    SPI_MasterInit();
//    while (1)
//    {
//        Test_LED();
//    }
    Test_LED();

    return 0;
}
