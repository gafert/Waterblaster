//
// Created by Michael Gafert on 03.10.17.
//
#define F_CPU 16000000
#include <avr/io.h>
#include <stdbool.h>
#include <avr/interrupt.h>
#include "uart.h"

void uart_transmit(char character) {
    while (!(UCSR0A & (1 << 5)));
    UDR0 = character;
}

void uart_transmit_string(char *string) {
    for (int i = 0; string[i] != '\0'; i++) {
        uart_transmit(string[i]);
    }
}

char uart_receive() {
    // Whait for uart to receive string
    while (!(UCSR0A & (1 << RXC0)));
    return UDR0;
}

void enable_uart_interrupt(){
    // Enable the USART Recieve Complete interrupt (USART_RXC)
    UCSR0B |= (1 << RXCIE0);
}

void init_uart(uint32_t baudrate) {
    // UBRR formula from datasheet; 8 as we use doublespeed
    uint16_t ubrr = (F_CPU / 8 / baudrate) - 1;
    // Set baud rate, use double speed
    UBRR0H = (uint8_t) (ubrr >> 8); // get higher 8 bits
    UBRR0L = (uint8_t) (ubrr & 0xff); // get lower 8 bits
    UCSR0A |= (1 << U2X0);
    // Enable receiver and transmitter
    UCSR0B = (1 << TXEN0 | 1 << RXEN0);
}
