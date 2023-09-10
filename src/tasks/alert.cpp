#include "Arduino.h"
#include "alert.h"
#include "defs.h"

extern boolean alert;

static void alertLoop();

void alertTask(void * pvParameters) {
    while (true) {
        if (alert) {
            alertLoop();
        }
        delay(300);
    }
}

static void alertLoop() {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(300);
    digitalWrite(BUZZER_PIN, LOW);
    delay(300);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(300);
    digitalWrite(BUZZER_PIN, LOW);
}