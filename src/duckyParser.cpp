#include "Keyboard.h"
#include "duckyParser.h"
#include "specialKeys.h"

// :: prefix is a cheat to make parsing easier
#define TERMINAL "::term"
#define RUN "::run"
#define KEYS "::keys"
#define OS "::os"
#define SLEEP "::sleep"
#define DELAY "::delay"
#define SET_DELAY "::setdelay"
#define REM "::rem"
#define ENTER "::enter"

//#define DEBUG
#define KEYBOARD_ON true

DuckyParser::DuckyParser () = default;

int DuckyParser::execute(File f) {
    while(f.available())
        // If error on line, return line number
        if (executeLine(f.readStringUntil('\n'))) // Terminator is not included
            return currentLine;
    return 0;
}

int DuckyParser::executeLine(const String &line) {
    currentLine++;
    if (line.endsWith("\r")) // Windows carriage return
        line = line.substring(0, line.length() - 1);
#ifdef DEBUG
    Serial.println("> [" + line + "]");
#endif
    if (line.length() == 0)
        return 0; // Empty line

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
        } else if (line.startsWith(SET_DELAY)) {
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
            return 1;
        }
    }
    return 0;
}

void DuckyParser::delayFor(int time) {
#ifdef DEBUG
    Serial.print("Sleeping for ");
    Serial.println(time);
#endif
    delay(time);
#ifdef DEBUG
    Serial.println("Awake");
#endif
}

void DuckyParser::setOS(String os) {
    os.toLowerCase();
#ifdef DEBUG
    Serial.print("Setting OS to " + os);
#endif
    if (os == "linux")
        osType = OSType::Linux;
    else if (os == "windows")
        osType = OSType::Windows;
    else if (os == "osx")
        osType = OSType::OSX;
    else {
        osType = OSType::Unknown;
#ifdef DEBUG
        Serial.println("OS [" + os + "] not recognised");
#endif
    }
}

void DuckyParser::runProgram(const String &programName) {
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
        case OSType::Unknown:
            // Fail horribly
#ifdef DEBUG
            Serial.println("Run program called but OS not Set");
#endif
            exit(1);
            break;
    }
    delayFor(defaultDelay);
}
void DuckyParser::runProgramLinux(const String &programName) {
    char keys[2] = { (char)KEY_LEFT_GUI, '\0' };
    sendCombination(keys);
    delayFor(2 * defaultDelay);
    sendInput(programName);
    pressEnter();
    delayFor(5 * defaultDelay);
}
void DuckyParser::runProgramWindows(const String &programName) {
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
void DuckyParser::runProgramOSX(const String &programName) {
    char keys[3] = { (char)KEY_LEFT_GUI, ' ', '\0' };
    sendCombination(keys);
    delayFor(2 * defaultDelay);
    sendInput(programName);
    pressEnter();
    delayFor(5 * defaultDelay);
}

void DuckyParser::getTerminal() {
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
        case OSType::Unknown:
            // Fail horribly
#ifdef DEBUG
            Serial.println("Get Terminal called but OS not Set");
#endif
            exit(1);
            break;
    }
    delayFor(defaultDelay);
}
void DuckyParser::getTerminalLinux() {
    runProgramLinux("terminal");
//    char keys[4] = { (char)KEY_LEFT_ALT, (char)KEY_LEFT_CTRL, 't', '\0' };
//    sendCombination(keys);
//    delayFor(3 * defaultDelay);
}
void DuckyParser::getTerminalWindows() {
    runProgramWindows("powershell");
}
void DuckyParser::getTerminalOSX() {
    runProgramOSX("terminal");
}

/*
 * UAC run as admin prompt does not focus by default
 * This lets you programmatically select and hit allow on the window
 */
void DuckyParser::windowsUACShenanigans() {
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

void DuckyParser::pressEnter() {
    sendKey(KEY_RETURN);
//    Keyboard.press(KEY_RETURN);
//    Keyboard.releaseAll();
}

void DuckyParser::sendKey(char c) {
#ifdef DEBUG
    Serial.print(c, HEX);
#endif
    KEYBOARD_ON && Keyboard.write(c); // Write is press + release
}
void DuckyParser::sendInput(const String &input) {
#ifdef DEBUG
    Serial.print(input);
#endif
    KEYBOARD_ON && Keyboard.print(input);
}
void DuckyParser::sendLine(const String &line) {
#ifdef DEBUG
    Serial.println(line);
#endif
    KEYBOARD_ON && Keyboard.println(line);
}

void DuckyParser::sendCombination(String line) {
    int keyCount = 1, cKey = 0;
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
    keys[cKey] = '\0';
#ifdef DEBUG
    Serial.print("Read ");
    Serial.print(cKey);
    Serial.println(" total");
#endif
    sendCombination(keys);
    free(keys);
}

// Offt horrible #ifdef
void DuckyParser::sendCombination(char *keys) {
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
    delayFor(200);
    /* KEYBOARD_ON && */ Keyboard.releaseAll();
}
