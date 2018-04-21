//
// Created by Michael Gafert on 02.12.17.
//

#include <avr/io.h>
#include <stdlib.h>
#include "wifi_command.h"
#include "i2c_slave.h"

uint8_t buffer[2] = {0};
uint8_t index = 0;

void setBool(bool *set, uint8_t status) {
    *set = (bool) atoi((char*)status);
}

void process_buffer() {
    switch (buffer[0]) {
        case 's':
            setBool(&isWifiShootActive, buffer[1]);
            break;
        case 'r':
            setBool(&isWifiRightActive, buffer[1]);
            break;
        case 'l':
            setBool(&isWifiLeftActive, buffer[1]);
            break;
        case 'u':
            setBool(&isWifiUpActive, buffer[1]);
            break;
        case 'd':
            setBool(&isWifiDownActive, buffer[1]);
            break;
    }

    // reset buffer
    buffer[0] = 0;
    buffer[1] = 0;
}

void received_i2c(uint8_t data) {
    buffer[index++] = data;
    if (index >= 2) {
        index = 0;
        process_buffer();
    }
}

void requested_i2c() {

}

void init_wifi_command() {
    init_i2c(8); // Init slave with address 8
    i2c_setCallbacks(received_i2c, requested_i2c);

    sei();
}
