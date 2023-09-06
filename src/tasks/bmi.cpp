// #include "bmi.h"
// #include "SparkFun_BMI270_Arduino_Library.h"

// #include "Wire.h"

// BMI270 imu;
// uint8_t i2cAddress = BMI2_I2C_SEC_ADDR; // 0x69
// extern SemaphoreHandle_t mutex;


// void bmiSetup() {
//     // Check if sensor is connected and initialize
//     // Address is optional (defaults to 0x68)
//     xSemaphoreTake(mutex, portMAX_DELAY);
//     while(imu.beginI2C(i2cAddress) != BMI2_OK)
//     {
//         // Wait a bit to see if connection is established
//         delay(1000);
//     }
//     xSemaphoreGive(mutex);
// }

// void bmiTask(void *parameter) {
//     while (true) {
//         // Get measurements from the sensor. This must be called before accessing
//         // the sensor data, otherwise it will never update
//         xSemaphoreTake(mutex, portMAX_DELAY);
//         int8_t err = imu.getSensorData();
//         xSemaphoreGive(mutex);
//     }
// }