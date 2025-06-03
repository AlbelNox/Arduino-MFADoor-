#include "KeypadManager.h"

KeypadManager::KeypadManager(byte rowPinsInit[ROWS], byte colPinsInit[COLS], Display& displayRef)
  : rowPins{rowPinsInit[0], rowPinsInit[1], rowPinsInit[2], rowPinsInit[3]},
    colPins{colPinsInit[0], colPinsInit[1], colPinsInit[2], colPinsInit[3]},
    keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS),
    display(displayRef)
{}

String KeypadManager::keypadInput(const String& vorname) {
  String pinCode = "";
  unsigned long startTime = millis();
  unsigned long timeout = 10000;

  while (millis() - startTime < timeout) {
    char customKey = keypad.getKey();
    if (customKey) {
      if (customKey == '*' || customKey == '#') {
        display.writeInLcd(0, 3, "Eingabe geloescht");
        delay(1000);
        display.writeInLcd(0, 3, "                    ");
        display.greeting(vorname);
        return "";
      }
      if ((customKey >= '0' && customKey <= '9') ||
          customKey == 'A' || customKey == 'B' ||
          customKey == 'C' || customKey == 'D') {
        pinCode += customKey;
        if (pinCode.length() == 4) {
          Serial.print("Eingegebener PIN: ");
          Serial.println(pinCode);
          return pinCode;
        }
      }
    }
  }
  return ""; // timeout
}
