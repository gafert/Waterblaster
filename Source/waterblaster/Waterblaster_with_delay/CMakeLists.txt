# Needs for micro
#set(AVR_MCU atmega32u4)
#set(AVR_UPLOADTOOL 32u4_uploader)
#set(AVR_PROGRAMMER avr109)

add_avr_executable(waterblaster_with_delay
        stepper.c
        ws2821.c
        pump.c
        servo.c
        internal_led.c
        ir_receiver.c
        i2c_slave.c
        wifi_command.c
        main.c)