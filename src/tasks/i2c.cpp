#include "i2c.h"
#include "Wire.h"

#include "bmi.h"
#include "SparkFun_BMI270_Arduino_Library.h"

#include "PN532_I2C.h"
#include "PN532.h"
#include "NfcAdapter.h"
#include "nfc.h"
#include "defs.h"

BMI270 imu;
uint8_t i2cAddress = BMI2_I2C_SEC_ADDR; // 0x69
extern SemaphoreHandle_t mutex;

PN532_I2C pn532i2c(Wire);
NfcAdapter nfc(pn532i2c);
byte uid[7];
byte whiteListUid[4] = {0x53, 0x18, 0x46, 0xFA};

extern HardwareSerial Serial1;
extern TwoWire Wire;
extern boolean authorized;
extern boolean authChanged;

static void nfcTask();

void bmiSetup() {
    // Check if sensor is connected and initialize
    // Address is optional (defaults to 0x68)
    xSemaphoreTake(mutex, portMAX_DELAY);
    while(imu.beginI2C(i2cAddress) != BMI2_OK)
    {
        // Wait a bit to see if connection is established
        delay(1000);
    }
    xSemaphoreGive(mutex);
}

void NfcSetup()
{
    xSemaphoreTake(mutex, portMAX_DELAY);
    nfc.begin();
    xSemaphoreGive(mutex);
}

void i2cTask(void *pvParameters) {
    uint8_t nfcCounter = 0;
    while(true) {
        int8_t err = imu.getSensorData();
        Serial1.print("BMI status: ");
        Serial1.println(err);
        if (nfcCounter == 0) {
            nfcTask();
        }
        nfcCounter = (nfcCounter + 1) % 5;
    }
}

static void nfcTask() {
    Serial1.println("\nScan a NFC tag\n");

    bool tagPresent = nfc.tagPresent();

    if (tagPresent)
    {
        NfcTag tag = nfc.read();

        tag.print();
        tag.getUid(uid, tag.getUidLength());
        
        uint8_t uidLength = tag.getUidLength();
        int n = sizeof(whiteListUid) / sizeof(*whiteListUid);
    
        if (uidLength == n && std::equal(uid, uid + uidLength, whiteListUid)) {
            boolean authBefore = authorized;
            authorized = !authorized;
            authChanged = authorized ^ authBefore;
            delay(500);
        } else {
            Serial1.println("Not Authorized..");
        }
    } else {
        Serial1.println("Not present!");
    }
}