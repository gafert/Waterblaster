//
// Created by Michael Gafert on 18.11.17.
//

#include <avr/io.h>
#include "internal_led.h"

#ifndef CONCAT_HELPER
#define CONCAT_HELPER(a, b) a ## b
#endif
#ifndef CONCAT
#define CONCAT(a, b) CONCAT_HELPER(a,b)
#endif

#define INTERNAL_LED_DDRx CONCAT(DDR, INTERNAL_LED_PORT)
#define INTERNAL_LED_PORTx CONCAT(PORT, INTERNAL_LED_PORT)

void init_internal_led() {
    INTERNAL_LED_DDRx |= _BV(INTERNAL_LED_PIN);
}

void internal_led_on() {
    INTERNAL_LED_PORTx |= _BV(INTERNAL_LED_PIN);
}

void internal_led_off() {
    INTERNAL_LED_PORTx &= ~_BV(INTERNAL_LED_PIN);
}

void internal_led_toggle() {
    INTERNAL_LED_PORTx ^= _BV(INTERNAL_LED_PIN);
}
