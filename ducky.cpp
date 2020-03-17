#include "Keyboard.h"
#include "ducky.h"

// :: prefix is a cheat to make parsing easier
#define TERMINAL "::term"
#define RUN "::run"
#define KEYS "::keys"
#define OS "::os"
#define SLEEP "::sleep"
#define DELAY "::delay"
#define REM "::rem"

#define DEBUG false

Ducky::Ducky () = default;

Ducky::Ducky(OSType os) {
    osType = os;
}

void Ducky::execute(File f) {
    if (file)
        file.close();
    file = f;
    while(file.available())
        executeLine(file.readStringUntil('\n'));
    file.close();
}

void Ducky::executeLine(const String &line) {
    currentLine++;
    if (DEBUG)
        Serial.println("> [" + line + "]");
    if (line.length() == 0)
        return; // Empty line

    // Escaped line, send the rest as is
    else if (line.startsWith("\\"))
        sendLine(line.substring(1));

    // Not a command, send as is
    else if (!line.startsWith("::"))
        sendLine(line);

    else {
        // Only thing left is a command
        int separateAt = line.indexOf(' ');
        String cmd = line.substring(0, separateAt);
        cmd.toLowerCase();
        if (DEBUG)
            Serial.println("Command is: [" + cmd + "]");

        // Terminal has proven to be a strange edge case
        if (cmd.startsWith(TERMINAL)) {
            getTerminal();
        } else if (cmd == RUN) {
            runProgram(line.substring(separateAt + 1));
        } else if (cmd == KEYS) {
            sendCombination(line);
        } else if (cmd == OS) {
            setOS(line.substring(separateAt + 1));
        } else if (cmd == SLEEP || cmd == DELAY) {
            delayBy(line.substring(separateAt + 1).toInt());
        } else if (cmd == REM) {
            // Comment, do nothing
        } else {
            Serial.print("(Line ");
            Serial.print(currentLine);
            Serial.print(") Unknown: [");
            Serial.println(line + "]");
            // Maybe log to serial/file unknown command encountered
        }
    }
    if (DEBUG)
        Serial.println();
}

void Ducky::delayBy(int time) {
    if (DEBUG)
        Serial.println("Sleeping for " [time]);
    delay(time);
}

void Ducky::setOS(const String &os) {
    if (DEBUG)
        Serial.println("Setting OS to " + os);
    if (os == "linux")
        osType = OSType::Linux;
    else if (os == "windows")
        osType = OSType::Windows;
    else if (os == "osx")
        osType = OSType::OSX;
}

void Ducky::runProgram(const String &programName) {
    if (DEBUG)
        Serial.println("Running Program " + programName);
    switch (osType) {
        case OSType::Linux:
            runProgramLinux(programName);
            break;
        case OSType::Windows:
            runProgramWindows(programName);
            break;
        case OSType::OSX:
            runProgramOSX(programName);
            break;
    }
    delay(defaultDelay);
}
void Ducky::runProgramLinux(const String &programName) {
    char keys[3] = { (char)KEY_LEFT_GUI, ' ', '\0' };
    sendCombination(keys);
    sendInput(programName);
    pressEnter();
}
void Ducky::runProgramWindows(const String &programName) {
    char keys[3] = { (char)KEY_LEFT_GUI, 'r', '\0' };
    sendCombination(keys);
    sendInput(programName);
    pressEnter();
}
void Ducky::runProgramOSX(const String &programName) {
    char keys[3] = { (char)KEY_LEFT_GUI, ' ', '\0' };
    sendCombination(keys);
    sendInput(programName);
    pressEnter();
}

void Ducky::getTerminal() {
    if (DEBUG)
        Serial.println("Getting Terminal");
    switch (osType) {
        case OSType::Linux:
            getTerminalLinux();
            break;
        case OSType::Windows:
            getTerminalWindows();
            break;
        case OSType::OSX:
            getTerminalOSX();
            break;
    }
    delay(defaultDelay);
}
void Ducky::getTerminalLinux() {
    char keys[4] = { (char)KEY_LEFT_ALT, (char)KEY_LEFT_CTRL, 't', '\0' };
    sendCombination(keys);
}
void Ducky::getTerminalWindows() {
    char keys[4] = { (char)KEY_LEFT_GUI, 'r', '\0', '\0' };
    sendCombination(keys);
    sendInput("powershell");
    keys[0] = KEY_LEFT_CTRL; keys[1] = KEY_LEFT_SHIFT; keys[2] = KEY_RETURN;
    sendCombination(keys);
    sendKey(KEY_LEFT_ARROW);
    pressEnter();
}
void Ducky::getTerminalOSX() {
    char keys[3] = { (char)KEY_LEFT_GUI, ' ', '\0' };
    sendCombination(keys);
    sendInput("terminal");
    pressEnter();
}

