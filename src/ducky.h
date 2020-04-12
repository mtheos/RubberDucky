#ifndef _DUCKY_H_
#define _DUCKY_H_

#include "SD.h"
#include "osType.h"

class Ducky {
public:
    static void run();
private:
    Ducky();
    static void initializeGPIO();
    static String readGPIO();
    static void error(const String &msg);
};

#endif //_DUCKY_H_
