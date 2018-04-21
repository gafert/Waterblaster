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
#include "ws2821.h"
#include "internal_led.h"

#ifndef CONCAT_HELPER
#define CONCAT_HELPER(a, b) a ## b
#endif
#ifndef CONCAT
#define CONCAT(a, b) CONCAT_HELPER(a,b)
#endif

#define WS2821_PORTx CONCAT(PORT, WS2821_PORT)
#define WS2821_DDRx CONCAT(DDR, WS2821_PORT)
#define NUM_BITS (8) // Constant value: bits per byte

uint8_t *led_array = NULL;
uint8_t num_leds = 0;
uint8_t num_bytes = 0;

bool led_timing_precise = true;

color colors = {0,0,255};
int8_t led_index = 0;
uint8_t overflows = 10;
bool set_triggered = false;
bool reset_triggered = false;

ISR(TIMER2_COMPA_vect) {
    // Count to 6 overflow_counter as we want 100ms delay
    static uint8_t overflow_counter = 0;
    overflow_counter++;
    if (overflow_counter > overflows) {
        overflow_counter = 0;
        if(led_index == -1) {
            set_RGB_all(0,0,0);
            update_leds();
            if(reset_triggered){
                overflows = 10;
                led_index = -1;
                set_RGB_all(0, 0, 0);
                update_leds();
                colors.r = 0;
                colors.g = 0;
                colors.b = 255;
            }
        }
        set_RGB_all(0,0,0);
        set_RGB_single(led_index, colors.r, colors.g, colors.b);
        update_leds();
        led_index++;
        if (led_index > 4) {
            led_index = -1;
        }
    }
}

void set_color(color colors_){
    if(!set_triggered) {
        set_triggered = true;
        reset_triggered = false;
        overflows = 3;
        colors = colors_;
        led_index = 0;
        set_RGB_all(0, 0, 0);
        update_leds();
    }
}

void reset_color(){
    if(!reset_triggered) {
        reset_triggered = true;
        set_triggered = false;
    }
}

void init_nozzle_animation() {
    // Turn on timer
    precise_led_timing(1);
    TCCR2A |= (1 << WGM21); // CTC MODE
    TCCR2B |= (1 << CS22) | (1 << CS20) | (1 << CS21);   //  with 1024 prescaler
    TIMSK2 |= (1 << OCIE2A);   // Enable Interrupts
    OCR2A = (1/(F_CPU / 1024)) - 1; // Set compare register

    set_RGB_all(0, 0, 0);
    update_leds();
    _delay_ms(1000);
}

/**
 * Sets the RGB value of a specific led
 * @param idx The index of the led to change
 * @param r Red value (0-255) -1 to leave unchanged
 * @param g Green value (0-255) -1 to leave unchanged
 * @param b Blue value (0-255) -1 to leave unchanged
 */
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

void bounce_color(uint8_t *value) {

#define COUNT_DOWN 0
#define COUNT_UP 1

    static uint8_t mode = COUNT_UP;
    if (*value >= 255) mode = COUNT_DOWN;
    if (*value <= 0) mode = COUNT_UP;
    switch (mode) {
        case COUNT_DOWN:
            *value = *value - 1;
            break;
        case COUNT_UP:
            *value = *value + 1;
            break;
        default:
            *value = *value + 1;
            break;
    }
}

void init_leds(uint8_t *leds, uint8_t lenght) {
    led_array = leds;
    num_leds = lenght;
    num_bytes = lenght * 3; // RGB for each led

    WS2821_DDRx |= _BV(WS2821_PIN);
    WS2821_PORTx &= ~_BV(WS2821_PIN);
    if ((led_array = (uint8_t *) malloc(num_bytes))) {
        memset(led_array, 0, num_bytes);
    }
    set_RGB_all(0, 0, 0);
    update_leds();
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
            high = WS2821_PORTx | _BV(WS2821_PIN), // Bitmask for sending HIGH to pin
            low = WS2821_PORTx & ~_BV(WS2821_PIN), // Bitmask for sending LOW to pin
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
    "I" (WS2821_PIN),                 // %1
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

