# Waterblaster

A watergun controlled by a Samsung Remote and all devices connected to its website.
Its entirely 3D Printed held together by M3 screws and glue.

This was a project of ours. It´s not good documented but a was a lot of fun developing.

Most of the documentation can be found in [Presentation](https://github.com/gafert/Waterblaster/tree/master/Presentation).
The joystick does not yet work reliable so use the samsung remote version.

The 3D Model is availiable as FreeCAD and Step format in [Model](https://github.com/gafert/Waterblaster/tree/master/Model)

# Pins

The pins of the modules can be changed in the header files. E.g. if your Pump is connected to Pin D3 go to pump.h and change it there.
Same with the Steppers in the stepper.h file, altough the stepper driver needs two inputs for direction and step count.
The led pin can be changed in ws2812.h and the internal led in internal_led.h (just there for testing). The Servo is connected to OCIA1. Thats PORTB1 on the ATmega328p.

The esp8266 module is connected via I2C to the Atmega328p. It serves a website under 192.168.4.1 with which the Serveo, Stepper and Pump can be controlled.
As it uses the xtensa toolchain it needs to be compiled with Platformio which has the xtensa toolchain and esptool for uploading built in. Just download platformio and add it to your path.

## Requirements

### For Waterblaster and IR Joystick
* avr-gcc
* avr-libc
* avr-binutils
* make
* cmake 3.0

### For Waterblaster WiFi functionality with the esp8266
* platformio

## Compiling

### Compiling the Waterblaster and IR Joystick files

First you need to install the requirements and add them to your PATH variable.
We used the avr cmake toolchain of [mkleemann](https://github.com/mkleemann/cmake-avr).

To compile these two programms type
```cmake CMakeLists.txt```
in the Source folder of the project.

A makefile should generate.

Type
```make help```
to see all possible commands. With for example
```make waterblaster_samsung_remote_UPLOAD```
the hex file gets compiled and uploaded to the specified port. If you want to change the port, baud rate or the avr programmer go to [Source/waterblaster/CMakeLists.txt](https://github.com/gafert/Waterblaster/blob/master/Source/waterblaster/CMakeLists.txt) or [Source/joystick/CMakeLists.txt](https://github.com/gafert/Waterblaster/blob/master/Source/joystick/CMakeLists.txt).

### Compiling the esp8266 Code

The esptool used by platformio is not working they it should be with the AI-Cloud Inside ESP-01 Module.
That´s why there is an extra [script](https://github.com/gafert/Waterblaster/blob/master/Source/waterblaster_wifi/platformio_extra_script.py) which uses the esptool from the esprissif github. To download it enter

```git submodule init 

git submodule update```

Install platformio and add it to your path.
Go to Source/waterblaster_wifi with ```cd Source/waterblaster_wifi```

Next initialize platformio with ```pio run```

Platformio now installs all neccessary components.

Next upload to the esp with ```pio run --target upload```

Platformio is smart enough to find the port automatically. You should not need to change something else, but if you want look at the [plaformio.ini](https://github.com/gafert/Waterblaster/blob/master/Source/waterblaster_wifi/platformio.ini).
