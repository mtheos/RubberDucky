#include "Keyboard.h"
#include "ducky.h"

// :: prefix is a cheat to make parsing easier
#define TERMINAL "::term"
#define RUN "::run"
#define KEYS "::keys"
#define OS "::os"
#define SLEEP "::sleep"
#define DELAY "::delay"
#define DEFDELAY "::defdelay"
#define REM "::rem"
#define ENTER "::enter"

#define DEBUG true
#define KEYBOARD_ON true

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
    DEBUG && Serial.println("> [" + line + "]");
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
        DEBUG && Serial.println("Command is: [" + cmd + "]");

        // Terminal has proven to be a strange edge case
        if (cmd.startsWith(TERMINAL)) {
            getTerminal();
        } else if (cmd.startsWith(RUN)) {
            runProgram(line.substring(separateAt + 1));
        } else if (cmd.startsWith(KEYS)) {
            sendCombination(line);
        } else if (cmd.startsWith(OS)) {
            setOS(line.substring(separateAt + 1));
        } else if (cmd.startsWith(SLEEP) || cmd.startsWith(DELAY)) {
            delayFor(line.substring(separateAt + 1).toInt());
        } else if (line.startsWith(DEFDELAY)) {
            defaultDelay = line.substring(separateAt + 1).toInt();
        } else if (cmd.startsWith(REM)) {
                // Comment, do nothing
        } else if (cmd.startsWith(ENTER)) {
            pressEnter();
        } else {
            DEBUG && Serial.print("(Line ");
            DEBUG && Serial.print(currentLine);
            DEBUG && Serial.print(") Unknown: [");
            DEBUG && Serial.println(line + "]");
            // Maybe log to serial/file unknown command encountered
        }
    }
}

void Ducky::delayFor(int time) {
    DEBUG && Serial.print("Sleeping for ");
    DEBUG && Serial.println(time);
    delay(time);
}

void Ducky::setOS(const String &os) {
    DEBUG && Serial.println("Setting OS to " + os);
    if (os == "linux")
        osType = OSType::Linux;
    else if (os == "windows")
        osType = OSType::Windows;
    else if (os == "osx")
        osType = OSType::OSX;
}

void Ducky::runProgram(const String &programName) {
    DEBUG && Serial.println("Running Program " + programName);
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
    delayFor(defaultDelay);
}
void Ducky::runProgramLinux(const String &programName) {
    char keys[3] = { (char)KEY_LEFT_GUI, ' ', '\0' };
    sendCombination(keys);
    delayFor(2 * defaultDelay);
    sendInput(programName);
    pressEnter();
}
void Ducky::runProgramWindows(const String &programName) {
    char keys[4] = { (char)KEY_LEFT_GUI, 'r', '\0', '\0' };
    sendCombination(keys);
    delayFor(2 * defaultDelay);
    sendInput(programName);
    keys[0] = KEY_LEFT_CTRL; keys[1] = KEY_LEFT_SHIFT; keys[2] = KEY_RETURN;
    sendCombination(keys);
    delayFor(5 * defaultDelay);
    windowsUACShenanigans();
    delayFor(5 * defaultDelay);
}
void Ducky::runProgramOSX(const String &programName) {
    char keys[3] = { (char)KEY_LEFT_GUI, ' ', '\0' };
    sendCombination(keys);
    delayFor(2 * defaultDelay);
    sendInput(programName);
    pressEnter();
    delayFor(5 * defaultDelay);
}

void Ducky::getTerminal() {
    DEBUG && Serial.println("Getting Terminal");
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
    delayFor(defaultDelay);
}
void Ducky::getTerminalLinux() {
    char keys[4] = { (char)KEY_LEFT_ALT, (char)KEY_LEFT_CTRL, 't', '\0' };
    sendCombination(keys);
    delayFor(5 * defaultDelay);
}
void Ducky::getTerminalWindows() {
    runProgramWindows("powershell");
}
void Ducky::getTerminalOSX() {
    runProgramOSX("terminal");
}

/*
 * UAC run as admin prompt does not focus by default
 * This lets you programmatically select and hit allow on the window
 */
void Ducky::windowsUACShenanigans() {
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.press(KEY_TAB);
    Keyboard.release(KEY_TAB);
    delayFor(5 * defaultDelay);
    Keyboard.press(KEY_LEFT_ARROW);
    Keyboard.release(KEY_LEFT_ARROW);
    Keyboard.releaseAll();
    delayFor(defaultDelay);
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.press('Y');
    Keyboard.releaseAll();
}

void Ducky::pressEnter() {
    sendKey(KEY_RETURN);
//    Keyboard.press(KEY_RETURN);
//    Keyboard.releaseAll();
}

void Ducky::sendKey(char c) {
    DEBUG && Serial.print(c, HEX);
    KEYBOARD_ON && Keyboard.write(c); // Write is press + release
}
void Ducky::sendInput(const String &input) {
    DEBUG && Serial.print(input);
    KEYBOARD_ON && Keyboard.print(input);
}
void Ducky::sendLine(const String &line) {
    DEBUG && Serial.println(line);
    KEYBOARD_ON && Keyboard.println(line);
}

void Ducky::sendCombination(String line) {
    int keyCount = 0, cKey = 0;
    for (int i = 0; i < line.length(); i++) {
        if (line.charAt(i) == ' ')
            keyCount++;
    }
    DEBUG && Serial.print("Combination keyCount = ");
    DEBUG && Serial.print(keyCount);
    DEBUG && Serial.println();
    char *keys = (char *)malloc(sizeof(char) * keyCount);
    line += ' '; // For end delimitation.
    int separateAt = line.indexOf(' ');
    while ((line = line.substring(separateAt + 1)) != "") {
        char key = commandToCode(line.substring(0, line.indexOf(' ')));
        keys[cKey++] = key;
        separateAt = line.indexOf(' ');
    }
    DEBUG && Serial.print("Read ");
    DEBUG && Serial.print(cKey);
    DEBUG && Serial.println(" total");
    sendCombination(keys);
    free(keys);
}

void Ducky::sendCombination(char *keys) {
    DEBUG && Serial.print("Holding: [ ");
    do {
        DEBUG && Serial.print(keys[0], HEX);
        DEBUG && Serial.print(' ');
        KEYBOARD_ON && Keyboard.press(keys[0]);
    } while (*(++keys));
    DEBUG && Serial.println("]");
    /* KEYBOARD_ON && */ Keyboard.releaseAll();
}

// Absolute filth.
char Ducky::commandToCode(const String &key) {
    // If it's not a key string, it's a single character
    char code = '\0';
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

    if (DEBUG) {
        code != '\0' && Serial.print("Command Key Found: ");
        code == '\0' && Serial.print("Key Found: ");
        code == '\0' && (code = key.charAt(0));
        Serial.print(code, HEX);
        Serial.println();
    }

    // Slight duplication with the above for debugging
    if (code == '\0')
        code = key.charAt(0);
    return code;
}
