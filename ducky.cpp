#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include "FingerprintUSBHost.h"
#include "ducky.h"
#include "duckyParser.h"
#include "blink.h"

#define SD_SELECT 10
#define        ONE 6
#define        TWO 7
#define       FOUR 8
#define      EIGHT 9

Ducky::Ducky () = default;

void Ducky::run() {
    initializeGPIO();
    bool loop = true;
    String last_gpio = "";
    while(loop) {
        String gpio = readGPIO();
        if (gpio == last_gpio) {
            Blink::blinkDone();     // blinkDone takes 1.5 seconds, so this is effectively a short delay
            delay(5000);            // Make it a little longer as it take times to set switches
            Blink::blinkStarting(); // blinkStaring only takes 400ms
            continue;
        }
        last_gpio = gpio;
        Serial.println(gpio);
        OSType osType;
        if (gpio == "0000") {
            Serial.println("Safe mode");
            continue;
        } else if (gpio == "1111") {
            Serial.println("Script Select");
            osType = FingerprintUSBHost().guessHostOS();
            switch (osType) {
                case OSType::Linux:
                    Serial.println("Linux detected");
                    gpio = gpio + "_lin";
                    break;
                case OSType::Windows:
                    Serial.println("Windows detected");
                    gpio = gpio + "_win";
                    break;
                case OSType::OSX:
                    Serial.println("OSX detected");
                    gpio = gpio + "_osx";
                    break;
                case OSType::Unknown:
                    Serial.println("OS not detected");
                    break;
            }
        } else if (gpio[0] == '1') {
            Serial.println("OS Select");
            osType = FingerprintUSBHost().guessHostOS();
        } else {
            osType = OSType::Unknown;
        }
        if (!SD.begin(SD_SELECT))
            error("Unable to select SD... Check connections");
        Serial.println("SD card found.");
        Serial.print("Opening file \"");
        Serial.print(gpio);
        Serial.println(".dck\".");
        // Can only have 1 file open at a time... Close before opening another.
        File f = SD.open(gpio + ".dck", FILE_READ);
        if (!f)
            error("Error opening file!");
        Serial.println("Success.");
        Serial.println("Running.");
        DuckyParser dp(osType);
        int err = dp.execute(f);
        f.close();
        if (err)
            error("Parse error line: " + String(err));
        if (gpio == "Never")
            loop = false; // Stops infinite loop warnings
    }
    Serial.println("Finished.");
}

void Ducky::error(const String &msg) {
    Serial.println(msg);
    while (true) Blink::blinkError();
}

void Ducky::initializeGPIO() {
    pinMode(ONE, INPUT_PULLUP);
    pinMode(TWO, INPUT_PULLUP);
    pinMode(FOUR, INPUT_PULLUP);
    pinMode(EIGHT, INPUT_PULLUP);
}

String Ducky::readGPIO() {
    char buff[5];
    // 0x30 is 0 in ASCII
    buff[0] = (char)(!digitalRead(ONE) + 0x30);
    buff[1] = (char)(!digitalRead(TWO) + 0x30);
    buff[2] = (char)(!digitalRead(FOUR) + 0x30);
    buff[3] = (char)(!digitalRead(EIGHT) + 0x30);
    buff[4] = '\0';
    return buff;
}
