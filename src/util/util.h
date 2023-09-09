#include "Arduino.h"

#define GRAVITY_EARTH  (9.80665f)

#define MILLIS_TO_S(x) ((float)(x) / 1000.0f)

float gToMps2(float g);

float mpsToKmh(float mps);