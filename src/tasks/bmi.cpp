#include "bmi.h"
#include "SparkFun_BMI270_Arduino_Library.h"

#include "Wire.h"

BMI270 imu;
uint8_t i2cAddress = BMI2_I2C_SEC_ADDR; // 0x69


void bmiSetup() {
    // Check if sensor is connected and initialize
    // Address is optional (defaults to 0x68)
    while(imu.beginI2C(i2cAddress) != BMI2_OK)
    {
        // Not connected, inform user
        Serial.println("Error: BMI270 not connected, check wiring and I2C address!");

        // Wait a bit to see if connection is established
        delay(1000);
    }

    Serial.println("BMI270 connected!");
}

void bmiTask(void *parameter) {
    while (true) {
        // Get measurements from the sensor. This must be called before accessing
        // the sensor data, otherwise it will never update
        int8_t err = imu.getSensorData();

        if (err < 0)
            Serial.println("Error!!");
        else if (err > 0)
            Serial.println("Warning!!");

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
        delay(100);
    }
}