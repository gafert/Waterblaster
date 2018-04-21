//
// Created by Michael Gafert on 31.10.17.
//

#define F_CPU 16000000UL

#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>
#include "stepper.h"
#include "servo.h"
#include "internal_led.h"
#include "ws2812.h"
#include "pump.h"
#include "ir_receiver.h"
#include "wifi_command.h"
#include "millis_timer.h"

#define NUM_LEDS 4
uint8_t *leds;

int main() {
    init_millis_timer();

    init_stepper();
    init_internal_led();
    init_servo();
    init_pump();
    init_ir_receiver();
    init_wifi_command();

    uint8_t ir_command = 0;
    uint8_t stepper_code = 0;
    uint8_t servo_code = 0;
    uint8_t movement_active = 0;
    uint8_t pump_active = 0;

    init_leds(leds, NUM_LEDS);

    while (1) {
        // Called every loop to check animation
        led_animation_loop();

        if (isTakeActionActive()) {
            ir_command = getMovementValue();

            stepper_code = (ir_command & 0b00000111);
            servo_code = ((ir_command) & 0b00111000) >> 3;

            movement_active = (ir_command & 0b01000000) >> 6;
            pump_active = (ir_command & 0b10000000) >> 7;

            // ganz links
            // 11100000
            // 11100000
            // 01100000
            // 10011111

            if (movement_active) {
                switch (stepper_code) {
                    case 0:
                        stepper_move_counter_clockwise();
                        break;
                    case 1:
                        stepper_move_counter_clockwise();
                        break;
                    case 2:
                        stepper_move_counter_clockwise();
                        break;
                    case 3:
                        break;
                    case 4:
                        break;
                    case 5:
                        stepper_move_clockwise();
                        break;
                    case 6:
                        stepper_move_clockwise();
                        break;
                    case 7:
                        stepper_move_clockwise();
                        break;
                }
                switch (servo_code) {
                    case 0:
                        servo_move_up();
                        break;
                    case 1:
                        servo_move_up();
                        break;
                    case 2:
                        servo_move_up();
                        break;
                    case 3:
                        break;
                    case 4:
                        break;
                    case 5:
                        servo_move_down();
                        break;
                    case 6:
                        servo_move_down();
                        break;
                    case 7:
                        servo_move_down();
                        break;
                }
            }
            if (pump_active) {
                pump_on();
                color colors_ = {255, 0, 1};
                set_color(colors_);
            } else {
                pump_off();
                reset_color();
            }
        }

        if(isWifiRightActive){
            stepper_move_counter_clockwise();
        }
        if(isWifiLeftActive){
            stepper_move_clockwise();
        }
        if(isWifiDownActive){
            servo_move_down();
        }
        if(isWifiUpActive){
            servo_move_up();
        }
        if(isWifiShootActive){
            pump_on();
            color colors_ = {255,0,1};
            set_color(colors_);
        } else {
            pump_off();
            reset_color();
        }
    }
}