#include <ArduinoJson.h>
#include "Display.h"
#include "KeypadManager.h"

class Authorization {
  public:
    Authorization(const char* jsonData, Display* display, KeypadManager* keypadManager);

    bool isUIDAuthorized(const String& uid);
    bool isPinCorrect(const String& uid, const String& pin);
    String getNameFromUID(const String& uid);
    void checkRfid(const String& uidStr);
    void checkPin(const String& uidStr, const String& pin, const String& vorname);

  private:
    const char* _json;
    Display* _display;  // Zeiger auf Display-Instanz
    KeypadManager* _keypadManager;
};