#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Buttons
#define START_BTN   A2
#define SELECT_BTN  A3

// LED and buzzer
#define GOOD_LED A0
#define BAD_LED  A1
#define BUZZER   13

// IC socket pins connected to Arduino
#define P1  2
#define P2  3
#define P3  4
#define P4  5
#define P5  6
#define P6  7
#define P9  8
#define P10 9
#define P11 10
#define P12 11
#define P13 12

int icSelect = 0;

String icName[] = {
  "7400 NAND",
  "7402 NOR",
  "7404 NOT",
  "7408 AND",
  "7432 OR",
  "7486 XOR"
};

void setup() {
  lcd.init();
  lcd.backlight();

  pinMode(START_BTN, INPUT);
  pinMode(SELECT_BTN, INPUT);

  pinMode(GOOD_LED, OUTPUT);
  pinMode(BAD_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  showIC();
}

void loop() {
  if (digitalRead(SELECT_BTN) == HIGH) {
    icSelect++;
    if (icSelect > 5) icSelect = 0;
    showIC();
    delay(300);
  }

  if (digitalRead(START_BTN) == HIGH) {
    lcd.clear();
    lcd.print("Testing...");
    delay(500);

    bool result = false;

    if (icSelect == 0) result = test7400();
    if (icSelect == 1) result = test7402();
    if (icSelect == 2) result = test7404();
    if (icSelect == 3) result = test7408();
    if (icSelect == 4) result = test7432();
    if (icSelect == 5) result = test7486();

    showResult(result);
    delay(2000);
    showIC();
  }
}

void showIC() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Select IC:");
  lcd.setCursor(0, 1);
  lcd.print(icName[icSelect]);
}

void showResult(bool good) {
  lcd.clear();

  if (good) {
    lcd.print("IC GOOD");
    digitalWrite(GOOD_LED, HIGH);
    digitalWrite(BAD_LED, LOW);
    tone(BUZZER, 1000, 200);
  } else {
    lcd.print("IC BAD");
    digitalWrite(GOOD_LED, LOW);
    digitalWrite(BAD_LED, HIGH);
    tone(BUZZER, 300, 500);
  }

  delay(1000);
  digitalWrite(GOOD_LED, LOW);
  digitalWrite(BAD_LED, LOW);
}

// Two-input gate testing function
bool testGate(int in1, int in2, int out, int gateType) {
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(out, INPUT);

  for (int a = 0; a <= 1; a++) {
    for (int b = 0; b <= 1; b++) {
      digitalWrite(in1, a);
      digitalWrite(in2, b);
      delay(10);

      int expected = 0;

      if (gateType == 0) expected = !(a & b);  // NAND
      if (gateType == 1) expected = !(a | b);  // NOR
      if (gateType == 2) expected = (a & b);   // AND
      if (gateType == 3) expected = (a | b);   // OR
      if (gateType == 4) expected = (a ^ b);   // XOR

      int actual = digitalRead(out);

      if (actual != expected) {
        return false;
      }
    }
  }

  return true;
}

// 7400 NAND
bool test7400() {
  return testGate(P1, P2, P3, 0) &&
         testGate(P4, P5, P6, 0) &&
         testGate(P12, P13, P11, 0);
}

// 7402 NOR
bool test7402() {
  return testGate(P2, P3, P1, 1) &&
         testGate(P5, P6, P4, 1) &&
         testGate(P12, P13, P11, 1);
}

// 7408 AND
bool test7408() {
  return testGate(P1, P2, P3, 2) &&
         testGate(P4, P5, P6, 2) &&
         testGate(P12, P13, P11, 2);
}

// 7432 OR
bool test7432() {
  return testGate(P1, P2, P3, 3) &&
         testGate(P4, P5, P6, 3) &&
         testGate(P12, P13, P11, 3);
}

// 7486 XOR
bool test7486() {
  return testGate(P1, P2, P3, 4) &&
         testGate(P4, P5, P6, 4) &&
         testGate(P12, P13, P11, 4);
}

// 7404 NOT
bool testNOT(int inputPin, int outputPin) {
  pinMode(inputPin, OUTPUT);
  pinMode(outputPin, INPUT);

  digitalWrite(inputPin, LOW);
  delay(10);
  if (digitalRead(outputPin) != HIGH) return false;

  digitalWrite(inputPin, HIGH);
  delay(10);
  if (digitalRead(outputPin) != LOW) return false;

  return true;
}

bool test7404() {
  return testNOT(P1, P2) &&
         testNOT(P3, P4) &&
         testNOT(P5, P6) &&
         testNOT(P11, P10) &&
         testNOT(P13, P12);
}
