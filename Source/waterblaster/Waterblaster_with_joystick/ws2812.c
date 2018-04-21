//
// Created by Michael Gafert on 26.10.17.
//

#include <avr/io.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include "ws2812.h"
#include "internal_led.h"
#include "millis_timer.h"

#ifndef CONCAT_HELPER
#define CONCAT_HELPER(a, b) a ## b
#endif
#ifndef CONCAT
#define CONCAT(a, b) CONCAT_HELPER(a,b)
#endif

#define WS2821_PORTx CONCAT(PORT, WS2812_PORT)
#define WS2821_DDRx CONCAT(DDR, WS2812_PORT)
#define NUM_BITS (8) // Constant value: bits per byte

uint8_t *led_array = NULL;
uint8_t num_leds = 0;
uint8_t num_bytes = 0;

bool led_timing_precise = true;

#define MS_DELAY_SLOW 200
#define MS_DELAY_FAST 100
int animation_speed = MS_DELAY_SLOW; // How long to the next led

color colors = {0, 0, 255}; // Color of the leds
int8_t led_index = 0;       // Led index cycles trough

bool set_triggered = false;
bool reset_triggered = false;

void init_leds(uint8_t *leds, uint8_t lenght) {
    led_array = leds;
    num_leds = lenght;
    num_bytes = lenght * 3; // RGB for each led

    WS2821_DDRx |= _BV(WS2812_PIN);
    WS2821_PORTx &= ~_BV(WS2812_PIN);
    if ((led_array = (uint8_t *) malloc(num_bytes))) {
        memset(led_array, 0, num_bytes);
    }
    set_RGB_all(0, 0, 0);
    update_leds();
}

void set_color(color colors_) {
    // Only trigger once
    if (!set_triggered) {
        set_triggered = true; // reset variable
        reset_triggered = false; // If set unset reset

        colors = colors_; // Set new color
        animation_speed = MS_DELAY_FAST; // Set new speed
        led_index = 0; // Reset led index

        // Refresh leds with nothing
        set_RGB_all(0, 0, 0);
        update_leds();
    }
}

void reset_color() {
    // Main reset of color is only done once the animation has finished
    if (!reset_triggered) {
        reset_triggered = true;
        set_triggered = false;
    }
}


void led_animation_loop() {
    static unsigned long previousMillis = 0;
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= animation_speed) {
        internal_led_toggle();
        previousMillis = currentMillis;
        if (led_index == -1) {
            set_RGB_all(0, 0, 0);
            update_leds();
            if (reset_triggered) {
                animation_speed = MS_DELAY_SLOW;
                led_index = -1;
                set_RGB_all(0, 0, 0);
                update_leds();
                colors.r = 0;
                colors.g = 0;
                colors.b = 255;
            }
        }
        set_RGB_all(0, 0, 0);
        set_RGB_single(led_index, colors.r, colors.g, colors.b);
        update_leds();
        led_index++;
        if (led_index > 4) {
            led_index = -1;
        }
    }
}

void set_RGB_single(uint16_t idx, uint8_t r, uint8_t g, uint8_t b) {
    if (idx < num_leds) {
        uint8_t *p = &led_array[idx * 3];
        *p++ = g;
        *p++ = r;
        *p = b;
    }
}

void set_RGB_all(uint8_t r, uint8_t g, uint8_t b) {
    for (uint16_t i = 0; i < num_leds; ++i) {
        set_RGB_single(i, r, g, b);
    }
}


void precise_led_timing(bool status) {
    led_timing_precise = status;
}

void update_leds(void) {
    if (!led_array) return;

    if (led_timing_precise) cli(); // Disable interrupts so that timing is as precise as possible
    volatile uint8_t
            *p = led_array,          // Copy the start address of our data array
            val = *p++,              // Get the current byte value & point to next byte
            high = WS2821_PORTx | _BV(WS2812_PIN), // Bitmask for sending HIGH to pin
            low = WS2821_PORTx & ~_BV(WS2812_PIN), // Bitmask for sending LOW to pin
            tmp = low,               // Swap variable to adjust duty cycle
            nbits = NUM_BITS;        // Bit counter for inner loop
    volatile uint16_t nbytes = num_bytes; // Byte counter for outer loop


    asm volatile(
    //       Instruction          CLK     Description                Phase       Example send 1   Example send 0
    "nextbit:\n\t"                // -    label                      (T =  0)    -
            "sbi  %0, %1\n\t"     // 2    signal HIGH                (T =  2)    signal HIGH   11 signal HIGH      11
            "sbrc %4, 7\n\t"      // 1-2  if MSB set                 (T =  ?)    1 in bit num  1  0 in bit number  11
            "mov  %6, %3\n\t"     // 0-1  tmp'll set signal high     (T =  4)    tmp = HIGH    1  tmp = LOW        -
            "dec  %5\n\t"         // 1    decrease bitcount          (T =  5)                  1                   1
            "nop\n\t"             // 1    nop (idle 1 clock cycle)   (T =  6)                  1                   1
            "st   %a2, %6\n\t"    // 2    set PORT to tmp            (T =  8)    signal tmp    11  signal tmp      00
            "mov  %6, %7\n\t"     // 1    reset tmp to low (default) (T =  9)                  1                   0
            "breq nextbyte\n\t"   // 1-2  if bitcount==0 -> nextbyte (T =  ?)                  1                   0
            "rol  %4\n\t"         // 1    shift MSB leftwards        (T = 11)                  1                   0
            "rjmp .+0\n\t"        // 2    nop nop                    (T = 13)                  11                  00
            "cbi   %0, %1\n\t"    // 2    signal LOW                 (T = 15)    signal LOW    00 signal LOW       00
            "rjmp .+0\n\t"        // 2    nop nop                    (T = 17)                  00                  00
            "nop\n\t"             // 1    nop                        (T = 18)                  0                   0
            "rjmp nextbit\n\t"    // 2    bitcount !=0 -> nextbit    (T = 20)                  00                  00
            "nextbyte:\n\t"       // -    label                      -
            "ldi  %5, 8\n\t"      // 1    reset bitcount             (T = 11)
            "ld   %4, %a8+\n\t"   // 2    val = *p++                 (T = 13)
            "cbi   %0, %1\n\t"    // 2    signal LOW                 (T = 15)
            "rjmp .+0\n\t"        // 2    nop nop                    (T = 17)
            "nop\n\t"             // 1    nop                        (T = 18)
            "dec %9\n\t"          // 1    decrease bytecount         (T = 19)
            "brne nextbit\n\t"    // 2    if bytecount!=0 -> nextbit (T = 20)

    : // No output operands
    : // Input operands   Operand Id (w/ constraint)
    "I" (_SFR_IO_ADDR(WS2821_PORTx)), // %0
    "I" (WS2812_PIN),                 // %1
    "e" (&WS2821_PORTx),              // %a2
    "r" (high),                // %3
    "r" (val),                 // %4
    "r" (nbits),               // %5
    "r" (tmp),                 // %6
    "r" (low),                 // %7
    "e" (p),                   // %a8
    "w" (nbytes)               // %9
    );

    if (led_timing_precise) sei();
    // 50us to reset
    // _delay_us(50);
}

