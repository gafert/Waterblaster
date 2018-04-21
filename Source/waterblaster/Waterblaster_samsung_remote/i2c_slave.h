#ifndef I2C_SLAVE_H
#define I2C_SLAVE_H

#include <avr/interrupt.h>
#include <stdint.h>

void init_i2c(uint8_t address);
void i2c_stop(void);
void i2c_setCallbacks(void (*recv)(uint8_t), void (*req)());

inline void __attribute__((always_inline)) I2C_transmitByte(uint8_t data)
{
  TWDR = data;
}

ISR(TWI_vect);

#endif
