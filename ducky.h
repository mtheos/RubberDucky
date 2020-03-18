#ifndef _DUCKY_H_
#define _DUCKY_H_

#include "SD.h"
#include "osType.h"

class Ducky {
public:
    Ducky();
    explicit Ducky(OSType t);
    void execute(File f);
    void executeLine(const String &line);
    bool logging = false;
    OSType osType = OSType::Linux;
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
    char commandToCode(const String &key);
    void sendLine(const String &line);
    void sendInput(const String &input);
    void sendCombination(char *keys);
    void sendCombination(String line);
    void sendKey(char c);
    void pressEnter();
};

#endif //_DUCKY_H_
