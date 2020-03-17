#include <Arduino.h>
#include "testBlink.h"

void testBlink() {
  int i = 0;
  while (i++ < 3) {
    for (int i = 0; i < 3; i++) {
      TXLED1;   // set the LED off
      delay(150);
      TXLED0;
      delay(120);
    }
    delay(400);
    for (int i = 0; i < 3; i++) {
      RXLED1;   // set the LED off
      delay(400);
      RXLED0;
      delay(400);
    }
    delay(400);
    for (int i = 0; i < 3; i++) {
      TXLED1;   // set the LED off
      delay(150);
      TXLED0;
      delay(150);
    }
    delay(1000);
  }
}
