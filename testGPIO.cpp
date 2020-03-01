#include <Arduino.h>
#include"testGPIO.h"

#define ONE 2
#define TWO 3
#define FOUR 4
#define EIGHT 5

void setupGPIO();
void runGPIO();

void testGPIO() {
  setupGPIO();
  runGPIO();
}

void setupGPIO() {
  // INPUT_PULLUP is the only mode that is reliable when pins are in the open state (i.e. switch)
  // Pulling pins to ground will let you read them (make sure you invert the input
  pinMode(ONE, INPUT_PULLUP);
  pinMode(TWO, INPUT_PULLUP);
  pinMode(FOUR, INPUT_PULLUP);
  pinMode(EIGHT, INPUT_PULLUP);
  Serial.println("Pin Test Ready");
}

void runGPIO() {
  static int loopCount = 0;
  Serial.print("Loop: ");
  Serial.println(++loopCount);
  int result = !digitalRead(ONE);
  result |= !digitalRead(TWO) << 1;
  result |= !digitalRead(FOUR) << 2;
  result |= !digitalRead(EIGHT) << 3;
//  int result = !digitalRead(ONE) || !digitalRead(TWO) << 1 || !digitalRead(FOUR) << 2 || !digitalRead(EIGHT) << 3;
  Serial.print("Result: ");
  Serial.println(result);
  delay(2000);
}
