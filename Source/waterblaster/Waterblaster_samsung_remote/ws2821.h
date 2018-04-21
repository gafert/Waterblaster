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

/**
 * Sets the RGB value of a specific led
 * @param idx The index of the led to change
 * @param r Red value (0-255) -1 to leave unchanged
 * @param g Green value (0-255) -1 to leave unchanged
 * @param b Blue value (0-255) -1 to leave unchanged
 */
void set_RGB_single(uint16_t idx, uint8_t r, uint8_t g, uint8_t b);
void set_RGB_all(uint8_t r, uint8_t g, uint8_t b);

/**
 * This function disables interrupts while the information for the leds is sent
 * Needed if there are long interrupts which could interfere with sending
 * @param status Enable or disable interrupts while sending
 */
void precise_led_timing(bool status);

/**
 * Called every cycle, checks animation
 */
void led_animation_loop();

/**
 * Sets a new color of the animation
 * @param colors_ Color to set
 */
void set_color(color colors_);
void reset_color();

#endif //EMBEDDED_SYSTEMS_WS2821_H
