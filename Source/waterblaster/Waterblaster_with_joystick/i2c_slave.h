#ifndef I2C_SLAVE_H
#define I2C_SLAVE_H

#include <avr/interrupt.h>
#include <stdint.h>

void init_i2c(uint8_t address);
void i2c_stop(void);
void i2c_setCallbacks(void (*recv)(uint8_t), void (*req)());

/**
 * By declaring a function inline, you can direct GCC to make calls to that function faster.
 * One way GCC can achieve this is to integrate that function’s code into the code for its callers.
 * This makes execution faster by eliminating the function-call overhead; in addition,
 * if any of the actual argument values are constant, their known values may permit simplifications
 * at compile time so that not all of the inline function’s code needs to be included.
 * The effect on code size is less predictable; object code may be larger or smaller with function inlining,
 * depending on the particular case.
 */
inline void __attribute__((always_inline)) I2C_transmitByte(uint8_t data)
{
  TWDR = data;
}

ISR(TWI_vect);

#endif
