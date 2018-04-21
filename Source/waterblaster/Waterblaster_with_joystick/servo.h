//
// Created by Michael Gafert on 18.11.17.
//

#ifndef EMBEDDED_SYSTEMS_SERVO_H
#define EMBEDDED_SYSTEMS_SERVO_H

// PIN is OCIA1 and PORTB1 on ATMEGA328P

void init_servo(void);
void servo_move_down();
void servo_move_up();
void servo_set_speed(uint8_t speed);

#endif //EMBEDDED_SYSTEMS_SERVO_H
