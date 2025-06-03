#include "Display.h"

Display::Display(const int& SS_PIN, const int& RST_PIN) //const int& SS_PIN, const int& RST_PIN
  : lcd(0x27, 20, 4),  // LCD-Adresse und Größe
    mfrc522(SS_PIN, RST_PIN)
{
  lcd.init();
  lcd.backlight();
  SPI.begin();
  mfrc522.PCD_Init();
}
void Display::clear(){
  lcd.clear();
}

void Display::writeInLcd(const int& positionLength, const int& positionHeight, const String& message) {
  lcd.setCursor(positionLength, positionHeight);
  lcd.print(message);
}

void Display::greeting(const String& vorname) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Hallo, " + vorname + "!");
}

void Display::pinCorrect(const String& vorname) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PIN korrekt,");
  lcd.setCursor(0, 1);
  lcd.print(vorname);
}

void Display::pinIncorrect() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Falscher PIN!");
}
