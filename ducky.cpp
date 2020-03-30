#include "Keyboard.h"
#include "ducky.h"
#include "specialKeys.h"

// :: prefix is a cheat to make parsing easier
#define TERMINAL "::term"
#define RUN "::run"
#define KEYS "::keys"
#define OS "::os"
#define SLEEP "::sleep"
#define DELAY "::delay"
#define DEF_DELAY "::defdelay"
#define REM "::rem"
#define ENTER "::enter"

#define DEBUG
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
#ifdef DEBUG
    Serial.println("> [" + line + "]");
#endif
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
#ifdef DEBUG
        Serial.println("Command is: [" + cmd + "]");
#endif

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
        } else if (line.startsWith(DEF_DELAY)) {
            defaultDelay = line.substring(separateAt + 1).toInt();
        } else if (cmd.startsWith(REM)) {
                // Comment, do nothing
        } else if (cmd.startsWith(ENTER)) {
            pressEnter();
        } else {
#ifdef DEBUG
            Serial.print("(Line ");
            Serial.print(currentLine);
            Serial.print(") Unknown: [");
            Serial.println(line + "]");
#endif
            // Maybe log to serial/file unknown command encountered
        }
    }
}

void Ducky::delayFor(int time) {
#ifdef DEBUG
    Serial.print("Sleeping for ");
    Serial.println(time);
#endif
    delay(time);
#ifdef DEBUG
    Serial.println("Awake");
#endif
}

void Ducky::setOS(const String &os) {
#ifdef DEBUG
    Serial.println("Setting OS to " + os);
#endif
    if (os == "linux")
        osType = OSType::Linux;
    else if (os == "windows")
        osType = OSType::Windows;
    else if (os == "osx")
        osType = OSType::OSX;
}

void Ducky::runProgram(const String &programName) {
#ifdef DEBUG
    Serial.println("Running Program " + programName);
#endif
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
#ifdef DEBUG
    Serial.println("Getting Terminal");
#endif
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
#ifdef DEBUG
    Serial.print(c, HEX);
#endif
    KEYBOARD_ON && Keyboard.write(c); // Write is press + release
}
void Ducky::sendInput(const String &input) {
#ifdef DEBUG
    Serial.print(input);
#endif
    KEYBOARD_ON && Keyboard.print(input);
}
void Ducky::sendLine(const String &line) {
#ifdef DEBUG
    Serial.println(line);
#endif
    KEYBOARD_ON && Keyboard.println(line);
}

void Ducky::sendCombination(String line) {
    int keyCount = 0, cKey = 0;
    for (int i = 0; i < line.length(); i++) {
        if (line.charAt(i) == ' ')
            keyCount++;
    }
#ifdef DEBUG
    Serial.print("Combination keyCount = ");
    Serial.print(keyCount);
    Serial.println();
#endif
    char *keys = (char *)malloc(sizeof(char) * keyCount);
    line += ' '; // For end delimitation.
    int separateAt = line.indexOf(' ');
    while ((line = line.substring(separateAt + 1)) != "") {
        char key = specialKeys::keyToCode(line.substring(0, line.indexOf(' ')));
        keys[cKey++] = key;
        separateAt = line.indexOf(' ');
    }
#ifdef DEBUG
    Serial.print("Read ");
    Serial.print(cKey);
    Serial.println(" total");
#endif
    sendCombination(keys);
    free(keys);
}

// Offt horrible #ifdef
void Ducky::sendCombination(char *keys) {
#ifdef DEBUG
    Serial.print("Holding: [ ");
#endif
    do {
#ifdef DEBUG
        Serial.print(keys[0], HEX);
        Serial.print(' ');
#endif
        KEYBOARD_ON && Keyboard.press(keys[0]);
    } while (*(++keys));
#ifdef DEBUG
    Serial.println("]");
#endif
    /* KEYBOARD_ON && */ Keyboard.releaseAll();
}
