#include "Authorization.h"
#include "Display.h"
#include "KeypadManager.h"

Authorization::Authorization(const char* jsonData, Display* display, KeypadManager* keypadManager) {
  _json = jsonData;
  _display = display;
  _keypadManager = keypadManager;
}

bool Authorization::isUIDAuthorized(const String& uid) {
  StaticJsonDocument<512> doc;
  if (deserializeJson(doc, _json)) return false;

  for (JsonObject entry : doc.as<JsonArray>()) {
    if (uid == entry["uid"].as<String>()) {
      return true;
    }
  }
  return false;
}

bool Authorization::isPinCorrect(const String& uid, const String& pin) {
  StaticJsonDocument<512> doc;
  if (deserializeJson(doc, _json)) return false;

  for (JsonObject entry : doc.as<JsonArray>()) {
    if (uid == entry["uid"].as<String>()) {
      return pin == entry["pin"].as<String>();
    }
  }
  return false;
}

String Authorization::getNameFromUID(const String& uid) {
  StaticJsonDocument<512> doc;
  if (deserializeJson(doc, _json)) return "";

  for (JsonObject entry : doc.as<JsonArray>()) {
    if (uid == entry["uid"].as<String>()) {
      return entry["vorname"].as<String>();
    }
  }
  return "";
}

void Authorization::checkRfid(const String& uidStr){
  if (isUIDAuthorized(uidStr)) {
    String vorname = getNameFromUID(uidStr);
    _display->greeting(vorname);
    String pin = _keypadManager->keypadInput(vorname);
    if(pin.length() == 4){
    checkPin(uidStr, pin, vorname);
    } else {
      _display->pinIncorrect();
    }
  }
}

void Authorization::checkPin(const String& uidStr, const String& pin, const String& vorname){
  bool pinIsCorrect = isPinCorrect(uidStr, pin);
    if(pinIsCorrect){
      _display->pinCorrect(vorname);
    } else{
      _display->pinIncorrect();
    }
}