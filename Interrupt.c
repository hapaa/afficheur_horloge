#include <avr/io.h>
#include <avr/interrupt.h>

ISR(INT0_vect){
    //Faire le bidule
    compteur++;
}

int set_interrupt(){

    //etre sûr que TWEN de TWCR est à 0

    DDRD &= ~(1<<DDD0); //Met PORTD0 en Input

    PORTD |= (1<<PORTD0); //Mettre le pull-up resistor
    
    EICRA |= (1<<ISCO1)|(1<<ISCO0); //Active le mode détection de fronts montants du INT0

    EIMSK |= (1<<INT0); //Met le bit correspondant à l'interruption 0 sur 1 dans le EIMSK

    SREG |= (1<<I); //Met le bit à 1 du global interrupt du status register, permet d'activer les interruptions

}

int main(){

    int compteur_precedent(0);
    int compteur(0);
    set_interrupt();

    while(True){
        if(compteur_precedent!=compteur)
        {
            USART_Transmit(compteur);
            compteur_precedent=compteur;
        }
    }
    

}