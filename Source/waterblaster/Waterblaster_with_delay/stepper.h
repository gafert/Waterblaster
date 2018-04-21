#include <avr/io.h>

#define STEPPER_DIR_PORT D
#define STEPPER_DIR_PIN 7

#define STEPPER_STEP_PORT D
#define STEPPER_STEP_PIN 6

#define STEPPER_SPEED 10000			// LOWER --> Faster, HIGHER --> Slower

void init_stepper();
void stepper_move_counter_clockwise();
void stepper_move_clockwise();