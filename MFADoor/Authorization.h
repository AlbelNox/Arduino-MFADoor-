#ifndef AUTHORIZATION_H
#define AUTHORIZATION_H

#include <ArduinoJson.h>

class Authorization {
  public:
    Authorization(const char* jsonData);

    bool isUIDAuthorized(const String& uid);
    bool isPinCorrect(const String& uid, const String& pin);
    String getNameFromUID(const String& uid);
  
  private:
    const char* _json;
};

#endif