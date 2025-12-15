// import required libraries
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <stdbool.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int RS = 12, EN = 11, D4 = 5, D5 = 4, D6 = 3, D7 = 2;
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

// Keypad matrix pins
const byte ROWS = 4;  //four rows
const byte COLS = 3;  //three columns
byte keypadRowPins[ROWS] = {A1, A2, A3, A4};      // Row pins
byte keypadColPins[COLS] = {6, 7, 8};           // Column pins

char keypadLayout[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

Keypad keypad = Keypad(makeKeymap(keypadLayout), keypadRowPins, keypadColPins, ROWS, COLS);

const int UPPER_GREEN_PIN = A0;
const int LOWER_GREEN_PIN = 13;
const int UPPER_RED_PIN = 9;
const int LOWER_RED_PIN = 10;

void setup() {
  // set up the LCD's number of columns and rows (smaller than original as only 4 characters are required)
  lcd.begin(4, 1);
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.autoscroll();
  
  pinMode(UPPER_GREEN_PIN, OUTPUT);
  pinMode(LOWER_GREEN_PIN, OUTPUT);
  pinMode(UPPER_RED_PIN, OUTPUT);
  pinMode(LOWER_RED_PIN, OUTPUT);
}

const int PASSWORD_LENGTH = 4;
const char expectedPassword[PASSWORD_LENGTH] = { '2', '2', '8', '8' };
char currentPassword[PASSWORD_LENGTH] = { 0 };

bool passwordCorrect = false;
bool passwordIncorrect = false;
unsigned long passwordIncorrectTimestamp = 0;

void loop() {
  if (passwordCorrect) {
    blinkCorrect();
    return;
  }
  if (passwordIncorrect) {
    blinkIncorrect();
    return;
  }
  blinkReady();
  const char key = keypad.getKey();
  if (key) {
    switch (keypad.getState()) {
      case PRESSED: {
        if (key >= '0' && key <= '9') {
          appendChar(key);
        } else if (key == '*') {
          clearPassword();
        } else if (key == '#') { 
          verifyPassword();
        }
        break;
      }
    }
  }
  delay(10);
}

void blinkCorrect() {
  digitalWrite(UPPER_RED_PIN, 0);
  digitalWrite(LOWER_RED_PIN, 0);
  if (millis() % 1000 < 500) {
    digitalWrite(UPPER_GREEN_PIN, 0);
    digitalWrite(LOWER_GREEN_PIN, 1);
  } else {
    digitalWrite(UPPER_GREEN_PIN, 1);
    digitalWrite(LOWER_GREEN_PIN, 0);
  }
}

void blinkIncorrect() {
  digitalWrite(UPPER_GREEN_PIN, 0);
  digitalWrite(LOWER_GREEN_PIN, 0);
  if (millis() % 1000 < 500) {
    digitalWrite(UPPER_RED_PIN, 0);
    digitalWrite(LOWER_RED_PIN, 1);
  } else {
    digitalWrite(UPPER_RED_PIN, 1);
    digitalWrite(LOWER_RED_PIN, 0);
  }
  if (millis() - passwordIncorrectTimestamp > 3000) {
    clearPassword();
    passwordIncorrect = false;
    digitalWrite(UPPER_RED_PIN, 0);
    digitalWrite(LOWER_RED_PIN, 0);
  }
}

void blinkReady() {
  digitalWrite(UPPER_GREEN_PIN, 1);
  digitalWrite(LOWER_GREEN_PIN, 0);
  if (millis() % 1000 < 500) {
    digitalWrite(UPPER_RED_PIN, 0);
    digitalWrite(LOWER_RED_PIN, 0);
  } else {
    digitalWrite(UPPER_RED_PIN, 0);
    digitalWrite(LOWER_RED_PIN, 1);
  }
}

void appendChar(char c) {
  for (int i = 0; i < PASSWORD_LENGTH - 1; i++) {
    currentPassword[i] = currentPassword[i + 1];
  }
  currentPassword[PASSWORD_LENGTH - 1] = c;
  lcd.write(c);
}

void clearPassword() {
  for (int i = 0; i < PASSWORD_LENGTH; i++) {
    currentPassword[i] = 0;
  }
  lcd.clear();
  lcd.setCursor(4, 0);
}

void verifyPassword() {
  digitalWrite(UPPER_RED_PIN, 0);
  digitalWrite(LOWER_RED_PIN, 0);
  digitalWrite(UPPER_GREEN_PIN, 0);
  digitalWrite(LOWER_GREEN_PIN, 0);
  bool verifiedCorrect = true;
  for (int i = 0; i < PASSWORD_LENGTH - 1; i++) {
    if (currentPassword[i] != expectedPassword[i]) {
      verifiedCorrect = false;
    }
  }
  if (verifiedCorrect) {
    passwordCorrect = true;
  } else {
    passwordIncorrect = true;
    passwordIncorrectTimestamp = millis();
  }
}
