#include <ESP8266WiFi.h> // library voor de raspberry pi
#include <WiFiClient.h>

#include <avr/pgmspace.h>
#include <LedControl.h>

const char* ssid = "NSELab";
const char* password = "NSELabWiFi";
const char* serverIPaddress = "145.52.127.184"; // IP address van de Pi 
const int port = 8080; // Port voor server
const int serverPort = 6060;



const int numDevices = 4;      // number of MAX7219s used
const long scrollDelay = 50;   // adjust scrolling speed
unsigned long bufferLong [14] = {0};

LedControl lc = LedControl(12, 14, 15, 4); //din cs

//const unsigned char scrollText[] PROGMEM = {" dit WERKT "};
char receivedText[50] = " HOI IK BEN MARIJN ";  // Buffer voor ontvangen tekst

WiFiServer server(serverPort);

void setup() { 
  Serial.begin(115200);

  WiFi.begin(ssid, password);

   while (WiFi.status() != WL_CONNECTED) {
     delay(1000);
     Serial.println("Connecting to WiFi...");
   }

  Serial.println("Connected to WiFi");

  // start TCP server
  server.begin();
  Serial.println("Server started");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  
  // sendIP functies bestaan om de server IP invulling te geven. Comment uit wat je niet gebruikt
  sendIP("Wemos1");
  //sendIP("Wemos2");
  //sendIP("Wemos3");
  Serial.println("Waiting for request");
   for (int x = 0; x < numDevices; x++) {
    lc.shutdown(x, false);      //The MAX72XX is in power-saving mode on startup
    lc.setIntensity(x, 8);      // Set the brightness to default value
    lc.clearDisplay(x);         // and clear the display
  }
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
  serverCode();
  if (Serial.available() > 0) {
    String inlees = Serial.readStringUntil('\n');
    Serial.println("Ontvangen data: " + inlees);
    clientCodeMetSend(inlees);
  }
  scrollMessage((const unsigned char *)receivedText);
}
const unsigned char font5x7 [] PROGMEM = {      //Numeric Font Matrix (Arranged as 7x font data + 1x kerning data)
  B00000000,  //Space (Char 0x20)
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  6,


  B10000000,  //!
  B10000000,
  B10000000,
  B10000000,
  B00000000,
  B00000000,
  B10000000,
  2,


  B10100000,  //"
  B10100000,
  B10100000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  4,


  B01010000,  //#
  B01010000,
  B11111000,
  B01010000,
  B11111000,
  B01010000,
  B01010000,
  6,


  B00100000,  //$
  B01111000,
  B10100000,
  B01110000,
  B00101000,
  B11110000,
  B00100000,
  6,


  B11000000,  //%
  B11001000,
  B00010000,
  B00100000,
  B01000000,
  B10011000,
  B00011000,
  6,


  B01100000,  //&
  B10010000,
  B10100000,
  B01000000,
  B10101000,
  B10010000,
  B01101000,
  6,


  B11000000,  //'
  B01000000,
  B10000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  3,


  B00100000,  //(
  B01000000,
  B10000000,
  B10000000,
  B10000000,
  B01000000,
  B00100000,
  4,


  B10000000,  //)
  B01000000,
  B00100000,
  B00100000,
  B00100000,
  B01000000,
  B10000000,
  4,


  B00000000,  //*
  B00100000,
  B10101000,
  B01110000,
  B10101000,
  B00100000,
  B00000000,
  6,


  B00000000,  //+
  B00100000,
  B00100000,
  B11111000,
  B00100000,
  B00100000,
  B00000000,
  6,


  B00000000,  //,
  B00000000,
  B00000000,
  B00000000,
  B11000000,
  B01000000,
  B10000000,
  3,


  B00000000,  //-
  B00000000,
  B00000000,
  B11111000,
  B00000000,
  B00000000,
  B00000000,
  6,


  B00000000,  //.
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B11000000,
  B11000000,
  3,


  B00000000,  ///
  B00001000,
  B00010000,
  B00100000,
  B01000000,
  B10000000,
  B00000000,
  6,


  B01110000,  //0
  B10001000,
  B10011000,
  B10101000,
  B11001000,
  B10001000,
  B01110000,
  6,


  B01000000,  //1
  B11000000,
  B01000000,
  B01000000,
  B01000000,
  B01000000,
  B11100000,
  4,


  B01110000,  //2
  B10001000,
  B00001000,
  B00010000,
  B00100000,
  B01000000,
  B11111000,
  6,


  B11111000,  //3
  B00010000,
  B00100000,
  B00010000,
  B00001000,
  B10001000,
  B01110000,
  6,


  B00010000,  //4
 
