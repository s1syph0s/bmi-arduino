#include "bmi.h"
#include "SparkFun_BMI270_Arduino_Library.h"
#include "util/util.h"

#include "Wire.h"

BMI270 imu;
uint8_t i2cAddress = BMI2_I2C_SEC_ADDR; // 0x69
extern SemaphoreHandle_t i2cMutex;

displacementData bmiReading = {.currSpeed = 0, .distTravelled = 0, .millisBefore = 0};

void bmiSetup() {
    // Check if sensor is connected and initialize
    // Address is optional (defaults to 0x68)
    xSemaphoreTake(i2cMutex, portMAX_DELAY);
    while(imu.beginI2C(i2cAddress) != BMI2_OK)
    {
        // Wait a bit to see if connection is established
        delay(1000);
    }
    xSemaphoreGive(i2cMutex);
}

void bmiTask(void *parameter) {
    while (true) {
        // Get measurements from the sensor. This must be called before accessing
        // the sensor data, otherwise it will never update
        xSemaphoreTake(i2cMutex, portMAX_DELAY);
        int8_t err = imu.getSensorData();
        xSemaphoreGive(i2cMutex);
    }
}

static void evaluateRawData() {
    uint32_t currMillis = imu.data.sensorTimeMillis;
    if (currMillis - bmiReading.millisBefore < 0) {
        return;
    }
    float accelZ = imu.data.accelZ; // Z in G
    accelZ = gToMps2(accelZ); // Z in mps2
    bmiReading.currSpeed = calculateCurrSpeed(bmiReading.currSpeed, accelZ, MILLIS_TO_S(currMillis));
    bmiReading.distTravelled = calculateDistTravelled(bmiReading.distTravelled, accelZ, MILLIS_TO_S(currMillis));
}


/*
* AccelZ in mps2
* currSpeed in mps
* timeS in s
*/
static float calculateCurrSpeed(float currSpeed, float accelZ, float timeS) {
    return currSpeed + accelZ * timeS;
}

/*
* AccelZ in mps2
* currDistTravelled in m
* timeS in s
*/
static float calculateDistTravelled(float currDistTravelled, float accelZ, float timeS) {
    return currDistTravelled * timeS + (accelZ * pow(timeS, 2)) / 2;
}