// NAME: PN5180-Library.ino
//
// DESC: Example usage of the PN5180 library for the PN5180-NFC Module
//       from NXP Semiconductors.
//
// Copyright (c) 2018 by Andreas Trappmann. All rights reserved.
//
// This file is part of the PN5180 library for the Arduino environment.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// BEWARE: SPI with an Arduino to a PN5180 module has to be at a level of 3.3V
// use of logic-level converters from 5V->3.3V is absolutely necessary
// on most Arduinos for all input pins of PN5180!
// If used with an ESP-32, there is no need for a logic-level converter, since
// it operates on 3.3V already.
//
// Arduino <-> Level Converter <-> PN5180 pin mapping:
// 5V             <-->             5V
// 3.3V           <-->             3.3V
// GND            <-->             GND
// 5V      <-> HV
// GND     <-> GND (HV)
//             LV              <-> 3.3V
//             GND (LV)        <-> GND
// SCLK,13 <-> HV1 - LV1       --> SCLK
// MISO,12        <---         <-- MISO
// MOSI,11 <-> HV3 - LV3       --> MOSI
// SS,10   <-> HV4 - LV4       --> NSS (=Not SS -> active LOW)
// BUSY,9         <---             BUSY
// Reset,7 <-> HV2 - LV2       --> RST
//
// ESP-32    <--> PN5180 pin mapping:
// 3.3V      <--> 3.3V
// GND       <--> GND
// SCLK, 18   --> SCLK
// MISO, 19  <--  MISO
// MOSI, 23   --> MOSI
// SS, 16     --> NSS (=Not SS -> active LOW)
// BUSY, 5   <--  BUSY
// Reset, 17  --> RST
//

/*
 * Pins on ICODE2 Reader Writer:
 *
 *   ICODE2   |     PN5180
 * pin  label | pin  I/O  name
 * 1    +5V
 * 2    +3,3V
 * 3    RST     10   I    RESET_N (low active)
 * 4    NSS     1    I    SPI NSS
 * 5    MOSI    3    I    SPI MOSI
 * 6    MISO    5    O    SPI MISO
 * 7    SCK     7    I    SPI Clock
 * 8    BUSY    8    O    Busy Signal
 * 9    GND     9  Supply VSS - Ground
 * 10   GPIO    38   O    GPO1 - Control for external DC/DC
 * 11   IRQ     39   O    IRQ
 * 12   AUX     40   O    AUX1 - Analog/Digital test signal
 * 13   REQ     2?  I/O   AUX2 - Analog test bus or download
 *
 
#define WRITE_ENABLED 1*/

#include <PN5180.h>
#include <PN5180ISO15693.h>
#include <WiFi.h>
#include <HTTPClient.h>

#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_MEGA2560) || defined(ARDUINO_AVR_NANO)

#define PN5180_NSS  10
#define PN5180_BUSY 9
#define PN5180_RST  7

#elif defined(ARDUINO_ARCH_ESP32)

#define PN5180_NSS  5
#define PN5180_BUSY 16
#define PN5180_RST  17

TaskHandle_t Task1, Task0;
QueueHandle_t queue;

#else
#error Please define your pinout here!
#endif

PN5180ISO15693 nfc(PN5180_NSS, PN5180_BUSY, PN5180_RST);

const char * ssid = "DIFR.APoint";
const char * password = "17092020";
uint8_t uid[8];
uint8_t uidBuffer[5][8];
uint8_t uidBucket[5][8];
uint8_t *ptr1;
uint8_t *ptr2;
uint8_t readBuffer[4];
uint8_t serialBuffer[5][4];
uint8_t serialBucket[5][4];
int n=0;

String GOOGLE_SCRIPT_ID = "AKfycbxibgiGwnmWHVlHq2R7xVx9_wCfG7OEW6YUaLIGtE-Zs3uR5DA"; // Replace by your GAS service id

