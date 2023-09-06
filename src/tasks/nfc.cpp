#include "Wire.h"

#include "PN532_I2C.h"
#include "PN532.h"
#include "NfcAdapter.h"
#include "nfc.h"
#include "defs.h"

PN532_I2C pn532i2c(Wire);
NfcAdapter nfc(pn532i2c);
extern HardwareSerial Serial1;
extern TwoWire Wire;
extern boolean authorized;
extern boolean authChanged;

void NfcSetup()
{
    nfc.begin();
    }

void NfcTask(void *parameter)
{
    byte uid[7];
    byte whiteListUid[4] = {0x53, 0x18, 0x46, 0xFA};

    while(true) {
        Serial1.println("\nScan a NFC tag\n");
        if (nfc.tagPresent())
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
                delay(1000);
            } else {
                Serial1.println("Not Authorized..");
            }
        } else {
            Serial1.println("Debug");
        }
        delay(100);
    }
    
}