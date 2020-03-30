#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include "testMicroSD.h"

#define SD_SELECT 10

// Print error and hang
void error(String s) {
  Serial.println(s);
  while(1);
}

void testMicroSD() {
  Serial.print("SD Test Ready.");
  if (!SD.begin(SD_SELECT))
    error("Unable to select SD... Check connections");
  Serial.println("SD card found.");
  
  Serial.print("Opening file for write...");
  // Can only have 1 file open at a time... Close before opening another.
  File file = SD.open("test.txt", FILE_WRITE);
  if (!file)
    error("Error opening file for write");
  Serial.println("success.");
    
  Serial.print("Writing to file...");
  file.println("Line 1");
  file.println("Line 2");
  file.close();
  Serial.println("success.");

  Serial.print("Opening file for read...");
  file = SD.open("test.txt", FILE_READ);
  if (!file)
    error("Error opening file for read");
  Serial.println("success.");
  // read from the file until there's nothing else in it:
  while (file.available()) {
    Serial.write(file.read());
//    Serial.print(file.read()); // check if print works here
  }
  file.close();
  Serial.println("success.");
  Serial.println("FIN.");
}
