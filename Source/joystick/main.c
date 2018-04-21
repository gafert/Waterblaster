/*
 * Infrarot_Sender.c
 *
 * Created: 08.11.2017 17:48:26
 * Author : Fabian
 */ 

#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "ir.h"

volatile uint8_t X = 0;
volatile uint8_t Y = 0;
volatile uint8_t turnActive = 0;
volatile uint8_t pumpActive = 0;

ISR(ADC_vect)
{
	if(ADC>=524 || ADC <= 500){
		turnActive = 1;
		}else{
		turnActive = 0;

	}
	
	if(ADMUX & (1<<MUX0)){
		Y = round(ADC/128);
		}else{
		X = round(ADC/128);
	}
	
	ADMUX ^= (1<<MUX0);

	ADCSRA |= (1<<ADSC);          // Start the next conversion
}

void init_ADC(){
	ADMUX = (1<<REFS0) | (1<<MUX0);							// Set Reference to AVCC and input to ADC0
	ADCSRA = (1<<ADEN)|(1<<ADPS2) |(1<<ADPS1)|(1<<ADIE);	// Enable ADC, set prescaler to 16
	// Fadc=Fcpu/prescaler=16000000/64=250kHz
	// Fadc should be between 50kHz and 200kHz
	// Enable ADC conversion complete interrupt
	
	sei();
}


int main(void)
{
	init_ir();
	init_ADC();

	//Pull-Up resistor for Joystick button
	PORTD |= (1<<6);
	uint8_t transmitCode = 0;
	
	//PORTD |= (1<<6);
	//DDRD |= (1<<0);
	
	ADCSRA |= (1<<ADSC);						// Start the first conversion
	
    while (1) 
    {
		/*
		if((PIND & (1<<6))==0){
			if(!isCurrentlySending()){
				ir_send(0b11100000, 0b01000110);
			}
			_delay_ms(5);
		}
		*/
		
		if((PIND & (1<<6))==0){
			pumpActive = 1;
			}else{
			pumpActive = 0;
		}
		
		if(turnActive == 1 || pumpActive==1){
			transmitCode |= ((uint8_t)pumpActive<<7);
			transmitCode |= ((uint8_t)turnActive<<6);
			
			transmitCode |= (Y<<3);
			transmitCode |= (X<<0);
			
			//transmitCode = 0 	0	0	0	0	0	0	0
			//				<-><->	<------->   <------->
			//			 pump turn		Y			X
			
			if(!isCurrentlySending()){
				ir_send(0b11100000, transmitCode);
			}
			_delay_ms(2);
			
			transmitCode = 0;
		}
		
    }
}

