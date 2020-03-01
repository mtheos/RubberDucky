#include "Keyboard.h"

void testKeyboard() {
  Keyboard.begin();
  delay(3000); // No intelligent way to wait for HID afaik
  Keyboard.press(KEY_LEFT_GUI);
  delay(300);
  Keyboard.releaseAll();
  delay(300);
  Keyboard.println("sublime");
  delay(500);
  Keyboard.press(KEY_LEFT_CTRL);
  Keyboard.press(KEY_LEFT_SHIFT);
  Keyboard.press(KEY_RETURN);
  Keyboard.releaseAll();
  delay(800);
  Keyboard.press(KEY_LEFT_ARROW);
  Keyboard.release(KEY_LEFT_ARROW);
  delay(500);
  Keyboard.press(KEY_RETURN);
  Keyboard.release(KEY_RETURN);
  delay(500);
  Keyboard.print("Payload");
  delay(100);
  Keyboard.press(KEY_RETURN);
  Keyboard.release(KEY_RETURN);
  delay(100);
  Keyboard.releaseAll();
}
