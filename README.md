# USB-snes-to-digital
Arduino uno project to convert USB joystick to digital for use with Commodore or Atari computers

It is currently developed to support the _iNNEXT SNES Retro USB
Controller_.

## Install

Install the _USB Host Shield Library 2.0_.
Use the Arduino IDE library manager and install version 1.3.2

Compile the program main/main.ino with the Arduino IDE.

## Usage

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

## LInks

This software was written for the Arduino Uno board:
https://store.arduino.cc/usa/arduino-uno-rev3

This software requires the USB host shield and the corresponding
library:
https://github.com/felis/USB_Host_Shield_2.0

The software was inspired by:
https://create.arduino.cc/projecthub/Bobbs/atari-ps3-controller-221d10?ref=part&ref_id=31791&offset=3
https://create.arduino.cc/projecthub/DocSnyderde/connect-usb-joystick-to-commodore-c64-2fb5ba

Arduino Timer Interrupt programming:
https://www.instructables.com/id/Arduino-Timer-Interrupts/
https://playground.arduino.cc/Code/Timer1/
https://www.robotshop.com/community/forum/t/arduino-101-timers-and-interrupts/13072

Information on Commodore 1351 mouse:
http://sensi.org/~svo/[m]ouse/
http://www.zimmers.net/anonftp/pub/cbm/documents/projects/interfaces/mouse/Mouse.html

Information how to read joystick and POT on the C64:
https://codebase64.org/doku.php?id=base:io_programming
