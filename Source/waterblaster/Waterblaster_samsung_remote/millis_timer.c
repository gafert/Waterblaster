//
// Created by Michael Gafert on 03.12.17.
//

#include <avr/io.h>
#include <avr/interrupt.h>
#include "millis_timer.h"

volatile unsigned long current_millis = 0;

ISR(TIMER2_COMPA_vect) {
    current_millis++;
}

unsigned long millis(){
    return current_millis;
}

void init_millis_timer(){
    TCCR2A = (1 << WGM21); // CTC MODE
    TCCR2B |= (1 << CS22);   //  with 64 prescaler
    TIMSK2 |= (1 << OCIE2A);   // Enable Interrupts
    OCR2A = 250; // Overflows every 1ms
    sei();
}
