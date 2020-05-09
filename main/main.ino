/* -*- mode:C; c-basic-offset:2; indent-tabs-mode:nil -*- */
/*
  USB-snes-to-digital
  https://github.com/doj/USB-snes-to-digital

  The following Arduino Uno pins are used:
  pin 9-13 used by the USB host shield (library)

  pin 0 - up
  pin 1 - down
  pin 2 - left
  pin 3 - right
  pin 4 - fire
  pin 5 - fire 2
  pin 6 - fire 3
  you can change the pin assignment with the pin* variables below

  Features:
  - [ ] use digital x-pad for joystick directions.
  - [ ] use B button for fire
  - [ ] use X button for up direction
  - [ ] use L button for left
  - [ ] use R button for right
  - [ ] use A button for auto fire with 5 Hz
  - [ ] use Y button for auto fire with 10 Hz
  - [ ] use start button for fire 2 on POT X
  - [ ] use select button for fire 3 on POT Y
  - [ ] use analog joystick of a PlayStation 3 controller for POT X and POT Y

  /Applications/Arduino.app/Contents/Java/hardware/arduino/avr/cores/arduino/Arduino.h
*/

int pinUp = 0;
int pinDown = 1;
int pinLeft = 2;
int pinRight = 3;
int pinFire = 4;
int pinFire2 = 5;
int pinFire3 = 6;

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
