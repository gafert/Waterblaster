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
#include "millis_timer.h"
#include "uart.h"

#define NUM_LEDS 4
uint8_t *leds;

int main() {
    init_millis_timer();

    init_uart(115200);

    init_stepper();
    init_internal_led();
    init_servo();
    init_pump();
    init_ir_receiver();
    init_wifi_command();

    init_leds(leds, NUM_LEDS);

    while(1){
        // Called every loop to check animation
        led_animation_loop();

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