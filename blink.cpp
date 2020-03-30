#include <Arduino.h>
#include "blink.h"

Blink::Blink() = default;

#define OFF '0'
#define  ON '1'

#define GREEN_ON  TXLED1
#define GREEN_OFF TXLED0
#define ORANGE_ON  RXLED1
#define ORANGE_OFF RXLED0

#define STEP 100

void Blink::blinkError() {
    String green_led  = "1---0---";
    String orange_led = "0---1---";
    blink(green_led, orange_led, STEP);
}

void Blink::blinkDone() {
    String green_led  = "1-";
    String orange_led = "1--0-----------";
    blink(green_led, orange_led, STEP);
}

void Blink::blinkStarting() {
    String green_led  = "1-";
    String orange_led = "1010";
    blink(green_led, orange_led, STEP);
}

void Blink::blink(String green_led, String orange_led, int step) {
    int i = -1, j = -1;
    while (++i < green_led.length() || ++j < orange_led.length()) {
        // So strings don't need to be the same length. Last op will be repeated
        if (i == green_led.length()) i--;
        if (j == orange_led.length()) j--;

        char green = green_led.charAt(i);
        char orange = orange_led.charAt(j);
        if (green == ON)
            GREEN_ON;
        else if (green == OFF)
            GREEN_OFF;

        if (orange == ON)
            ORANGE_ON;
        else if (orange == OFF)
            ORANGE_OFF;
        delay(step);
    }
}
