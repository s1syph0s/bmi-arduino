#include "ultrasonic.h"
#include "defs.h"
#include "Arduino.h"


// cm/us
#define SOUND_SPEED 0.034

long duration;
extern float distanceCm;

void UltrasonicSetup() {
    pinMode(ULTR_TRIG, OUTPUT);
    pinMode(ULTR_ECHO, INPUT);
}

void UltrasonicTask(void *parameter) {
    while (true) {
        digitalWrite(ULTR_TRIG, LOW);
        delayMicroseconds(2);
        digitalWrite(ULTR_TRIG, HIGH);
        delayMicroseconds(10);
        digitalWrite(ULTR_TRIG, LOW);

        duration = pulseIn(ULTR_ECHO, HIGH);
        distanceCm = duration * SOUND_SPEED / 2;
    }
    
}