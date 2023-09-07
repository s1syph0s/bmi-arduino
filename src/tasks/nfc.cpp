#include "Wire.h"

#include "PN532_I2C.h"
#include "PN532.h"
#include "NfcAdapter.h"
#include "nfc.h"
#include "defs.h"

PN532_I2C pn532i2c(Wire);
PN532 nfc(pn532i2c);

extern HardwareSerial Serial1;
extern TwoWire Wire;
extern boolean authorized;
extern boolean authChanged;

extern SemaphoreHandle_t mutex;

void NfcSetup()
{
    nfc.begin();
    uint32_t versiondata = nfc.getFirmwareVersion();
    if (! versiondata) {
        Serial.print("Didn't find PN53x board");
        while (1); // halt
    }

    // Got ok data, print it out!
    Serial1.print("Found chip PN5"); Serial1.println((versiondata>>24) & 0xFF, HEX); 
    Serial1.print("Firmware ver. "); Serial1.print((versiondata>>16) & 0xFF, DEC); 
    Serial1.print('.'); Serial1.println((versiondata>>8) & 0xFF, DEC);

    // Set the max number of retry attempts to read from a card
    // This prevents us from waiting forever for a card, which is
    // the default behaviour of the PN532.
    nfc.setPassiveActivationRetries(0x20);

    // configure board to read RFID tags
    nfc.SAMConfig();

    Serial1.println("Waiting for an ISO14443A card");
}

void NfcTask(void *parameter)
{
    byte uid[7];
    byte whiteListUid[4] = {0x53, 0x18, 0x46, 0xFA};
    uint8_t whiteListUidLength = sizeof(whiteListUid) / sizeof(*whiteListUid);
    uint8_t uidLength;

    while (true) {
        // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
        // 'uid' will be populated with the UID, and uidLength will indicate
        // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
        Serial1.println("Reading nfc...");
        xSemaphoreTake(mutex, portMAX_DELAY);
        bool success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);
        xSemaphoreGive(mutex);

        if (success) {
            if (uidLength == whiteListUidLength && std::equal(uid, uid+uidLength, whiteListUid)) {
                boolean authBefore = authorized;
                authorized = !authorized;
                authChanged = authorized ^ authBefore;
                delay(500);
            } else {
                Serial1.println("NFC Tag not whitelisted..");
            }
        }
        else
        {
            // PN532 probably timed out waiting for a card
            Serial1.println("Timed out waiting for a card");
        }
    }
}