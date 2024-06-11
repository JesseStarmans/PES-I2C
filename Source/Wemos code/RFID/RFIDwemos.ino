#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h> // library voor de raspberry pi
#include <WiFiClient.h>

//gebruikt gemaakt van een RFID library: https://github.com/miguelbalboa/rfid/blob/master/examples/ReadNUID/ReadNUID.ino

#define SS_PIN D8
#define RST_PIN D3
#define WEMOS3 "Wemos3"
#define WEMOS4 "Wemos4"

//const char* wemos3 = "wemos3";
//const char* wemos4 = "wemos4";

// const char* ssid = "PiNetGroepG";
// const char* password = "GroepGNet";
// const char* serverIPaddress = "10.42.0.251"; // IP address van de Pi 

const char* ssid = "NSELab";
const char* password = "NSELabWiFi";
const char* serverIPaddress = "145.52.127.184"; // IP address van de Pi 
const int port = 8080; // Port voor server

WiFiServer server(port);

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

byte nuidPICC[4];



void setup() { 
  Serial.begin(9600);
  SPI.begin(); 
  rfid.PCD_Init(); 
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) { 
    delay(500); 
    Serial.print(".");
  }

  Serial.println("connected to wifi");

server.begin();
  Serial.println("Server started");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  
  // sendIP functies bestaan om de server IP invulling te geven. Comment uit wat je niet gebruikt
  //sendIP("Wemos1");
  //sendIP("Wemos2");
  //sendIP("Wemos3");
  sendIP("Wemos4");
  Serial.println("Waiting for request");
}

void sendIP(String name){
  int ipDelen[4];
  int i = 0;
  String tempString = serverIPaddress;
  String bufferIP;
  while (tempString.length() > 0) {
    bufferIP = tempString.substring(0, tempString.indexOf('.'));
    ipDelen[i++] = bufferIP.toInt();
    tempString = tempString.substring(tempString.indexOf('.') + 1);
  }
  IPAddress serverIP(ipDelen[0], ipDelen[1], ipDelen[2], ipDelen[3]);
  
  WiFiClient client;
  if (client.connect(serverIP, port)){
    Serial.println("Connected to server");
  
  // Send a message to the server
  IPAddress localIP = WiFi.localIP();
  char ipFull[50] = "";
  sprintf(ipFull, "%s %d.%d.%d.%d", name, localIP[0], localIP[1],localIP[2],localIP[3]);
  Serial.println(ipFull);
  client.println(ipFull);
  delay(150);
  client.stop();
  }
  else {
    Serial.println("Unable to connect to server");
  }
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent())
    return;

  if (!rfid.PICC_ReadCardSerial())
    return;

  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);

  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

  if (rfid.uid.uidByte[0] != nuidPICC[0] || 
    rfid.uid.uidByte[1] != nuidPICC[1] || 
    rfid.uid.uidByte[2] != nuidPICC[2] || 
    rfid.uid.uidByte[3] != nuidPICC[3]) {
    Serial.println(F("A new card has been detected."));

    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }

    Serial.println(F("The NUID tag is:"));
    Serial.print(F("In hex: "));
    printHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    Serial.print(F("In dec: "));
    printDec(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();

    // Send RFID data to the server
    sendRFIDData();
  }
  else {
    Serial.println(F("Card read previously."));
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  delay(1000);

  nuidPICC[0] = 0; // zorgt ervoor dat dezelfde tag twee keer achter elkaar gescant kan worden
}

void sendRFIDData() {
    int ipDelen[4];
  int i = 0;
  String tempString = serverIPaddress;
  String bufferIP;
  while (tempString.length() > 0) {
    bufferIP = tempString.substring(0, tempString.indexOf('.'));
    ipDelen[i++] = bufferIP.toInt();
    tempString = tempString.substring(tempString.indexOf('.') + 1);
  }
  IPAddress serverIP(ipDelen[0], ipDelen[1], ipDelen[2], ipDelen[3]);
   WiFiClient client;
   if (client.connect(serverIP, port)) {
    String rfidData = "Voordeur: ";
    for (byte i = 0; i < rfid.uid.size; i++) {
      rfidData += String(rfid.uid.uidByte[i], HEX);
    }

    //client.println("POST /rfiddata HTTP/1.1");
    client.println(rfidData);
    
    Serial.println("RFID data sent to server: " + rfidData);

    if (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.println(line);
    }
   }
  else{
    Serial.println("Disconnected from server, reconnecting...");

  }

  
}


void resetNUIDBuffer() {
    for (byte i = 0; i < 4; i++) {
        nuidPICC[i] = 0;
    }
}


void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(' ');
    Serial.print(buffer[i], DEC);
  }
}
