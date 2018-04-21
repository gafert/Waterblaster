//
// Created by Michael Gafert on 19.11.17.
//

#include <avr/io.h>
#include "pump.h"

#ifndef CONCAT_HELPER
#define CONCAT_HELPER(a, b) a ## b
#endif
#ifndef CONCAT
#define CONCAT(a, b) CONCAT_HELPER(a,b)
#endif

#define PUMP_DDRx CONCAT(DDR, PUMP_PORT)
#define PUMP_PORTx CONCAT(PORT, PUMP_PORT)

void init_pump(){
    PUMP_DDRx |= _BV(PUMP_PIN);
}

void pump_on(){
    PUMP_PORTx |= _BV(PUMP_PIN);
}

void pump_off(){
    PUMP_PORTx &= ~_BV(PUMP_PIN);
}