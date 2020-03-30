#ifndef _BLINK_H_
#define _BLINK_H_

class Blink {
public:
    static void blinkError();
    static void blinkDone();
    static void blinkStarting();
    static void blink(String green_led, String orange_led, int step);
private:
    Blink();
};


#endif //_BLINK_H_
