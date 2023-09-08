#include "Arduino.h"
#include "Wire.h"

#include "tasks/nfc.h"
#include "tasks/bmi.h"
#include "tasks/ultrasonic.h"
#include "defs.h"
#include "SparkFun_BMI270_Arduino_Library.h"


boolean authorized = false;
boolean notAuthorized = false;
boolean authChanged = false;

float distanceCm = 0;

enum TurvaloState {locked, unlocked};

TurvaloState turvaloState;

static void buzzerAuthChange();
static void buzzerNotAuth();
static void printBmiData();

SemaphoreHandle_t mutex;

extern BMI270 imu;

void setup() {
  Wire.begin(2, 1);
  Serial1.begin(512000, SERIAL_8N1, 12, 13);

  Serial1.println("Turvalo test");

  mutex = xSemaphoreCreateMutex();

  NfcSetup();
  UltrasonicSetup();
  bmiSetup();
  

  // Buzzer setup
  pinMode(BUZZER_PIN, OUTPUT);
  delay(1000);

  xTaskCreate(UltrasonicTask, "UltrasonicTask", 2048, NULL, 1, NULL);
  xTaskCreate(NfcTask, "NfcTask", 2048, NULL, 1, NULL);
  xTaskCreate(bmiTask, "BmiTask", 2048, NULL, 1, NULL);
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
  } else if (notAuthorized) {
    Serial1.println("Not authorized!");
    buzzerNotAuth();
    xSemaphoreTake(mutex, portMAX_DELAY);
    notAuthorized = false;
    xSemaphoreGive(mutex);
  }
  delay(500);

  Serial1.print("Distance (cm): ");
  Serial1.println(distanceCm);
  printBmiData();
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

static void buzzerNotAuth() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(300);
  digitalWrite(BUZZER_PIN, LOW);
  delay(300);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(300);
  digitalWrite(BUZZER_PIN, LOW);
}

static void printBmiData() {
  // Print acceleration data
  Serial1.print("Acceleration in g's");
  Serial1.print("\t");
  Serial1.print("X: ");
  Serial1.print(imu.data.accelX, 3);
  Serial1.print("\t");
  Serial1.print("Y: ");
  Serial1.print(imu.data.accelY, 3);
  Serial1.print("\t");
  Serial1.print("Z: ");
  Serial1.println(imu.data.accelZ, 3);
}