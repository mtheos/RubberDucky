#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include "Keyboard.h"
#include "ducky.h"
#include "osType.h"

#include "testBlink.h"
#include "testGPIO.h"
#include "testMicroSD.h"
#include "testFingerprint.h"
#include "testKeyboard.h"

#define SD_SELECT 10

void error2(String s) {
  Serial.println(s);
  while(1);
}

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Start.");

  if (!SD.begin(SD_SELECT))
    error2("Unable to select SD... Check connections");
  Serial.println("SD card found.");
  
  Serial.print("Opening file for test...");
  // Can only have 1 file open at a time... Close before opening another.
  File f = SD.open("0000.dck", FILE_READ);
  if (!f)
    error2("Error opening file!");
  Serial.println("success.");
    
  Ducky d(OSType::Linux);
  d.execute(f);
  f.close();
  
  Serial.println("Testing Blink.");
//  testBlink();
  Serial.println("Testing GPIO.");
//  testGPIO();
  Serial.println("Testing microSD.");
//  testMicroSD();
  Serial.println("Testing Fingerprint.");
//  testFingerprint();
  Serial.println("Testing Keyboard.");
//  testKeyboard();
  Serial.println("End.");
}

void loop() {

}