const int sendInterval = 996 *4; // in millis, 996 instead of 1000 is adjustment, with 1000 it jumps ahead a minute every 3-4 hours
const char * root_ca=\
"-----BEGIN CERTIFICATE-----\n" \
"MIIDujCCAqKgAwIBAgILBAAAAAABD4Ym5g0wDQYJKoZIhvcNAQEFBQAwTDEgMB4G\n" \
"A1UECxMXR2xvYmFsU2lnbiBSb290IENBIC0gUjIxEzARBgNVBAoTCkdsb2JhbFNp\n" \
"Z24xEzARBgNVBAMTCkdsb2JhbFNpZ24wHhcNMDYxMjE1MDgwMDAwWhcNMjExMjE1\n" \
"MDgwMDAwWjBMMSAwHgYDVQQLExdHbG9iYWxTaWduIFJvb3QgQ0EgLSBSMjETMBEG\n" \
"A1UEChMKR2xvYmFsU2lnbjETMBEGA1UEAxMKR2xvYmFsU2lnbjCCASIwDQYJKoZI\n" \
"hvcNAQEBBQADggEPADCCAQoCggEBAKbPJA6+Lm8omUVCxKs+IVSbC9N/hHD6ErPL\n" \
"v4dfxn+G07IwXNb9rfF73OX4YJYJkhD10FPe+3t+c4isUoh7SqbKSaZeqKeMWhG8\n" \
"eoLrvozps6yWJQeXSpkqBy+0Hne/ig+1AnwblrjFuTosvNYSuetZfeLQBoZfXklq\n" \
"tTleiDTsvHgMCJiEbKjNS7SgfQx5TfC4LcshytVsW33hoCmEofnTlEnLJGKRILzd\n" \
"C9XZzPnqJworc5HGnRusyMvo4KD0L5CLTfuwNhv2GXqF4G3yYROIXJ/gkwpRl4pa\n" \
"zq+r1feqCapgvdzZX99yqWATXgAByUr6P6TqBwMhAo6CygPCm48CAwEAAaOBnDCB\n" \
"mTAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUm+IH\n" \
"V2ccHsBqBt5ZtJot39wZhi4wNgYDVR0fBC8wLTAroCmgJ4YlaHR0cDovL2NybC5n\n" \
"bG9iYWxzaWduLm5ldC9yb290LXIyLmNybDAfBgNVHSMEGDAWgBSb4gdXZxwewGoG\n" \
"3lm0mi3f3BmGLjANBgkqhkiG9w0BAQUFAAOCAQEAmYFThxxol4aR7OBKuEQLq4Gs\n" \
"J0/WwbgcQ3izDJr86iw8bmEbTUsp9Z8FHSbBuOmDAGJFtqkIk7mpM0sYmsL4h4hO\n" \
"291xNBrBVNpGP+DTKqttVCL1OmLNIG+6KYnX3ZHu01yiPqFbQfXf5WRDLenVOavS\n" \
"ot+3i9DAgBkcRcAtjOj4LaR0VknFBbVPFd5uRHg5h6h+u/N5GJG79G+dwfCMNYxd\n" \
"AfvDbbnvRG15RjF+Cv6pgsH/76tuIMRQyV+dTZsXjAzlAcmgQWpzU/qlULRuJQ/7\n" \
"TBj0/VLZjmmx6BEP3ojY+x1J96relc8geMJgEtslQIxq/H5COEBkEveegeGTLg==\n" \
"-----END CERTIFICATE-----\n";

WiFiClientSecure client;

