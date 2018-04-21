#define F_CPU 16000000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include "ir_receiver.h"

#define MICROSEC3000 55

uint8_t transmissionRunning = 0;
int8_t bitCounter = -1;
uint8_t byteCounter = 0;
uint8_t compareCode = 0, code2 = 0, code = 0, address = 0, compareAddress = 0, timerCount, finalCode, finalAddress;
volatile uint8_t firstCount = 0;
uint16_t IR_input;

uint8_t overFlows = 0;

void activateCode(unsigned int code, unsigned int address);

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
    if (isIrRightActive || isIrLeftActive || isIrUpActive || isIrDownActive || isIrShootActive)
        overFlows++;

    //Signal Length = 63ms
    //--> if there is no new Signal in the next 65,28ms (= 4 Timer Overflows), the Output is turned off automatically
    if (overFlows >= 4) {
        isIrRightActive = false;
        isIrLeftActive = false;
        isIrUpActive = false;
        isIrDownActive = false;
        isIrShootActive = false;
        overFlows = 0;
    }
}

ISR(INT0_vect) // Decode Signal
{
    if (transmissionRunning == 0) {
        if (firstCount == 0) {
            TCNT0 = 0;
            firstCount++;
        } else {
            timerCount = TCNT0;
            if (timerCount > MICROSEC3000) {
                transmissionRunning = 1;
                //PORTB |= (1<<HELP_LED);
            }
            firstCount = 0;
        }
        //Detect Start Pulse
    }

    if (transmissionRunning == 1) {
        if (bitCounter < 32) {                    //Counts from -1 - 31 = 32 Bit + 1 End Condition
            if (bitCounter == -1) {
                TCNT0 = 0;
            } else {
                byteCounter = bitCounter / 8;
                timerCount = TCNT0;
                TCNT0 = 0;

                if (timerCount > 27) {        //1,7ms
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
                if (timerCount > MICROSEC3000) {
                    transmissionRunning = 0;
                    bitCounter = -2;
                }

                if (bitCounter == 31) {
                    code2 = ~compareCode;

                    if (address == compareAddress && code == code2)
                        IR_input = (((unsigned int) code) << 8) | address;
                    else
                        IR_input = 0;

                    finalCode = (unsigned int) ((IR_input & 0xff00) >> 8);
                    finalAddress = (unsigned int) (IR_input & 0x00ff);
                    activateCode(finalCode, finalAddress);

                    address = 0;
                    code = 0;
                    code2 = 0;
                    compareAddress = 0;
                    compareCode = 0;

                    //PORTB &= ~HELP_LED;
                    TCNT0 = 0;
                    transmissionRunning = 0;
                    bitCounter = -2;
                }
            }
            bitCounter++;
        }
    }

}

void activateCode(unsigned int code, unsigned int address) {
    if (address == REMOTEADDRESS) {
        switch (code) {
            case CODERIGHT_samsung:
                isIrRightActive = true;
                overFlows = 0;
                break;
            case CODELEFT_samsung:
                isIrLeftActive = true;
                overFlows = 0;
                break;
            case CODEUP_samsung:
                isIrUpActive = true;
                overFlows = 0;
                break;
            case CODEDOWN_samsung:
                isIrDownActive = true;
                overFlows = 0;
                break;
            case CODEOK_samsung:
                isIrShootActive = true;
                overFlows = 0;
                break;
        }
    }
}