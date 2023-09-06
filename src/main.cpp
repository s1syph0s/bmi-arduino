// --------------------------------------
// i2c_scanner
//
// Modified from https://playground.arduino.cc/Main/I2cScanner/
// --------------------------------------
#include "Arduino.h"
#include "Wire.h"
#include "WiFi.h"
#include "WebServer.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include "PN532_I2C.h"
#include "PN532.h"
#include "NfcAdapter.h"

// Set I2C bus to use: Wire, Wire1, etc.
#define WIRE Wire

PN532_I2C pn532i2c(WIRE);
NfcAdapter nfc(pn532i2c);

String getLocalUidString(byte *_uid, unsigned int _uidLength)
{
    String uidString = "";
    for (unsigned int i = 0; i < _uidLength; i++)
    {
        if (i > 0)
        {
            uidString += " ";
        }

        if (_uid[i] < 0xF)
        {
            uidString += "0";
        }

        uidString += String((unsigned int)_uid[i], (unsigned char)HEX);
    }
    uidString.toUpperCase();
    return uidString;
}

void setup() {
  WIRE.begin(2, 1);

  // Serial.begin(9600);
  Serial1.begin(9600, SERIAL_8N1, 12, 13);

  nfc.begin();
  Serial1.println("Turvalo test");
}

void loop() {
  byte error, address;
  int nDevices;
  byte uid[7];
  byte whiteListUid[4] = {0x53, 0x18, 0x46, 0xFA};

  Serial1.println("\nScan a NFC tag\n");
  if (nfc.tagPresent())
  {
      NfcTag tag = nfc.read();
      tag.print();
      tag.getUid(uid, tag.getUidLength());
     
      uint8_t uidLength = tag.getUidLength();
      int n = sizeof(whiteListUid) / sizeof(*whiteListUid);
  
      if (uidLength == n && std::equal(uid, uid + uidLength, whiteListUid)) {
          Serial1.println("Authorized!");
      } else {
          Serial1.println("Not Authorized..");
      }
  }
}