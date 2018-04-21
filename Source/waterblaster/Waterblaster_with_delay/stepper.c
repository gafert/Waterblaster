#include <avr/io.h>
#include <util/delay.h>
#include "stepper.h"

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

void init_stepper() {
    STEPPER_DIR_DDRx |= (1 << STEPPER_DIR_PIN);
    STEPPER_STEP_DDRx |= (1 << STEPPER_STEP_PIN);  //Set outputs for direction and step
}

void stepper_move_counter_clockwise() {
    STEPPER_DIR_PORTx &= ~(1 << STEPPER_DIR_PIN);        //Set stepper direction to right - 0 = Right

    //Make a single step
    STEPPER_STEP_PORTx |= (1 << STEPPER_STEP_PIN);
    _delay_us(STEPPER_SPEED);
    STEPPER_STEP_PORTx &= ~(1 << STEPPER_STEP_PIN);
    _delay_us(STEPPER_SPEED);
}

void stepper_move_clockwise() {
    STEPPER_DIR_PORTx |= (1 << STEPPER_DIR_PIN);        //Set stepper direction to right - 1 = Left

    //Make a single step
    STEPPER_STEP_PORTx |= (1 << STEPPER_STEP_PIN);
    _delay_us(STEPPER_SPEED);
    STEPPER_STEP_PORTx &= ~(1 << STEPPER_STEP_PIN);
    _delay_us(STEPPER_SPEED);
}