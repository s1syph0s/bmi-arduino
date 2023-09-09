#include "util.h"

float gToMps2(float g) {
    return g * GRAVITY_EARTH;
}

float mpsToKmh(float mps) {
    return mps * 3.6;
}