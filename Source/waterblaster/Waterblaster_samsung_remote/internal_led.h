//
// Created by Michael Gafert on 18.11.17.
//

#ifndef EMBEDDED_SYSTEMS_INTERNAL_LED_H
#define EMBEDDED_SYSTEMS_INTERNAL_LED_H

#define INTERNAL_LED_PORT B
#define INTERNAL_LED_PIN 5

void init_internal_led();
void internal_led_on();
void internal_led_off();
void internal_led_toggle();

#endif //EMBEDDED_SYSTEMS_INTERNAL_LED_H
