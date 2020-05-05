/* -*- mode:C; c-basic-offset:2; indent-tabs-mode:nil -*- */
/*
  USB-snes-to-digital
  https://github.com/doj/USB-snes-to-digital

  This software requires the USB host shield and the corresponding library:
  https://github.com/felis/USB_Host_Shield_2.0
  https://www.arduinolibraries.info/libraries/usb-host-shield-library-2-0

  The software was inspired by
  https://create.arduino.cc/projecthub/Bobbs/atari-ps3-controller-221d10?ref=part&ref_id=31791&offset=3
  https://create.arduino.cc/projecthub/DocSnyderde/connect-usb-joystick-to-commodore-c64-2fb5ba

  The following Arduino Uno pins are used:
  pin 9-13 used by the USB host shield (library)

  pin 0 - up
  pin 1 - down
  pin 2 - left
  pin 3 - right
  pin 4 - fire
  you can change the pin assignment with the pin* variables below

  Features:
  - use digital x-pad for joystick directions.
  - use X button for fire
  - use X button for auto fire with 5 Hz
  - use X button for auto fire with 10 Hz
  - use X button for up direction
  - use start button for fire 2 on POT X
  - use select button for fire 3 on POT Y

  ~/Applications/Arduino.app/Contents/Java/hardware/arduino/avr/cores/arduino/Arduino.h
*/

int pinUp = 0;
int pinDown = 1;
int pinLeft = 2;
int pinRight = 3;
int pinFire = 4;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(pinUp, OUTPUT);
  pinMode(pinDown, OUTPUT);
  pinMode(pinLeft, OUTPUT);
  pinMode(pinRight, OUTPUT);
  pinMode(pinFire, OUTPUT);
}

// the loop function runs over and over again forever
int blink = LOW;
int blinkState = 0;
int togglePin = 0;
int autofire = 1;
int dirState = 0;
void loop() {
  // blink LED on pin 13
  if (++blinkState > 10)
  {
    digitalWrite(LED_BUILTIN, blink);
    blink = ! blink;
    blinkState = 0;
  }

  if (++dirState > 10)
  {
    dirState = 0;
    digitalWrite(togglePin, LOW);
    if (++togglePin >= pinFire)
    {
      togglePin = pinUp;
    }
    digitalWrite(togglePin, HIGH);
  }

  if (autofire == 1)
  {
    digitalWrite(pinFire, HIGH);
    autofire = 2;
  }
  else if (autofire == 2)
  {
    digitalWrite(pinFire, LOW);
    autofire = 1;
  }

  delay(100);
}
