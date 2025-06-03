#ifndef DISPLAY_H
#define DISPLAY_H

#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>

class Display {
  private:
    LiquidCrystal_I2C lcd;
    MFRC522 mfrc522;

  public:
    Display(const int& SS_PIN, const int& RST_PIN);  // Konstruktor ;

    void clear();
    void writeInLcd(const int& positionlength, const int& positionHeight, const String& message);
    void greeting(const String& vorname);
    void pinCorrect(const String& vorname);
    void pinIncorrect();
};

#endif