void showIRQStatus(uint32_t irqStatus) {
  Serial.print(F("IRQ-Status 0x"));
  Serial.print(irqStatus, HEX);
  Serial.print(": [ ");
  if (irqStatus & (1<< 0)) Serial.print(F("RQ "));
  if (irqStatus & (1<< 1)) Serial.print(F("TX "));
  if (irqStatus & (1<< 2)) Serial.print(F("IDLE "));
  if (irqStatus & (1<< 3)) Serial.print(F("MODE_DETECTED "));
  if (irqStatus & (1<< 4)) Serial.print(F("CARD_ACTIVATED "));
  if (irqStatus & (1<< 5)) Serial.print(F("STATE_CHANGE "));
  if (irqStatus & (1<< 6)) Serial.print(F("RFOFF_DET "));
  if (irqStatus & (1<< 7)) Serial.print(F("RFON_DET "));
  if (irqStatus & (1<< 8)) Serial.print(F("TX_RFOFF "));
  if (irqStatus & (1<< 9)) Serial.print(F("TX_RFON "));
  if (irqStatus & (1<<10)) Serial.print(F("RF_ACTIVE_ERROR "));
  if (irqStatus & (1<<11)) Serial.print(F("TIMER0 "));
  if (irqStatus & (1<<12)) Serial.print(F("TIMER1 "));
  if (irqStatus & (1<<13)) Serial.print(F("TIMER2 "));
  if (irqStatus & (1<<14)) Serial.print(F("RX_SOF_DET "));
  if (irqStatus & (1<<15)) Serial.print(F("RX_SC_DET "));
  if (irqStatus & (1<<16)) Serial.print(F("TEMPSENS_ERROR "));
  if (irqStatus & (1<<17)) Serial.print(F("GENERAL_ERROR "));
  if (irqStatus & (1<<18)) Serial.print(F("HV_ERROR "));
  if (irqStatus & (1<<19)) Serial.print(F("LPCD "));
  Serial.println("]");
}

void sendData(String params) {
   HTTPClient http;
   String url="https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?"+params;
    Serial.print("Making a request");
    http.begin(url, root_ca); //Specify the URL and certificate
    int httpCode = http.GET();  
    http.end();
}

void reverseStr(String& string)
{
    int n = string.length();
    int giua,i;
    // Swap character starting from two
    // corners
    for (i = 0; i < n / 2; i++){
        giua=string[i];
        string[i]=string[n - i - 1];
        string[n - i - 1]=giua;
    }
}
String uidtos(uint8_t* uid,int len){
String s="";
String S="";
uint8_t tam;
for(int i=len-1;i>=0;i--){
    while(uid[i]>0){
        tam=uid[i]%10;
        s+=tam;
        uid[i]=uid[i]/10;
    }
    reverseStr(s);
    S+=s;
    s="";
}
return S+"a";
}

void addToBuffer(uint8_t* uid){
    for(int j=0;j<8;j++){
      uidBuffer[n][j]=uid[j];
      }
    for(int j=0;j<4;j++){
      serialBuffer[n][j]=readBuffer[j];
      }
      n++;
      
    }

void isBufferFull(void * parameter){
  for(;;){
    ptr1=(uint8_t*)uidBucket;
    ptr2=(uint8_t*)serialBucket;
    int rowNum;
    xQueueReceive(queue, &rowNum, portMAX_DELAY);
    if(rowNum==5){
    n=0;
    Serial.println("Buffer is full!");
    sendData("UID="+uidtos((ptr1),8)+uidtos(ptr1+8,8)+uidtos(ptr1+16,8)+uidtos(ptr1+24,8)+uidtos(ptr1+32,8)+"&Serial="+uidtos(ptr2,4)+uidtos(ptr2+4,4)+uidtos(ptr2+8,4)+uidtos(ptr2+12,4)+uidtos(ptr2+16,4));
    memset(uidBucket, 0, 40*(sizeof(uint8_t)));
    memset(serialBucket, 0, 20*(sizeof(uint8_t)));
    delay(sendInterval);
    }
    delay(10);
}}


uint32_t loopCnt = 0;
bool errorFlag = false;

