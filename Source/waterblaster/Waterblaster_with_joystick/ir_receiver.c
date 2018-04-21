#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include "ir_receiver.h"

#define MICROSEC3000 55

uint8_t transmissionRunning = 0;
int8_t bitCounter = 0;
uint8_t byteCounter = 0;
uint8_t compareCode = 0, code2 = 0, code = 0, address = 0, compareAddress = 0, timerCount, finalCode, finalAddress;
volatile uint8_t firstCount = 0;
uint8_t movementCode = 0;

uint8_t overFlows = 0;

bool takeAction = false;

void activateCode(uint8_t code, uint8_t address);

void initTimer0() {
    //Counts up TCNT0 every 64us
    TCCR0B = (1 << CS00) | (1 << CS02); //Prescaler = 1024
    TIMSK0 |= (1 << TOIE0);            //Enable Timer Overflow Interrupt
    TCNT0 = 0;                        //reset the timer
}

void init_ir_receiver() {
    //stop errant interrupts until set up
    cli(); //disable all interrupts
    //DDRB |= (1<<DDB0) | (1<<DDB1);
    //DDRD|=(1<<3);
    initTimer0();


    EICRA |= (1 << ISC01);            //Fallende Flanke l�st Interrupt aus
    EIMSK |= (1 << INT0);                //INT0 Interrupt wird aktiviert. INT0 befindet sich an PORTD2

    sei(); //re-enable interrupts
    //all peripherals are now initialized
}

ISR (TIMER0_OVF_vect) // called every 16,32ms
{
    if (takeAction)
        overFlows++;

    //Signal Length = 63ms
    //--> if there is no new Signal in the next 65,28ms (= 4 Timer Overflows), the Output is turned off automatically
    if (overFlows >= 4) {
        overFlows = 0;
        takeAction = 0;
    }
}

ISR(INT0_vect)            //Encode Signal
{

    if (transmissionRunning == 0) {
        if (firstCount == 0) {
            TCNT0 = 0;
            firstCount++;
        } else {
            timerCount = TCNT0;
            if (timerCount > MICROSEC3000) {
                transmissionRunning = 1;
                TCNT0 = 0;
            }
            firstCount = 0;
        }
        //Detect Start Pulse
    } else {
        if (bitCounter < 32) {                    //Counts from 0 - 31 = 32 Bit
            byteCounter = bitCounter / 8;
            timerCount = TCNT0;
            TCNT0 = 0;

            if (timerCount > 27) {        //27 = 1,7ms = exaxt value between a transmitted 1 and a transmitted 0
                if (byteCounter == 0) {
                    address = address | (1 << (7 - (bitCounter - (byteCounter * 8))));
                } else if (byteCounter == 1) {
                    compareAddress = compareAddress | (1 << (7 - (bitCounter - (byteCounter * 8))));
                } else if (byteCounter == 2) {
                    code = code | (1 << (7 - (bitCounter - (byteCounter * 8))));
                } else {
                    compareCode = compareCode | (1 << (7 - (bitCounter - (byteCounter * 8))));
                }

            }

            bitCounter++;
            if (timerCount > MICROSEC3000) {
                transmissionRunning = 0;
                bitCounter = 0;
            }

            if (bitCounter == 32) {
                code2 = ~compareCode;                //new variable is needed, but don�t know why

                if (address == compareAddress && code == code2) {
                    finalAddress = address;
                    finalCode = code;
                } else {
                    finalAddress = 0;
                    finalCode = 0;
                }

                activateCode(finalCode, finalAddress);

                address = 0;
                code = 0;
                code2 = 0;
                compareAddress = 0;
                compareCode = 0;
                TCNT0 = 0;
                transmissionRunning = 0;
                bitCounter = 0;

            }


        }
    }

}

void activateCode(uint8_t code, uint8_t address) {
    if (address == REMOTEADDRESS) {
        takeAction = 1;
        movementCode = code;
    }
}

bool isTakeActionActive() {
    return takeAction;
}

uint8_t getMovementValue() {
    return movementCode;
}