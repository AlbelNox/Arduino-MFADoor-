#include <SPI.h>
#include <MFRC522.h>
#include <ArduinoJson.h>
#include <string.h>
#include "Authorization.h"
#include "Display.h"
#include "KeypadManager.h"

#define SS_PIN 10           // RFID SDA
#define RST_PIN 9           // RFID Reset
#define RED_LED_PIN 7       
#define GREEN_LED_PIN 6     
#define RELAY_PIN 8         // Türschloss (Pin8 - IN); GND - GND; VCC - 5V / Schloss: Mitte & Li/Re to test

//---------------------
//---Functions & Methods deklaration---
//---------------------
void resetAfterCheck();
void SerialPrintUidForAdding(const String& uidStr);
void addingMember(const String& uidStr);

Display display(SS_PIN, RST_PIN); //SS_PIN, RST_PIN
//---------------------
//---Granted UIDs---
//---------------------
// JSON-Daa as constant String (RAW = no escape characters needed)
//{ "uid": "3D98C501", "vorname": "Albel", "nachname": "Nox", "pin": "1234" },
const char* jsonUIDs = R"rawliteral(
[
  { "uid": "C5C73603", "vorname": "Mikel 2", "nachname": "Thiele", "pin": "3004" },
  { "uid": "FC3F1B49", "vorname": "Bahareh", "nachname": "Janott", "pin": "2525" },
  { "uid": "A3081D06", "vorname": "Bea", "nachname": "Janott", "pin": "7559" }
]
)rawliteral";

//Authorization::Authorization auth(jsonUIDs);

//---------------------
//---keypad---
//---------------------
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns

// pin assignment
//orange:   A0  yellow:   A1  green:    A2  blue:     A3
//violett:   2  black:     3  white:     4  grey:      5
byte rowPins[ROWS] = {A0, A1, A2, A3}; // Zeilen
byte colPins[COLS] = {2, 3, 4, 5};     // Spalten

KeypadManager keypadManager(rowPins, colPins, display);
String pinCode = "";
//RFID
MFRC522 mfrc522(SS_PIN, RST_PIN);

Authorization auth(jsonUIDs, &display, &keypadManager);
//---------------------
//---functions---
//---------------------
void resetAfterCheck() {
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RELAY_PIN, LOW);  // lock closing
  display.clear();
  display.writeInLcd(0, 0, "Ready for Check!");
}

//to add new RFIDs
void SerialPrintUidForAdding(const String& uidStr) {
  Serial.print("Gelesene UID: ");
  Serial.println(uidStr);
  display.clear();
}

// For adding New Rfids
void addingMember(const String& uidStr){
SerialPrintUidForAdding(uidStr); 
}

String readRFIDKey() {
  String uidStr = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    byte b = mfrc522.uid.uidByte[i];
    if (b < 0x10) uidStr += "0";
    uidStr += String(b, HEX);
  }
  return uidStr;
}

void setup() {
  Serial.begin(9600);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);
  // Startzustand: Schloss geschlossen (Relay aus)
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  display.writeInLcd(0, 0, "Ready for Check!");
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  String uidStr = readRFIDKey();
  uidStr.toUpperCase();
  auth.checkRfid(uidStr);
  resetAfterCheck();

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
