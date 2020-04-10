#include "Keyboard.h"
#include "specialKeys.h"

//#define DEBUG

// Absolute filth.
char specialKeys::keyToCode(const String &key) {
    // If it's not a key string, it's a single character
    char code = '\0';
    if (key == "KEY_SPACE")
        code = 0x32; // Easier than handling ' ' explicitly in a script
    else if (key == "KEY_LEFT_CTRL")
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
    else if (key == "KEY_RETURN" || key == "KEY_ENTER") // More likely case
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

#ifdef DEBUG
        code != '\0' && Serial.print("Command Key Found: ");
        code == '\0' && Serial.print("Key Found: ");
        code == '\0' && (code = key.charAt(0));
        Serial.print(code, HEX);
        Serial.println();
#endif

    // Slight duplication with the above for debugging
    if (code == '\0')
        code = key.charAt(0);
    return code;
}
