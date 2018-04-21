#include <avr/io.h>
#include <util/delay.h>
#include "stepper.h"
#include "millis_timer.h"

#ifndef CONCAT_HELPER
#define CONCAT_HELPER(a, b) a ## b
#endif
#ifndef CONCAT
#define CONCAT(a, b) CONCAT_HELPER(a,b)
#endif

#define STEPPER_DIR_PORTx CONCAT(PORT, STEPPER_DIR_PORT)
#define STEPPER_STEP_PORTx CONCAT(PORT, STEPPER_STEP_PORT)

#define STEPPER_DIR_DDRx CONCAT(DDR, STEPPER_DIR_PORT)
#define STEPPER_STEP_DDRx CONCAT(DDR, STEPPER_STEP_PORT)

unsigned long stepper_currentMillis = 0;
unsigned long stepper_previousMillis = 0;
unsigned long difference = 0;
uint8_t stepper_speed = 1;

void init_stepper() {
    STEPPER_DIR_DDRx |= (1 << STEPPER_DIR_PIN);
    STEPPER_STEP_DDRx |= (1 << STEPPER_STEP_PIN);  //Set outputs for direction and step
    stepper_speed = 1;
}

void stepper_set_speed(uint8_t speed){
    stepper_speed = speed;
}

void stepper_move_counter_clockwise() {
    STEPPER_DIR_PORTx &= ~(1 << STEPPER_DIR_PIN);        //Set stepper direction to right - 0 = Right
    stepper_currentMillis = millis();
    difference = (stepper_currentMillis - stepper_previousMillis);
    if(difference < stepper_speed) {
        STEPPER_STEP_PORTx |= (1 << STEPPER_STEP_PIN);
    }
    else if(difference < stepper_speed * 2) {
        STEPPER_STEP_PORTx &= ~(1 << STEPPER_STEP_PIN);
    } else {
        stepper_previousMillis = stepper_currentMillis;
    }
}

void stepper_move_clockwise() {
    STEPPER_DIR_PORTx |= (1 << STEPPER_DIR_PIN);        //Set stepper direction to right - 1 = Left
    stepper_currentMillis = millis();
    difference = (stepper_currentMillis - stepper_previousMillis);
    if(difference < stepper_speed) {
        STEPPER_STEP_PORTx |= (1 << STEPPER_STEP_PIN);
    }
    else if(difference < stepper_speed * 2) {
        STEPPER_STEP_PORTx &= ~(1 << STEPPER_STEP_PIN);
    } else {
        stepper_previousMillis = stepper_currentMillis;
    }
}