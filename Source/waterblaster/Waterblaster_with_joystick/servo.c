#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include "millis_timer.h"

uint16_t moveTo = 375;                            //Hilfsvariable für Position
uint8_t servo_speed = 1;

ISR(TIMER1_COMPA_vect) {
    OCR1A = 5000 - OCR1A;
    // Das Servosignal wird aus der Differenz von
    // Periodenlänge (5000*0,004ms=20ms) und letztem
    // Vergleichswert (OCR1A) gebildet
    // --> Wenn OCR1A = 375 dann steht der Servo in der Mittelposition (=1,5ms)
}

// Bewegt den Servo zur richtigen Stelle
void moveServo(uint16_t position) {
    cli();
    // Da OCR1A immer hin und her toggelt (zwischen 0-500 und 4500 - 5000),
    // muss vor dem Setzen der Position überprüft werden, auf welchen Wert man gerade steht.
    if (OCR1A <= 500) {
        OCR1A = position;
    } else if (OCR1A >= 4500) {
        OCR1A = 5000 - position;
    }
    sei();
}

void init_servo(void) {
    DDRB |= _BV(PORTB1);    // Connected to PORTB1

    // Timer wird im CTC Modus betrieben =
    // ISR wird immer dann ausgelöst,
    // wenn der Wert der ins OCR1A steht durch hinaufzählen
    // von dem Startwert 0 erreicht wurde (aka Compare Match)
    // Danach wird der Startwert wieder auf 0 gesetzt (aka Clear Timer on Compare)

    TCCR1A = (1 << COM1A0);                            // Toggelt bei Compare Match PB1
    TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10); // CTC-Mode; Prescaler 64
    TIMSK1 = (1 << OCIE1A);                            // Timer-Compare Interrupt an

    OCR1A = 4625;                                      // Neutralposition ((5000-4625)*0.004ms=1,5ms)
    // Höchstwert links = 500*0.004 = 2ms
    // Höchstwert rechts = 250*0.004 = 1ms

    sei();                                             // Interrupts global an
}

void servo_set_speed(uint8_t speed) {
    servo_speed = speed;
}

unsigned long servo_currentMillis = 0;
unsigned long servo_previousMillis = 0;

void servo_move_down() {
    servo_currentMillis = millis();
    if ((servo_currentMillis - servo_previousMillis) > servo_speed) {
        servo_previousMillis = servo_currentMillis;
        if (moveTo <= 500 - 1) {
            moveTo += 1;
        }
        moveServo(moveTo);
    }
}

void servo_move_up() {
    servo_currentMillis = millis();
    if ((servo_currentMillis - servo_previousMillis) > servo_speed) {
        servo_previousMillis = servo_currentMillis;
        if (moveTo >= 250 + 1) {
            moveTo -= 1;
        }
        moveServo(moveTo);
    }
}

