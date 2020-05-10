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

#define USE_LCD 1

#if USE_LCD
#include <LiquidCrystal.h>
LiquidCrystal lcd(/*RS=*/2, /*E=*/3, /*D4..D7*/4,5,6,7);

#else
#define pinUp    0
#define pinDown  1
#define pinLeft  2
#define pinRight 3
#define pinFire  4
#define pinFire2 5
#define pinFire3 6
#endif

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
#if USE_LCD
  lcd.begin(/*columns=*/16, /*rows=*/2);
#else
  pinMode(pinUp, OUTPUT);
  pinMode(pinDown, OUTPUT);
  pinMode(pinLeft, OUTPUT);
  pinMode(pinRight, OUTPUT);
  pinMode(pinFire, OUTPUT);
#endif
}

// the loop function runs over and over again forever
char blink = LOW;
char blinkState = 0;
char togglePin = 0;
char autofire = 1;
char dirState = 0;
void loop() {
  // blink LED on pin 13
  if (++blinkState > 10)
  {
    digitalWrite(LED_BUILTIN, blink);
    blink = ! blink;
    blinkState = 0;

#if USE_LCD
    lcd.setCursor(0,1);
    lcd.print(blink ? "on " : "off");
#endif
  }

#if !USE_LCD
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
#endif

  delay(100);
}
