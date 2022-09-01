#include "Digital_in.h"
#include <avr/io.h>

Digital_in::Digital_in(uint8_t pin_number){
    pinMask = (1<<pin_number);
}

void Digital_in::init(){
    DDRD &= ~pinMask;
    PORTD |= pinMask;
}

bool Digital_in::is_hi(){
    return PIND & pinMask;
}

bool Digital_in::is_lo(){
    return !(PIND & pinMask);
}
