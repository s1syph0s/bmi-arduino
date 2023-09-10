#include "bmi.h"
#include "SparkFun_BMI270_Arduino_Library.h"
#include "util/util.h"

#include "Wire.h"

#define ALERT_THRESHOLD 0.8

BMI270 imu;
uint8_t i2cAddress = BMI2_I2C_SEC_ADDR; // 0x69

extern SemaphoreHandle_t i2cMutex;
extern SemaphoreHandle_t alertMutex;

extern boolean alert;

struct alertTiming {
    boolean startMeasuring;
    uint32_t start;
    uint32_t current;
};

alertTiming bmiTiming;
static void evaluateAlert();
static void resetBmiTiming();

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
    resetBmiTiming();
}

void bmiTask(void *parameter) {
    while (true) {
        // Get measurements from the sensor. This must be called before accessing
        // the sensor data, otherwise it will never update
        xSemaphoreTake(i2cMutex, portMAX_DELAY);
        int8_t err = imu.getSensorData();
        xSemaphoreGive(i2cMutex);
        evaluateAlert();
        delay(500);
    }
}

static void evaluateAlert() {
    if (abs(imu.data.accelZ) >= ALERT_THRESHOLD && !bmiTiming.startMeasuring) {
        bmiTiming.startMeasuring = true;
        bmiTiming.start = imu.data.sensorTimeMillis;
        bmiTiming.current = imu.data.sensorTimeMillis;
        Serial1.println("Start measuring");
    }

    if (abs(imu.data.accelZ) >= ALERT_THRESHOLD && bmiTiming.startMeasuring) {
        bmiTiming.current = imu.data.sensorTimeMillis;
        if (bmiTiming.current-bmiTiming.start > 3000) {
            xSemaphoreTake(alertMutex, portMAX_DELAY);
            alert = true;
            xSemaphoreGive(alertMutex);
            resetBmiTiming();
        }
    } else if (abs(imu.data.accelZ) < ALERT_THRESHOLD && bmiTiming.startMeasuring) {
        resetBmiTiming();
    }
}

static void resetBmiTiming() {
    bmiTiming = {.startMeasuring = false, .start = 0, .current = 0};
}