void scan(){
  
  if (errorFlag) {
    uint32_t irqStatus = nfc.getIRQStatus();
    showIRQStatus(irqStatus);
    if (0 == (RX_SOF_DET_IRQ_STAT & irqStatus)) { // no card detected
      Serial.println(F("*** No card detected!"));
    }

    nfc.reset();
    nfc.setupRF();

    errorFlag = false;
  }

  Serial.println(F("----------------------------------"));
  Serial.print(F("Loop #"));
  Serial.print(loopCnt++);
  Serial.print("n=");
  Serial.println(n);

/*
  // code for unlocking an ICODE SLIX2 protected tag   
  uint8_t password[] = {0x5B, 0x6E, 0xFD, 0x7F};
  ISO15693ErrorCode myrc = nfc.unlockICODESLIX2(password);
  if (ISO15693_EC_OK == myrc) {
    Serial.println("unlockICODESLIX2 successful");
  }
*/
  char result[100];
  ISO15693ErrorCode rc = nfc.getInventory(uid);
  if (ISO15693_EC_OK != rc) {
    Serial.print(F("Error in getInventory: "));
    Serial.println(nfc.strerror(rc));
    errorFlag = true;
    return;
  }
  Serial.print(F("Inventory successful, UID="));
  for (int i=0; i<8; i++) {
    Serial.print(uid[7-i]); // LSB is first
    if (i < 8) Serial.print(":");
  }
  Serial.println();

  Serial.println(F("----------------------------------"));
  uint8_t blockSize, numBlocks;
  rc = nfc.getSystemInfo(uid, &blockSize, &numBlocks);
  if (ISO15693_EC_OK != rc) {
    Serial.print(F("Error in getSystemInfo: "));
    Serial.println(nfc.strerror(rc));
    errorFlag = true;
    return;
  }
  Serial.print(F("System Info retrieved: blockSize="));
  Serial.print(blockSize);
  Serial.print(F(", numBlocks="));
  Serial.println(numBlocks);
  Serial.println(F("----------------------------------"));
  for (int no=0; no<1; no++) {
    rc = nfc.readSingleBlock(uid, no, readBuffer, blockSize);
    if (ISO15693_EC_OK != rc) {
      Serial.print(F("Error in readSingleBlock #"));
      Serial.print(no);
      Serial.print(": ");
      Serial.println(nfc.strerror(rc));
      errorFlag = true;
      return;
    }
    Serial.print(F("Read block #"));
    Serial.print(no);
    Serial.print(": ");
    for (int i=0; i<blockSize; i++) {
      if (readBuffer[i] < 16) Serial.print("0");
      Serial.print(readBuffer[i], HEX);
      Serial.print(" ");
    }
    Serial.print(" ");
    for (int i=0; i<blockSize; i++) {
      if (isprint(readBuffer[i])) {
        Serial.print((char)readBuffer[i]);
      }
      else Serial.print(".");
    }
    Serial.println();
  }
/*#ifdef WRITE_ENABLED
  Serial.println(F("----------------------------------"));
  uint8_t *writeBuffer;
   writeBuffer = (uint8_t *) malloc(blockSize);
  for (int i=0; i<blockSize; i++) {
    writeBuffer[i] = 0x80 + i;
  }
  for (int no=0; no<numBlocks; no++) {
    rc = nfc.writeSingleBlock(uid, no, writeBuffer, blockSize);
    if (ISO15693_EC_OK == rc) {
      Serial.print(F("Wrote block #"));
      Serial.println(no);
    }
    else {
      Serial.print(F("Error in writeSingleBlock #"));
      Serial.print(no);
      Serial.print(": ");
      Serial.println(nfc.strerror(rc));
      errorFlag = true;
      return;
    }
  }
#endif /* WRITE_ENABLED */

/*
  // code for locking an ICODE SLIX2 protected tag   
  ISO15693ErrorCode myrc = nfc.lockICODESLIX2(password);
  if (ISO15693_EC_OK == myrc) {
    Serial.println("lockICODESLIX2 successful");
    delay(5000);
*/
  addToBuffer(uid);
  if(n==5){
    for(int i=0;i<5;i++)
    for(int j=0;j<8;j++)
    uidBucket[i][j]=uidBuffer[i][j];
    for(int i=0;i<5;i++)
    for(int j=0;j<4;j++)
    serialBucket[i][j]=serialBuffer[i][j];
    xQueueSend(queue, &n, portMAX_DELAY);
    memset(uidBuffer, 0, 40*(sizeof(uint8_t)));
    memset(serialBuffer, 0, 20*(sizeof(uint8_t)));
  }    
  delay(500);//delay between reads}
}
void codeForTask1(void * parameter) {
  for(;;){
  scan();
  delay(10);
}}


