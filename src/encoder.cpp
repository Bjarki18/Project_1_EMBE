#include "encoder.h"
#include <avr/io.h>

Encoder::Encoder(){
    EIMSK |= (1<<INT1); // external interrupt mask register for INT1 interrupt vector
  	EIMSK |= (1<<INT0);
	EICRA |= (1<<ISC10) | (1<<ISC11); // falling + rising edge interrupt requests 
  	EICRA |= (1<<ISC00) | (1<<ISC01);
    asm("sei"); // enable interrupts
}

void Encoder::increment(){
    pos += 1;
}

void Encoder::decrement(){
    pos -= 1;
}

int Encoder::position(){
    return pos;
}