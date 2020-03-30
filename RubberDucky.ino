#include <Arduino.h>
#include "ducky.h"

void setup() {
//    Serial.begin(9600);
//    while (!Serial); // Normal way to wait for serial, but we don't want to block
//    delay(250); // Wait 250ms for serial to start (if connected)
    Ducky::run();
}

void loop() {
    // The implementation of this function is trivial, and left to the reader
}
