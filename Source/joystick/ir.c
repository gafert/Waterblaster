//
// Created by Michael Gafert on 29.10.17.
//


#include <stdint.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>

#define DDR_IR_LED DDRD
#define PORT_IR_LED PORTD
#define PIN_IR_LED 7
#define BIT_LENGTH_0 44				//1,155ms		577,5us on/ 577,5us off
#define BIT_LENGTH_1 88				//1,155ms		577,5us on/ 1,7325ms off

volatile uint16_t command = 0;
volatile uint16_t command1 = 0;
volatile uint16_t command2 = 0;
volatile unsigned int command_lenght = 0;
volatile int command_index = -1;
volatile unsigned int on_time = 1;
volatile unsigned int start_signal = 0;
volatile _Bool command2Active=0;
volatile _Bool isSending = 0;

// Every 38kHz
ISR(TIMER1_COMPA_vect) {
	//PORT_IR_LED ^= (1<<PIN_IR_LED);
    if (command_index > 0) {
		
		//if new signal, the start signal is transmitted first
		//		 ________
		//		|        |
		//		|        |
		//		|        |
		//		|        |_________
		//		<--------><------->
		//		  4,5ms		 4,5ms	
		//		<----------------->
		//			START_SIGNAL
		
		if(start_signal==1){
			if(on_time<=171){
				PORT_IR_LED ^= (1<<PIN_IR_LED);
			}else if(on_time <= 342){
				PORT_IR_LED &= ~(1<<PIN_IR_LED);
			}else{
				on_time=1;
				start_signal=0;
			}
		}
		
		//if Bit = 1 toggle first quarter of signal 
		//		 ________
		//		|        |
		//		|        |
		//		|        |
		//		|        |___________________________
		//		<--------><-------><-------><------->
		//		  577,5us  577,5us	577,5us	 577,5us
		//		<----------------------------------->
		//					  BIT = 0
		
		if(start_signal==0){
			if ((command & (1 << (command_lenght - 1)))) {
				
				if(on_time <= BIT_LENGTH_1/4){
					PORT_IR_LED ^= (1<<PIN_IR_LED);
					}else{
					PORT_IR_LED &= ~(1<<PIN_IR_LED);
				}
				
				if (on_time >= BIT_LENGTH_1) {
					on_time = 0;
					command = ((uint16_t)command<<1);
					//uint16_t upperCommand = (uint32_t)command & ((uint32_t)0b1111111111111111 << 16);
					//uint16_t lowerCommand = (uint32_t)command & (0b1111111111111111);
					//upperCommand = ((uint16_t)upperCommand<<1);
					//lowerCommand = ((uint16_t)lowerCommand<<1);
					//command = ((uint32_t)upperCommand<<16) | lowerCommand;
					command_index--;
				}
				
				//if Bit = 0 --> toggle first half of signal
				//		 ________
				//		|        |
				//		|        |
				//		|        |
				//		|        |________
				//		<--------><------->
				//		  577,5us  577,5us
				//		<----------------->
				//			  BIT = 0
				
				} else {
				if(on_time <= BIT_LENGTH_0/2){
					PORT_IR_LED ^= (1<<PIN_IR_LED);
					}else{
					PORT_IR_LED &= ~(1<<PIN_IR_LED);
				}
				
				if (on_time >= BIT_LENGTH_0) {
					on_time = 0;
					command = ((uint16_t)command<<1);
					//uint16_t upperCommand = (uint32_t)command & ((uint32_t)0b1111111111111111 << 16);
					//uint16_t lowerCommand = (uint32_t)command & (0b1111111111111111);
					//upperCommand = ((uint16_t)upperCommand<<1);
					//lowerCommand = ((uint16_t)lowerCommand<<1);
					//command = ((uint32_t)upperCommand<<16) | lowerCommand;
					command_index--;
				}
				
			}
		}
        
			
        
        on_time++;
    }
	
	//if all bits are transmitted, send end-burst
	//		 ________
	//		|        |
	//		|        |
	//		|        |
	//		|        |
	//		<-------->
	//		  577,5us 
	//		<-------->
	//		 End-Burst
	
	if (command_index == 0) {
		if(command2Active==0){
			command = command2;
			command_lenght = 16;
			command_index = 16;
			command2Active = 1;
		}else{
			if(on_time<=22){
				PORT_IR_LED ^= (1<<PIN_IR_LED);
			}else{
				PORT_IR_LED &= ~(1<<PIN_IR_LED);
				on_time = 0;
				command_index--;
				command2Active=0;
				isSending=0;
			}
			on_time++;
		}
	}
	
}

void ir_send(uint8_t address_, uint8_t command_) {
    
		isSending=1;
		on_time = 1;
		uint8_t reverseCommand = ~command_;
		//command =   ((uint32_t)address_ << 24) | ((uint32_t)address_ << 16) | ((uint32_t)command_ << 8) | reverseCommand;
		//command = address_;
		command1 = (address_<<8) | address_;
		command2 = (command_<<8) | reverseCommand;
		command = command1;
		command_lenght = 16;
		command_index = 16;
		start_signal=1;
	
}

void init_ir() {
    // 38kHz interrupt
    // Compare Register = (Clock/(prescaler*frequency))-1
    // (16000000/(1*38000))-1 = 420
    // use of TIMER 1 because 420 > 255
    OCR1A = 420;
    TCCR1B |= (1 << WGM12); // turn on CTC mode
    TCCR1B |= (0 << CS12) | (0 << CS11) | (1 << CS10); // Set CS12, CS11 and CS10 bits for 1 prescaler
	//TCCR1B |= (1 << CS12);				//Slow mode
    TIMSK1 |= (1 << OCIE1A);

    DDR_IR_LED |= (1<<PIN_IR_LED);

    sei();
}

_Bool isCurrentlySending(){
	return isSending;
}

