//
// Created by Michael Gafert on 26.10.17.
//

#ifndef EMBEDDED_SYSTEMS_WS2821_H
#define EMBEDDED_SYSTEMS_WS2821_H

//
// Example usage
//

/*
#define F_CPU 16000000UL
#include "ws2821.h"
#define NUM_LEDS 16
uint8_t *leds;

int main() {
    init_leds(leds, NUM_LEDS);
    set_RGB_single(3, 255, 0, 0);
    update_leds();
}
 */

// Defines where the leds are connected
#define WS2821_PORT B
#define WS2821_PIN 0

typedef struct tColor {
    uint8_t r,g,b;
} color;

void init_leds(uint8_t *leds, uint8_t lenght);
void update_leds();
void set_RGB_single(uint16_t idx, uint8_t r, uint8_t g, uint8_t b);
void set_RGB_all(uint8_t r, uint8_t g, uint8_t b);
void bounce_color(uint8_t *);
void precise_led_timing(bool status);
void init_nozzle_animation();
void set_color(color colors_);
void reset_color();

#endif //EMBEDDED_SYSTEMS_WS2821_H
