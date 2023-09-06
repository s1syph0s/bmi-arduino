// --------------------------------------
// i2c_scanner
//
// Modified from https://playground.arduino.cc/Main/I2cScanner/
// --------------------------------------
#include "Arduino.h"
#include "Wire.h"

#include "tasks/nfc.h"
#include "tasks/ultrasonic.h"
#include "defs.h"

boolean authorized = false;
boolean authChanged = false;

float distanceCm = 0;

enum TurvaloState {locked, unlocked};

TurvaloState turvaloState;

static void buzzerAuthChange();

void setup() {
  Wire.begin(2, 1);
  Serial1.begin(9600, SERIAL_8N1, 12, 13);

  Serial1.println("Turvalo test");
  NfcSetup();
  UltrasonicSetup();

  // Buzzer setup
  pinMode(BUZZER_PIN, OUTPUT);
  delay(1000);

  xTaskCreate(NfcTask, "NfcTask", 2048, NULL, 1, NULL);
  xTaskCreate(UltrasonicTask, "UltrasonicTask", 2048, NULL, 1, NULL);
}

void loop() {
  if (authorized && authChanged) {
    Serial1.println("Authorized.");
    buzzerAuthChange();
    authChanged = false;
  } else if (!authorized && authChanged) {
    Serial1.println("Locked.");
    buzzerAuthChange();
    authChanged = false;
  }
  delay(100);

  Serial1.print("Distance (cm): ");
  Serial1.println(distanceCm);
}

static void buzzerAuthChange() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
  delay(200);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
}