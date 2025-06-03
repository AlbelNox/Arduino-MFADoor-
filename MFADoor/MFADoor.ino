#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ArduinoJson.h>
#include <Keypad.h>
#include "Authorization.h"

#define SS_PIN 10           // RFID SDA
#define RST_PIN 9           // RFID Reset
#define RED_LED_PIN 7       
#define GREEN_LED_PIN 6     
#define RELAY_PIN 8         // Türschloss (Pin8 - IN); GND - GND; VCC - 5V / Schloss: Mitte & Li/Re to test

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

Authorization auth(jsonUIDs);

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

char hexaKeys[ROWS][COLS] = {
  {'D','C','B','A'},
  {'#','9','6','3'},
  {'0','8','5','2'},
  {'*','7','4','1'}
};

Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
String pinCode = "";
//---------------------
//---lcd Display---
//---------------------

LiquidCrystal_I2C lcd(0x27, 20, 4); 
MFRC522 mfrc522(SS_PIN, RST_PIN);

//---------------------
//---functions---
//---------------------
void writeInLcd(const int& positionlength, const int& positionHight, const String& message) {
  lcd.setCursor(positionlength, positionHight);
  lcd.print(message);
}

void greeting(const String& vorname) {
  lcd.clear();
  digitalWrite(GREEN_LED_PIN, HIGH);
  delay(500);
  writeInLcd(0, 1, "Bitte Pin eingeben!");
  writeInLcd(0, 0, "Hallo " + vorname +",");
}

void resetAfterCheck() {
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RELAY_PIN, LOW);  // lock closing
  lcd.clear();
  writeInLcd(0, 0, "Ready for Check!");
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

//to add new RFIDs
void SerialPrintUidForAdding(const String& uidStr) {
  Serial.print("Gelesene UID: ");
  Serial.println(uidStr);
  lcd.clear();
}

String keypadInput(const String& vorname) {
  unsigned long startTime = millis();
  unsigned long timeout = 10000; // 10 seconds for Input

  while (millis() - startTime < timeout) {
    char customKey = keypad.getKey();
    if (customKey) {
    // optional: * or # for reseting the input
      if (customKey == '*' || customKey == '#') {
        writeInLcd(0, 3, "Eingabe geloescht");
        delay(1000);
        writeInLcd(0, 3, "                        ");
        greeting(vorname);
        pinCode = "";
        return pinCode;
      }
      // process only numbers
      if ((customKey >= '0' && customKey <= '9') ||
          customKey == 'A' || customKey == 'B' ||
          customKey == 'C' || customKey == 'D') {
        pinCode += customKey;

        if (pinCode.length() == 4) {
          Serial.print("Eingegebener PIN: "); //for testing
          Serial.println(pinCode);            //for testing
          return pinCode;
        }
      }
    }
  }
  pinCode = "";
  return pinCode; //for timeout
}


void pinCorrect(const String& vorname){
lcd.clear();
      writeInLcd(0, 0, "Zugang gewaehrt!");
      writeInLcd(0, 2, "Willkommen,");
      writeInLcd(0, 3, vorname);
      openDoor(2000);
}

void pinIncorrect(){
  lcd.clear();
  writeInLcd(0, 0, "Falscher PIN!");
  writeInLcd(0, 1, "Zugang verweigert");
  digitalWrite(RED_LED_PIN, HIGH);
  delay(3000);
}

// For adding New Rfids
void addingMember(const String& uidStr){
SerialPrintUidForAdding(uidStr); 
}

void checkRfid(const String& uidStr){
  if (auth.isUIDAuthorized(uidStr)) {
    String vorname = auth.getNameFromUID(uidStr);
    greeting(vorname);
    String pin = keypadInput(vorname);
    
    if(pin.length() == 4){
      if(auth.isPinCorrect(uidStr, pin)){
        pinCorrect(vorname);
      } else{
        pinIncorrect();
      }
      //checkPin(uidStr, pin, vorname);
    }
  } else {
    pinIncorrect();
  }
}

void checkPin(const String& uidStr, const String& pin, const String& vorname){
  bool pinIsCorrect = auth.isPinCorrect(uidStr, pin);
    if(pinIsCorrect){
      pinCorrect(vorname);
    } else{
      pinIncorrect();
    }
}

void openDoor(int duration) {
  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(6, HIGH);
  delay(duration);
  digitalWrite(6, LOW);
  digitalWrite(RELAY_PIN, LOW);
}

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  lcd.init();
  lcd.backlight();

  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);
  // Startzustand: Schloss geschlossen (Relay aus)
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  writeInLcd(0, 0, "Ready for Check!");
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  String uidStr = readRFIDKey();
  uidStr.toUpperCase();
  checkRfid(uidStr);
  resetAfterCheck();

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
