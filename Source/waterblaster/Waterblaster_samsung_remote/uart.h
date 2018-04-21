//
// Created by Michael Gafert on 03.10.17.
//

#ifndef PROJECT_UART_H
#define PROJECT_UART_H

void uart_transmit(char character);
void uart_transmit_string(char *string);
char uart_receive();
void enable_uart_interrupt();
void init_uart(uint32_t baudrate);

#endif //PROJECT_UART_H
