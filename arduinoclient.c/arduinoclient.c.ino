#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h> // library voor de raspberry pi
#include <WiFiClient.h>

//gebruikt gemaakt van een RFID library: https://github.com/miguelbalboa/rfid/blob/master/examples/ReadNUID/ReadNUID.ino

#define SS_PIN D8
#define RST_PIN D3

const char* ssid = "NSELab";
const char* password = "NSELabWiFi";
const char* server = "groepg-1.local"; // IP address van de Pi 
const int port = 8080; // Port voor server

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

byte nuidPICC[4];

void setup() { 
  Serial.begin(9600);
  SPI.begin(); 
  rfid.PCD_Init(); 

  Serial.println(F("This code scans the MIFARE Classic NUID."));

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) { 
    delay(100); 
  }

  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
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
  WiFiClient client;

  if (!client.connect(server, port)) {
    Serial.println("Connection to server failed"); //error als er geen connectie naar de server gemaakt kan worden
    return;
  }

  String rfidData = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    rfidData += String(rfid.uid.uidByte[i], HEX);
  }

  // Maakt HTTP request aan om naar de server te sturen.
  client.println("POST /rfiddata HTTP/1.1");
  client.println("Host: " + String(server));
  client.println("Content-Type: text/plain");
  client.println("Content-Length: " + String(rfidData.length()));
  client.println();
  client.println(rfidData);
  
  Serial.println("RFID data sent to server: " + rfidData);

  while (client.connected()) {
    if (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.println(line);
    }
  }

  client.stop();
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
