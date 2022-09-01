#include "encoder.h"
#include <avr/io.h>

Encoder::Encoder(){
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