void Ducky::pressEnter() {
    sendKey(KEY_RETURN);
//    Keyboard.press(KEY_RETURN);
//    Keyboard.releaseAll();
}

void Ducky::sendKey(char c) {
    if (!DEBUG)
        Keyboard.write(c); // Write is press + release
    else
        Serial.print(c, HEX);
}
void Ducky::sendInput(const String &input) {
    if (!DEBUG)
        Keyboard.print(input);
    else
        Serial.print(input);
}
void Ducky::sendLine(const String &line) {
    if (!DEBUG)
        Keyboard.println(line);
    else
        Serial.println(line);
}

void Ducky::sendCombination(String line) {
    int keyCount = 0, cKey = 0;
    for (int i = 0; i < line.length(); i++) {
        if (line.charAt(i) == ' ')
            keyCount++;
    }
    if (DEBUG)
        Serial.println(&"Combination keyCount = " [ keyCount]);
    char *keys = (char *)malloc(sizeof(char) * keyCount);
    line += ' '; // For end delimitation.
    int separateAt = line.indexOf(' ');
    while ((line = line.substring(separateAt + 1)) != "") {
        char key = commandToCode(line.substring(0, line.indexOf(' ')));
        keys[cKey++] = key;
        separateAt = line.indexOf(' ');
    }
    sendCombination(keys);
    if (DEBUG) {
        Serial.print("Read ");
        Serial.print(cKey);
        Serial.println(" total");
    }
    free(keys);
}

void Ducky::sendCombination(char *keys) {
    if (!DEBUG) {
        do {
            Keyboard.press(keys[0]);
        } while (++keys);
        delayBy(100);
        Keyboard.releaseAll();
        delayBy(800);
    } else {
        Serial.print("Holding: [ ");
        do {
            Serial.print((char)keys[0], HEX);
            Serial.print(' ');
        } while (++keys);
        Serial.println("]");
    }
}

// Absolute filth.
char Ducky::commandToCode(const String &key) {
    // If it's not a key string, it's a single character
    char code = key.charAt(0);
    if (key == "KEY_LEFT_CTRL")
        code = KEY_LEFT_CTRL;
    else if (key == "KEY_LEFT_SHIFT")
        code = KEY_LEFT_SHIFT;
    else if (key == "KEY_LEFT_ALT")
        code = KEY_LEFT_ALT;
    else if (key == "KEY_LEFT_GUI")
        code = KEY_LEFT_GUI;
    else if (key == "KEY_RIGHT_CTRL")
        code = KEY_RIGHT_CTRL;
    else if (key == "KEY_RIGHT_SHIFT")
        code = KEY_RIGHT_SHIFT;
    else if (key == "KEY_RIGHT_ALT")
        code = KEY_RIGHT_ALT;
    else if (key == "KEY_RIGHT_GUI")
        code = KEY_RIGHT_GUI;
    else if (key == "KEY_UP_ARROW")
        code = KEY_UP_ARROW;
    else if (key == "KEY_DOWN_ARROW")
        code = KEY_DOWN_ARROW;
    else if (key == "KEY_LEFT_ARROW")
        code = KEY_LEFT_ARROW;
    else if (key == "KEY_RIGHT_ARROW")
        code = KEY_RIGHT_ARROW;
    else if (key == "KEY_BACKSPACE")
        code = KEY_BACKSPACE;
    else if (key == "KEY_TAB")
        code = KEY_TAB;
    else if (key == "KEY_RETURN")
        code = KEY_RETURN;
    else if (key == "KEY_ESC")
        code = KEY_ESC;
    else if (key == "KEY_INSERT")
        code = KEY_INSERT;
    else if (key == "KEY_DELETE")
        code = KEY_DELETE;
    else if (key == "KEY_PAGE_UP")
        code = KEY_PAGE_UP;
    else if (key == "KEY_PAGE_DOWN")
        code = KEY_PAGE_DOWN;
    else if (key == "KEY_HOME")
        code = KEY_HOME;
    else if (key == "KEY_END")
        code = KEY_END;
    else if (key == "KEY_CAPS")
        code = KEY_CAPS_LOCK;
    else if (key == "KEY_F1")
        code = KEY_F1;
    else if (key == "KEY_F2")
        code = KEY_F2;
    else if (key == "KEY_F3")
        code = KEY_F3;
    else if (key == "KEY_F4")
        code = KEY_F4;
    else if (key == "KEY_F5")
        code = KEY_F5;
    else if (key == "KEY_F6")
        code = KEY_F6;
    else if (key == "KEY_F7")
        code = KEY_F7;
    else if (key == "KEY_F8")
        code = KEY_F8;
    else if (key == "KEY_F9")
        code = KEY_F9;
    else if (key == "KEY_F10")
        code = KEY_F10;
    else if (key == "KEY_F11")
        code = KEY_F11;
    else if (key == "KEY_F12")
        code = KEY_F12;

    return code;
}
