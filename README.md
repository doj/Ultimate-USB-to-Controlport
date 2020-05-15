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

| NES    | Sony     | Control Port |
| ------ | -------- | ------------
| B      | cross    | fire
| A      | circle   | auto fire A, 10 Hz
| Y      | square   | auto fire Y, 5 Hz
| X      | triangle | up
| n/a    | L1       | left
| n/a    | R1       | right
| L      | L2       | left
| R      | R2       | right
| select | select   | fire 2 on POT Y
| start  | start    | fire 3 on POT X

You can configure the frequency of the auto fire from 1 Hz to 255 Hz.
To configure the frequency press:
- select+start+A+up : increase the frequency of auto fire A
- select+start+A+down : decrease the frequency of auto fire A
- select+start+Y+up : increase the frequency of auto fire Y
- select+start+Y+down : decrease the frequency of auto fire Y

You can switch the directions of all buttons (lefty mode), press:
select+start+B+left.

A USB mouse will be used to output the
[Commodore 1351](https://www.c64-wiki.com/wiki/Mouse_1351) mouse
protocol on the Pot X and Pot Y control port inputs. The following
table shows the mouse button mapping:

| Mouse      | Control Port |
| ---------- | ------------ |
| left       | fire
| middle     | down
| right      | up
| wheel up   | left
| wheel down | right
| left side  | left
| right side | right

The mouse wheel is using the [Micromys protocol](http://wiki.icomp.de/wiki/Micromys_Protocol).
Left and Right side buttons are used on the [Microsoft Intellimouse](https://en.wikipedia.org/wiki/IntelliMouse).

The table below shows how keyboard keys are mapped to the control
port:

| Key | Control Port |
| --- | ------------ |
| up, num 8, w, i | up
| down, num 2, s, k | down
| left, 4, a, j | left
| right, 6, d, l | right
| ins, space, ~, enter, F1, b | fire (gamepad B)
| y | auto fire (gamepad Y)
| x | up (gamepad X)
| F2 | fire 2 on POT Y (gamepad select)
| F3 | fire 3 on POT X (gamepad start)

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

The following table shows how the 2 [control ports](https://en.wikipedia.org/wiki/Atari_joystick_port) with [https://en.wikipedia.org/wiki/D-subminiature](D-sub 9)
connectors should be connected to the Arduino pins. This suggested
connection is for a Commodore 64. You can change the pin assignments
in the config.h file.

Control Port 1

| Arduino | D-sub | Function |
| ------- | ----- | -------- |
| A0      | 1     | Up       |
| A1      | 2     | Down     |
| A2      | 3     | Left     |
| A3      | 4     | Right    |
| A5      | 5     | Pot Y    |
| A4      | 6     | Fire     |
| (n/c)   | 7     | +5V      |
| GND     | 8     | GND      |
| D2      | 9     | Pot X    |

Control Port 2

| Arduino | D-sub | Function |
| ------- | ----- | -------- |
| D4      | 1     | Up       |
| D5      | 2     | Down     |
| D6      | 3     | Left     |
| D7      | 4     | Right    |
| D1      | 5     | Pot Y    |
| D8      | 6     | Fire     |
| (n/c)   | 7     | +5V      |
| GND     | 8     | GND      |
| D3      | 9     | Pot X    |

Note: On the Arduino pins D0 and D1 are used for the serial debug
console. If you connect the control port Pot Y line to D1, you can't
use the serial console any more.

If you want to change the Arduino pin assignments, change the config.h file.

If you have a clone USB Host Shield, the following article may help
you fix it: https://esp8266-notes.blogspot.com/2017/08/defective-arduino-usb-host-shield-boards.html

Feature Implementation Status
------------------------------
The following list shows which features are implemented.

- [X] use digital x-pad for joystick directions.
- [X] use B button for fire
- [X] use X button for up direction
- [X] use L button for left
- [X] use R button for right
- [X] use A button for auto fire
- [X] use Y button for auto fire
- [X] configure auto fire frequency
- [X] use start button for fire 2 on POT Y
- [X] use select button for fire 3 on POT X
- [X] support two USB joysticks with both control ports
- [X] lefty mode
- [X] support mouse side buttons
- [X] support PlayStation Classic USB controller
- [X] use keyboard as joystick
- [ ] the USB controller should remember the button state and count
  how many button down pushes were made. However this is not so easy
  when autofire is selected.
- [ ] switch joystick 1,2 and mouse 1,2 with button combination
- [ ] reconfigure any USB button to any Commodore button
- [ ] save configuration to Arduino EEPROM https://www.arduino.cc/en/Reference/EEPROM
- [ ] support PlayStation 3 controller
- [ ] support PlayStation 4 controller
- [ ] use analog joystick of a PlayStation 3 controller for POT X and POT Y
- [ ] support analog hat button
- [ ] support Xbox controller
- [ ] use USB mouse for POT X and POT Y with 1351 protocol
- [ ] support 1351 mouse wheel
- [ ] fix device mapping in USB Host Shield Library
- [ ] MIDI input
- [ ] burst fire mode

Controllers
------------
The following controllers have been tested with the software:

- [Sony Playstation Classic](https://en.wikipedia.org/wiki/PlayStation_Classic) USB controller.
  ![Sony Playstation Classic](https://upload.wikimedia.org/wikipedia/commons/thumb/a/aa/PlayStation_Classic_Konsole_%2B_Controller.jpg/640px-PlayStation_Classic_Konsole_%2B_Controller.jpg)

- iNNEXT SNES USB controller, which you also find under other brand
names like kiwitata, retroflag.
  ![SNES USB controller](https://www.picclickimg.com/00/s/MTAwMVgxMDAw/z/sV0AAOSwYY1enxD7/$/2x-iNNEXT-SNES-USB-Wired-Game-Controller-Gamepad-_57.jpg)

- [Microsoft Intellimouse optical USB](https://en.wikipedia.org/wiki/IntelliMouse).
  ![intellimouse](https://images.fastcompany.net/image/upload/w_596,c_limit,q_auto:best,f_auto/wp-cms/uploads/sites/4/2018/01/i-2-the-intellimouse-abides.jpg)

- generic 3 button + wheel USB mouse

- generic USB keyboard

Serial Debug Console
---------------------
By default the serial debug console is enabled and uses 115200
baud. See the config.h file to configure the baud rate or to disable
the debug console output.

Links
------

This software uses the
[arduino-timer](https://github.com/contrem/arduino-timer) library.

Information on Commodore 1351 mouse:
[http://sensi.org/~svo/[m]ouse/](http://sensi.org/~svo/[m]ouse/)
http://www.zimmers.net/anonftp/pub/cbm/documents/projects/interfaces/mouse/Mouse.html

Information how to read joystick and POT on the C64:
https://codebase64.org/doku.php?id=base:io_programming

Information on the USB protocol:
http://www.usbmadesimple.co.uk/ums_4.htm

USB Host Shield 2.0 Library with fixes and better debuggin messages:
https://github.com/doj/USB_Host_Shield_2.0

Contact
--------
Write an email to Dirk Jagdmann <doj@cubic.org>
The source code is hosted on https://github.com/doj/Ultimate-USB-to-Controlport
