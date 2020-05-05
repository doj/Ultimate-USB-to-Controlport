/*
  USB-snes-to-digital
  https://github.com/doj/USB-snes-to-digital

  This software requires the USB host shield and the corresponding library:
  https://github.com/felis/USB_Host_Shield_2.0
  https://www.arduinolibraries.info/libraries/usb-host-shield-library-2-0

  The following Arduino Uno pins are used:
  pin 9-13 used by the USB host shield (library)

  pin 0 - up
  pin 1 - down
  pin 2 - left
  pin 3 - right
  pin 4 - fire
  you can change the pin assignment with the pin* variables below
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
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}
