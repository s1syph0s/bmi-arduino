// --------------------------------------
// i2c_scanner
//
// Modified from https://playground.arduino.cc/Main/I2cScanner/
// --------------------------------------
#include "Arduino.h"
#include "Wire.h"

#include "tasks/nfc.h"
#include "defs.h"

boolean authorized = false;
boolean authChanged = false;

String getLocalUidString(byte *_uid, unsigned int _uidLength)
{
    String uidString = "";
    for (unsigned int i = 0; i < _uidLength; i++)
    {
        if (i > 0)
        {
            uidString += " ";
        }

        if (_uid[i] < 0xF)
        {
            uidString += "0";
        }

        uidString += String((unsigned int)_uid[i], (unsigned char)HEX);
    }
    uidString.toUpperCase();
    return uidString;
}

void setup() {
  Wire.begin(2, 1);
  Serial1.begin(9600, SERIAL_8N1, 12, 13);

  Serial1.println("Turvalo test");
  NfcSetup();

  // Buzzer setup
  pinMode(BUZZER_PIN, OUTPUT);
  delay(1000);

  xTaskCreate(NfcTask, "NfcTask", 2048, NULL, 1, NULL);
}

void loop() {
  Serial1.println(authChanged);
  if (authorized && authChanged) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
    delay(200);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
    authChanged = false;
  }
  delay(100);
}