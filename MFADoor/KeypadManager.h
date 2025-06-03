#ifndef KEYPADMANAGER_H
#define KEYPADMANAGER_H

#include <Keypad.h>
#include "Display.h"

class KeypadManager {
  private:
    static const byte ROWS = 4;
    static const byte COLS = 4;

    char hexaKeys[ROWS][COLS] = {
      {'D','C','B','A'},
      {'#','9','6','3'},
      {'0','8','5','2'},
      {'*','7','4','1'}
    };

    byte rowPins[ROWS];
    byte colPins[COLS];

    Keypad keypad;
    Display& display;

  public:
    KeypadManager(byte rowPinsInit[ROWS], byte colPinsInit[COLS], Display& displayRef);
    String keypadInput(const String& vorname);
};

#endif