void setup() {
  Serial.begin(115200);
    WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println(F("=================================="));
  Serial.println(F("Uploaded: " __DATE__ " " __TIME__));
  Serial.println(F("PN5180 ISO15693 Demo Sketch"));

  nfc.begin();

  Serial.println(F("----------------------------------"));
  Serial.println(F("PN5180 Hard-Reset..."));
  nfc.reset();

  Serial.println(F("----------------------------------"));
  Serial.println(F("Reading product version..."));
  uint8_t productVersion[2];
  nfc.readEEprom(PRODUCT_VERSION, productVersion, sizeof(productVersion));
  Serial.print(F("Product version="));
  Serial.print(productVersion[1]);
  Serial.print(".");
  Serial.println(productVersion[0]);

  if (0xff == productVersion[1]) { // if product version 255, the initialization failed
    Serial.println(F("Initialization failed!?"));
    Serial.println(F("Press reset to restart..."));
    Serial.flush();
    exit(-1); // halt
  }
  
  Serial.println(F("----------------------------------"));
  Serial.println(F("Reading firmware version..."));
  uint8_t firmwareVersion[2];
  nfc.readEEprom(FIRMWARE_VERSION, firmwareVersion, sizeof(firmwareVersion));
  Serial.print(F("Firmware version="));
  Serial.print(firmwareVersion[1]);
  Serial.print(".");
  Serial.println(firmwareVersion[0]);

  Serial.println(F("----------------------------------"));
  Serial.println(F("Reading EEPROM version..."));
  uint8_t eepromVersion[2];
  nfc.readEEprom(EEPROM_VERSION, eepromVersion, sizeof(eepromVersion));
  Serial.print(F("EEPROM version="));
  Serial.print(eepromVersion[1]);
  Serial.print(".");
  Serial.println(eepromVersion[0]);

  /*
  Serial.println(F("----------------------------------"));
  Serial.println(F("Reading IRQ pin config..."));
  uint8_t irqConfig;
  nfc.readEEprom(IRQ_PIN_CONFIG, &irqConfig, 1));
  Serial.print(F("IRQ_PIN_CONFIG=0x"));
  Serial.println(irqConfig, HEX);
  Serial.println(F("----------------------------------"));
  Serial.println(F("Reading IRQ_ENABLE register..."));
  uint32_t irqEnable;
  nfc.readRegister(IRQ_ENABLE, &irqEnable));
  Serial.print(F("IRQ_ENABLE=0x"));
  Serial.println(irqConfig, HEX);
  */

  Serial.println(F("----------------------------------"));
  Serial.println(F("Enable RF field..."));
  nfc.setupRF();

  queue = xQueueCreate(1, sizeof(int));
  
  xTaskCreatePinnedToCore(
      isBufferFull, /* Function to implement the task */
      "wifitask", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      0,  /* Priority of the task */
      &Task0,  /* Task handle. */
      0); /* Core where the task should run */
   xTaskCreatePinnedToCore(
      codeForTask1, /* Function to implement the task */
      "scantask", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      1,  /* Priority of the task */
      &Task1,  /* Task handle. */
      1); /* Core where the task should run */
}


void loop(){
  vTaskDelay(10);}
