Ultimate USB to Controlport
============================
Arduino project to convert USB joystick and mouse to digital for use
with [Commodore](https://www.c64-wiki.com/wiki/Control_Port)
or [Atari](https://en.wikipedia.org/wiki/Atari_joystick_port) computers.

It is currently developed to support the [iNNEXT SNES Retro USB
Controller](https://www.google.com/search?q=innext+snes+usb+controller),
but should work with any generic USB joystick or gamepad.

Compile and Install
--------------------

Install the [USB Host Shield Library 2.0](https://github.com/felis/USB_Host_Shield_2.0).
Use the Arduino IDE library manager and install version 1.3.2

Compile the program main/main.ino with the Arduino IDE.

Usage
------

Connect a USB joystick or gamepad to the USB Host Shield.
If the USB Host Shield (Library) supports the joystick it will produce
events on the Commodore control port.

The following buttons of a gamepad are assigned to the following
directions:

- B or cross : fire
- A or circle : auto fire 5Hz
- Y or square : auto fire 3Hz
- X or triangle : up
- L or L1 or L2 : left
- L or R1 or R2 : right
- select : fire 2 on POT Y
- start : fire 3 on POT X

You can configure the frequency of the auto fire from 1Hz to 255Hz.
To configure the frequency press:
- select+start+A+up : increase the frequency of auto fire A
- select+start+A+down : decrease the frequency of auto fire A
- select+start+Y+up : increase the frequency of auto fire Y
- select+start+Y+down : decrease the frequency of auto fire Y

Hardware
---------

This project was tested with a
[Arduino Uno R3](https://store.arduino.cc/usa/arduino-uno-rev3) board
and the
[Arduino USB Host Shield](https://store.arduino.cc/usa/arduino-usb-host-shield).
While the USB Host Shield is no longer sold by the Arduino company,
you can easily find clones of the board on
[AliExpress](https://www.aliexpress.com/), where you can also buy a
clone Arduino Uno board. To connect the resistors and cables (or Sub-D
connectors) you can also purchase a
[Proto Shield R3](https://store.arduino.cc/usa/proto-shield-rev3-uno-size).

The following picture shows the loation of all I/O pins on the Arduino
Uno R3 board.

![Arduino Uno R3 pinout](https://upload.wikimedia.org/wikipedia/commons/c/c9/Pinout_of_ARDUINO_Board_and_ATMega328PU.svg)

Links
------

This software uses the
[arduino-timer](https://github.com/contrem/arduino-timer) library.

Information on Commodore 1351 mouse:
[http://sensi.org/~svo/[m]ouse/](http://sensi.org/~svo/[m]ouse/)
http://www.zimmers.net/anonftp/pub/cbm/documents/projects/interfaces/mouse/Mouse.html

Information how to read joystick and POT on the C64:
https://codebase64.org/doku.php?id=base:io_programming

Contact
--------
Write an email to Dirk Jagdmann <doj@cubic.org>
