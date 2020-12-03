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

const char * ssid = "RFID";
const char * password = "17092020";
uint8_t uid[8];
uint8_t uidBuffer[5][8];
uint8_t uidBucket[5][8];
uint8_t *ptr1;
uint8_t *ptr2;
uint8_t readBuffer[4];
uint8_t dataBuffer[12];
uint8_t bigBuffer[5][12];
uint8_t bigBucket[5][12];
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



String numtos(uint8_t num){
String s="";
uint8_t tam;
   while(num>0){
        tam=num%10;
        s+=tam;
        num=num/10;
    }
    reverseStr(s);
return s;
}

String uidtos(uint8_t* uid){
String s="";
String S="";
for(int i=7;i>=0;i--){
  if(uid==0)
  s="0";
  else
    s=numtos(uid[i]);
    S+=s;
    s="";
}
return S+"a";
}

String processBuffer(uint8_t* dataBuffer){
String s="";
String S="";
for(int i=0;i<9;i++)
  s=s+char(dataBuffer[i]);
  S=S+s+numtos(dataBuffer[9])+numtos(dataBuffer[10])+numtos(dataBuffer[11]);
  return S+"a";
}

void addToBuffer(uint8_t* uid, uint8_t* dataBuffer){
    for(int j=0;j<8;j++){
      uidBuffer[n][j]=uid[j];
      }
    for(int j=0;j<12;j++){
      bigBuffer[n][j]=dataBuffer[j];
      }
      n++;  
    }
    

void addToDataBuffer(uint8_t* readBuffer, uint8_t* dataBuffer, int no){
  for(int i=0;i<4;i++)
  dataBuffer[i+4*no]=readBuffer[i];
  }

void isBufferFull(void * parameter){
  for(;;){
    ptr1=(uint8_t*)uidBucket;
    ptr2=(uint8_t*)bigBucket;
    int rowNum;
    xQueueReceive(queue, &rowNum, portMAX_DELAY);
    if(rowNum>=5){
    n=0;
    Serial.println("Buffer is full!");
    Serial.println(processBuffer(ptr2));
    sendData("UID="+uidtos(ptr1)+uidtos(ptr1+8)+uidtos(ptr1+16)+uidtos(ptr1+24)+uidtos(ptr1+32)+"&Serial="+processBuffer(ptr2)+processBuffer(ptr2+12)+processBuffer(ptr2+24)+processBuffer(ptr2+36)+processBuffer(ptr2+48));
    memset(uidBucket, 0, 40*(sizeof(uint8_t)));
    memset(bigBucket, 0, 60*(sizeof(uint8_t)));
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
  for (int no=0; no<3; no++) {
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
    addToDataBuffer(readBuffer,dataBuffer,no);
    Serial.print(" ");
    for (int i=0; i<blockSize; i++) {
      if (isprint(readBuffer[i])) {
        Serial.print((char)readBuffer[i]);
      }
      else Serial.print(".");
    }
    Serial.println();
  }

  addToBuffer(uid,dataBuffer);
  if(n>=5){
    xQueueSend(queue, &n, portMAX_DELAY);
    for(int i=0;i<5;i++)
    for(int j=0;j<8;j++)
    uidBucket[i][j]=uidBuffer[i][j];
    for(int i=0;i<5;i++)
    for(int j=0;j<12;j++)
    bigBucket[i][j]=bigBuffer[i][j];
    memset(uidBuffer, 0, 40*(sizeof(uint8_t)));
    memset(bigBuffer, 0, 60*(sizeof(uint8_t)));
  }
  for(int i=0;i<5;i++){
    Serial.print(i);Serial.print(":");
    for(int j=0;j<12;j++)
    Serial.print(bigBucket[i][j]);
    Serial.println();
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
