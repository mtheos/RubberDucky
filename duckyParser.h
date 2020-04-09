#ifndef _DUCKY_PARSER_H_
#define _DUCKY_PARSER_H_

#include "SD.h"
#include "osType.h"

class DuckyParser {
public:
    DuckyParser();
    explicit DuckyParser(OSType os);
    int execute(File f);
    int executeLine(const String &line);
    bool logging = false;
    OSType osType = OSType::Unknown;
    int defaultDelay = 100;
private:
    int currentLine = 0;
    File file;
    void setOS(const String &os);
    void runProgram(const String &programName);
    void runProgramLinux(const String &programName);
    void runProgramWindows(const String &programName);
    void runProgramOSX(const String &programName);
    void getTerminal();
    void getTerminalLinux();
    void getTerminalWindows();
    void getTerminalOSX();
    void windowsUACShenanigans();
    void delayFor(int time);
    void sendLine(const String &line);
    void sendInput(const String &input);
    void sendCombination(char *keys);
    void sendCombination(String line);
    void sendKey(char c);
    void pressEnter();
};

#endif //_DUCKY_PARSER_H_
