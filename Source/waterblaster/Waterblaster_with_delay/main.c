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
#include "ws2821.h"
#include "pump.h"
#include "ir_receiver.h"
#include "wifi_command.h"

#define NUM_LEDS 4
uint8_t *leds;

int main() {
    init_stepper();
    init_leds(leds, NUM_LEDS);
    init_internal_led();
    init_servo();
    init_pump();
    init_IRReceiver();
    init_nozzle_animation();
    init_wifi_command();

    while(1){
        if(isIrRightActive || isWifiRightActive){
            stepper_move_counter_clockwise();
        }
        if(isIrLeftActive || isWifiLeftActive){
            stepper_move_clockwise();
        }
        if(isIrDownActive || isWifiDownActive){
            servo_move_down();
            _delay_ms(2);
        }
        if(isIrUpActive || isWifiUpActive){
            servo_move_up();
            _delay_ms(2);
        }
        if(isIrShootActive || isWifiShootActive){
            pump_on();
            color colors_ = {255,0,1};
            set_color(colors_);
        } else {
            pump_off();
            reset_color();
        }
    }
}