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
#include "PN532_HSU.h"
#include "PN532.h"
#include "NfcAdapter.h"

// Set I2C bus to use: Wire, Wire1, etc.
#define WIRE Wire

/* Put your SSID & Password */
const char* ssid = "ESP32";  // Enter SSID here
const char* password = "12345678";  //Enter Password here

/* Put IP Address details */
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

WebServer server(80);
String i2cAddress;

PN532_HSU pn532hsu(Serial2);
//Nfc nfc(pn532hsu);
NfcAdapter nfc(pn532hsu);

void handle_onConnect() {
server.send(200, "text/plain", i2cAddress.c_str());
}

void handle_notFound() {
  server.send(404, "text/plain", "Not Found");
}

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
  WIRE.begin();

  Serial.begin(9600);
  while (!Serial)
     delay(10);
  Serial.println("\nI2C Scanner");

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);

  server.on("/", handle_onConnect);
  server.onNotFound(handle_notFound);
  server.begin();

  // nfc.begin();

  // uint32_t versiondata = nfc.getFirmwareVersion();
  // if (! versiondata) {
  //   Serial.print("Didn't find PN53x board");
  //   while (1); // halt
  // }
  
  // // Got ok data, print it out!
  // Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  // Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  // Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);

  // // Set the max number of retry attempts to read from a card
  // // This prevents us from waiting forever for a card, which is
  // // the default behaviour of the PN532.
  // nfc.setPassiveActivationRetries(0xFF);
  
  // // configure board to read RFID tags
  // nfc.SAMConfig();
  nfc.begin();
}

void loop() {
  byte error, address;
  int nDevices;
  byte uid[7];
  byte whiteListUid[7] = {0x53, 0x18, 0x46, 0xFA};

  Serial.println("\nScan a NFC tag\n");
  if (nfc.tagPresent())
  {
      NfcTag tag = nfc.read();
      tag.print();
      tag.getUid(uid, tag.getUidLength());
      Serial.println(getLocalUidString(uid, 4));
      Serial.println(getLocalUidString(whiteListUid, 4));
     
      uint8_t uidLength = tag.getUidLength();
      Serial.println(uidLength);
      int n = sizeof(whiteListUid) / sizeof(*whiteListUid);
  
      if (uidLength == n && std::equal(uid, uid + uidLength, whiteListUid)) {
          Serial.println("Authorized!");
      } else {
          Serial.println("Not Authorized..");
      }
  }

  server.handleClient();

  Serial.println("Scanning...");

  nDevices = 0;

  std::stringstream stream;
  for(address = 1; address < 127; address++ ) 
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    WIRE.beginTransmission(address);
    error = WIRE.endTransmission();

    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16) 
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");

      stream.str("");
      stream << "0x" << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(address) << '\n';

      i2cAddress = String(stream.str().c_str());

      nDevices++;
    }
    else if (error==4) 
    {
      Serial.print("Unknown error at address 0x");
      if (address<16) 
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
    i2cAddress = "No I2C devices found\n";
  }
  else
    Serial.println("done\n");

  delay(5000);           // wait 5 seconds for next scan
}