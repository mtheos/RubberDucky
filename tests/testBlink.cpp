#include <Arduino.h>
#include "testBlink.h"

void testBlink() {
  int j = 0;
  while (j++ < 3) {
    for (int i = 0; i < 3; i++) {
      TXLED1;   // set the LED off
      delay(150);
      TXLED0;
      delay(150);
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
//      TXLED0;
      delay(150);
    }
    delay(1000);
  }
}
