#include "Authorization.h"
Authorization::Authorization(const char* jsonData) {
  _json = jsonData;
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