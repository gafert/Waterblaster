include(../cmake-avr/generic-gcc-avr.cmake)
set(CMAKE_TOOLCHAIN_FILE ../cmake-avr/generic-gcc-avr.cmake)

set(AVR_MCU atmega328p)
set(MCU_SPEED "16000000")
#set(AVR_UPLOADTOOL_PORT /dev/cu.usbmodem1411)    # normale FTDI adapter
set(AVR_UPLOADTOOL_PORT /dev/cu.wchusbserial1410) # cheap chinese FTDI adapter
set(AVR_UPLOAD_SPEED 57600)
set(AVR_PROGRAMMER arduino)

# enables compiler optimization
add_definitions("-Os")

message(STATUS "Current uploadtool is: ${AVR_UPLOADTOOL}")
message(STATUS "Current programmer is: ${AVR_PROGRAMMER}")
message(STATUS "Current upload port is: ${AVR_UPLOADTOOL_PORT}")
message(STATUS "Current uploadtool options are: ${AVR_UPLOADTOOL_OPTIONS}")
message(STATUS "Current MCU is set to: ${AVR_MCU}")

add_avr_executable(joystick
        ir.c
        main.c)
