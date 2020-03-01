#include <Arduino.h>
#include "testBlink.h"
#include "testGPIO.h"
#include "testMicroSD.h"
#include "testFingerprint.h"
#include "testKeyboard.h"
#include "Keyboard.h"
#include "ducky.h"
#include "osType.h"

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Ducky d(OSType::Linux);
  File f;
  d.execute(f);
  
  Serial.println("Testing Blink.");
  testBlink();
  Serial.println("Testing GPIO.");
  testGPIO();
  Serial.println("Testing microSD.");
  testMicroSD();
  Serial.println("Testing Fingerprint.");
  testFingerprint();
  Serial.println("Testing Keyboard.");
  testKeyboard();
}

void loop() {

}
