#define NOODSTOP D7
#define LEDPIN D4
#include <FastLED.h>
#include <ESP8266WiFi.h> // library voor de raspberry pi
#include <WiFiClient.h>
#include <avr/pgmspace.h>
#include <LedControl.h>
#include <string>


const char* ssid = "PiNetGroepG";
const char* password = "GroepGNet";
const char* serverIPaddress = "10.42.0.251"; // IP address van de Pi 
const int port = 8080; // Port voor server
const int serverPort = 6060;

// const char* ssid = "NSELab";
// const char* password = "NSELabWiFi";
// const char* serverIPaddress = "145.52.127.184"; // IP address van de Pi 
// const int port = 8080; // Port voor server
// const int serverPort = 6060;

const int numDevices = 4;      // number of MAX7219s used
const long scrollDelay = 50;   // adjust scrolling speed
unsigned long bufferLong [14] = {0};
const unsigned long debounceDelay = 300; // verhoog de debounce delay

LedControl lc = LedControl(12, 14, 15, 4); //din cs
unsigned long lastButtonPress = 0;

volatile bool buttonPressed = false;
void ICACHE_RAM_ATTR buttonISR();

//const unsigned char scrollText[] PROGMEM = {" dit WERKT "};
char receivedText[50] = " B  $ ";  // Buffer voor ontvangen tekst

WiFiServer server(serverPort);

void setup() { 
  Serial.begin(115200);
  pinMode(NOODSTOP, INPUT_PULLUP);
  pinMode(LEDPIN, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(NOODSTOP), buttonISR, RISING);

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
  Serial.println(localIP);
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
  if(buttonPressed){
    digitalWrite(LEDPIN, HIGH);
    clientCodeMetSend("Noodknop:");
    buttonPressed = false;
  }
  serverCode();
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("Reconecting......");
    while(!WiFi.reconnect()){

    }
  }
  if (Serial.available() > 0) {
    String inlees = Serial.readStringUntil('\n');
    Serial.println("Ontvangen data: " + inlees);
    clientCodeMetSend(inlees);
  }
  scrollMessage((const unsigned char *)receivedText);
}

void buttonISR() {
  buttonPressed = true;
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


  B00111000, //$
  B01000000,
  B11111000,
  B01000000,
  B11111000,
  B01000000,
  B00111000,
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
  B00110000,
  B01010000,
  B10010000,
  B11111000,
  B00010000,
  B00010000,
  6,


  B11111000,  //5
  B10000000,
  B11110000,
  B00001000,
  B00001000,
  B10001000,
  B01110000,
  6,


  B00110000,  //6
  B01000000,
  B10000000,
  B11110000,
  B10001000,
  B10001000,
  B01110000,
  6,


  B11111000,  //7
  B10001000,
  B00001000,
  B00010000,
  B00100000,
  B00100000,
  B00100000,
  6,


  B01110000,  //8
  B10001000,
  B10001000,
  B01110000,
  B10001000,
  B10001000,
  B01110000,
  6,


  B01110000,  //9
  B10001000,
  B10001000,
  B01111000,
  B00001000,
  B00010000,
  B01100000,
  6,


  B00000000,  //:
  B11000000,
  B11000000,
  B00000000,
  B11000000,
  B11000000,
  B00000000,
  3,


  B00000000,  //;
  B11000000,
  B11000000,
  B00000000,
  B11000000,
  B01000000,
  B10000000,
  3,


  B00010000,  //<
  B00100000,
  B01000000,
  B10000000,
  B01000000,
  B00100000,
  B00010000,
  5,


  B00000000,  //=
  B00000000,
  B11111000,
  B00000000,
  B11111000,
  B00000000,
  B00000000,
  6,


  B10000000,  //>
  B01000000,
  B00100000,
  B00010000,
  B00100000,
  B01000000,
  B10000000,
  5,


  B01110000,  //?
  B10001000,
  B00001000,
  B00010000,
  B00100000,
  B00000000,
  B00100000,
  6,


  B01110000,  //@
  B10001000,
  B00001000,
  B01101000,
  B10101000,
  B10101000,
  B01110000,
  6,


  B01110000,  //A
  B10001000,
  B10001000,
  B10001000,
  B11111000,
  B10001000,
  B10001000,
  6,


  B11110000, //B
  B10001000,
  B10001000,
  B10110000,
  B10001000,
  B10001000,
  B11110000,
  6,


  B01110000,  //C
  B10001000,
  B10000000,
  B10000000,
  B10000000,
  B10001000,
  B01110000,
  6,


  B11000000,  //D(В)
  B10100000,
  B10010000,
  B10010000,
  B10010000,
  B10100000,
  B11000000,
  6,


  B11111000,  //E(У)
  B10000000,
  B10000000,
  B11111000,
  B10000000,
  B10000000,
  B11111000,
  6,


  B11111000,  //F(А)
  B10000000,
  B10000000,
  B11111000,
  B10000000,
  B10000000,
  B10000000,
  6,


  B01110000,  //G(П)
  B10001000,
  B10000000,
  B10111000,
  B10001000,
  B10001000,
  B01110000,
  6,


  B10001000,  //H(Р)
  B10001000,
  B10001000,
  B11111000,
  B10001000,
  B10001000,
  B10001000,
  6,


  B11111000,  //I(Ш)
  B00100000,
  B00100000,
  B00100000,
  B00100000,
  B00100000,
  B11111000,
  6,


  B01111110,  //J(О)
  B00001000,
  B00001000,
  B00001000,
  B00001000,
  B10001000,
  B01110000,
  6,


  B10001000,  //K(Л)
  B10010000,
  B10100000,
  B11000000,
  B10100000,
  B10010000,
  B10001000,
  6,


  B10000000,  //L
  B10000000,
  B10000000,
  B10000000,
  B10000000,
  B10000000,
  B11111000,
  6,


  B10001000,  //M
  B11011000,
  B10101000,
  B10101000,
  B10001000,
  B10001000,
  B10001000,
  6,


  B10001000,  //N(Т)
  B11001000,
  B10101000,
  B10011000,
  B10011000,
  B10001000,
  B10001000,
  6,


  B01110000,  //O
  B10001000,
  B10001000,
  B10001000,
  B10001000,
  B10001000,
  B01110000,
  6,


  B11110000,  //P
  B10001000,
  B10001000,
  B11110000,
  B10000000,
  B10000000,
  B10000000,
  6,


  B01110000,  //Q(Й)
  B10001000,
  B10001000,
  B10001000,
  B10101000,
  B10011000,
  B01110110,
  6,


  B11100000,  //R(К)
  B10010000,
  B10010000,
  B11100000,
  B10100000,
  B10010000,
  B10001000,
  6,


  B01111000,  //S
  B10000000,
  B10000000,
  B01110000,
  B00001000,
  B00001000,
  B11110000,
  6,


  B11111000,  //T(Е)
  B00100000,
  B00100000,
  B00100000,
  B00100000,
  B00100000,
  B00100000,
  6,


  B10001000,  //U(Г)
  B10001000,
  B10001000,
  B10001000,
  B10001000,
  B10001000,
  B01110000,
  6,


  B10001000,  //V(М)
  B10001000,
  B10001000,
  B10001000,
  B01010000,
  B01010000,
  B00100000,
  6,


  B10001000,  //W
  B10001000,
  B10001000,
  B10101000,
  B10101000,
  B10101000,
  B01010000,
  6,


  B10001000,  //X
  B10001000,
  B01010000,
  B00100000,
  B01010000,
  B10001000,
  B10001000,
  6,


  B10001000,  //Y(Н)
  B10001000,
  B01010000,
  B00100000,
  B00100000,
  B00100000,
  B00100000,
  6,


  B11111100,  //Z(Я)
  B00001000,
  B00010000,
  B00100000,
  B01000000,
  B10000000,
  B11111000,
  6,


  B00000000,  //[(х)
  B00000000,
  B10001000,
  B01010000,
  B00100000,
  B01010000,
  B10001000,
  6,


  B00000000,  //(Backward Slash)
  B10000000,
  B01000000,
  B00100000,
  B00010000,
  B00001000,
  B00000000,
  6,


  B11100000,  //]
  B00100000,
  B00100000,
  B00100000,
  B00100000,
  B00100000,
  B11100000,
  4,


  B00100000,  //^
  B01010000,
  B10001000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  6,


  B00000000,  //_
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B11111000,
  6,


  B10000000,  //`
  B01000000,
  B00100000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  4,


  B00000000,  //a
  B00000000,
  B01110000,
  B00001000,
  B01111000,
  B10001000,
  B01111000,
  6,


  B00000000,  //b(и)
  B00000000,
  B10001000,
  B10011000,
  B10101000,
  B11001000,
  B10001000,
  6,


  B00000000,  //c
  B00000000,
  B01110000,
  B10001000,
  B10000000,
  B10001000,
  B01110000,
  6,


  B00001000,  //d
  B00001000,
  B01101000,
  B10011000,
  B10001000,
  B10001000,
  B01111000,
  6,


  B00000000,  //e(у)
  B00000000,
  B10001000,
  B10001000,
  B11111000,
  B00001000,
  B01110000,
  6,


  B00000000,  //f(а)
  B00000000,
  B01110000,
  B00001000,
  B01111000,
  B10001000,
  B01111000,
  6,


  B00000000,  //g(п)
  B00000000,
  B11110000,
  B10001000,
  B10001000,
  B10001000,
  B10001000,
  6,


  B10000000,  //h
  B10000000,
  B10110000,
  B11001000,
  B10001000,
  B10001000,
  B10001000,
  6,


  B01000000,  //i
  B00000000,
  B11000000,
  B01000000,
  B01000000,
  B01000000,
  B11100000,
  4,


  B00000000,  //j(о)
  B00000000,
  B01110000,
  B10001000,
  B10001000,
  B10001000,
  B01110000,
  6,


  B00000000,  //k(л)
  B00000000,
  B00111000,
  B01001000,
  B01001000,
  B01001000,
  B10001000,
  6,


  B11000000,  //l
  B01000000,
  B01000000,
  B01000000,
  B01000000,
  B01000000,
  B11100000,
  4,


  B00000000,  //m
  B00000000,
  B11010000,
  B10101000,
  B10101000,
  B10001000,
  B10001000,
  6,


  B00000000,  //n(т)
  B00000000,
  B11111000,
  B00100000,
  B00100000,
  B00100000,
  B00100000,
  6,


  B00000000,  //o
  B00000000,
  B01110000,
  B10001000,
  B10001000,
  B10001000,
  B01110000,
  6,


  B00000000,  //p
  B00000000,
  B11110000,
  B10001000,
  B11110000,
  B10000000,
  B10000000,
  6,


  B00000000,  //q(й)
  B01110000,
  B10001000,
  B10011000,
  B10101000,
  B11001000,
  B10001000,
  6,


  B00000000,  //r
  B00000000,
  B10110000,
  B11001000,
  B10000000,
  B10000000,
  B10000000,
  6,


  B00000000,  //s
  B00000000,
  B01110000,
  B10000000,
  B01110000,
  B00001000,
  B11110000,
  6,


  B01000000,  //t
  B01000000,
  B11100000,
  B01000000,
  B01000000,
  B01001000,
  B00110000,
  6,


  B00000000,  //u
  B00000000,
  B10001000,
  B10001000,
  B10001000,
  B10011000,
  B01101000,
  6,


  B00000000,  //v
  B00000000,
  B10001000,
  B10001000,
  B10001000,
  B01010000,
  B00100000,
  6,


  B00000000,  //w
  B00000000,
  B10001000,
  B10101000,
  B10101000,
  B10101000,
  B01010000,
  6,


  B00000000,  //x
  B00000000,
  B10001000,
  B01010000,
  B00100000,
  B01010000,
  B10001000,
  6,


  B00000000,  //y(н)
  B00000000,
  B10001000,
  B10001000,
  B11111000,
  B10001000,
  B10001000,
  6,


  B00000000,  //z
  B00000000,
  B11111000,
  B00010000,
  B00100000,
  B01000000,
  B11111000,
  6,


  B10001000,  //{(Х)
  B10001000,
  B01010000,
  B00100000,
  B01010000,
  B10001000,
  B10001000,
  6,


  B10000000,  //|
  B10000000,
  B10000000,
  B10000000,
  B10000000,
  B10000000,
  B10000000,
  2,


  B10000000,  //}
  B01000000,
  B01000000,
  B00100000,
  B01000000,
  B01000000,
  B10000000,
  4,


  B00000000,  //~
  B00000000,
  B00000000,
  B01101000,
  B10010000,
  B00000000,
  B00000000,
  6,


  B01100000,  // (Char 0x7F)
  B10010000,
  B10010000,
  B01100000,
  B00000000,
  B00000000,
  B00000000,
  5,



};

void serverCode(){
  WiFiClient client = server.available();
  if(client){
    Serial.println("New Client");
    //while (client.connected()) {
      //if (client.available()) {
        String request = client.readStringUntil('\r');
        Serial.println("Request: " + request);

        String weerTeGevenTekst;
        if (request.indexOf("lichtkrant:") != -1) {
        weerTeGevenTekst = request.substring(12);
        weerTeGevenTekst.toCharArray(receivedText, 50);
        weerTeGevenTekst[weerTeGevenTekst.length()] = '\0';
        scrollMessage((const unsigned char *)weerTeGevenTekst.c_str());
        }
        delay(100);

        client.stop();
        Serial.println("Client disconnected");
   // }
  //}
}

}

void clientCode(){
  if (WiFi.status() == WL_CONNECTED) {
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
      Serial.println("Connected to server");
      
      client.println("Hello from Wemos Lolin Mini!");
      delay(150);

      while (client.available() > 0) {
        String response = client.readStringUntil('\r');
        Serial.println(response);
      }

      client.stop();
    } else {
      Serial.println("Connection to server failed");
    }
    
    delay(500);
  }

}

void clientCodeMetSend(String toSend) {
  if (WiFi.status() == WL_CONNECTED) {
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
      Serial.println("Connected to server");
      
      client.println(toSend.c_str());
      delay(150);

      client.stop();
    } else {
      Serial.println("Connection to server failed");
    }
    
    delay(500);
  }
            digitalWrite(LEDPIN, LOW);

}

void scrollFont() {
  for (int counter = 0x20; counter < 0x80; counter++) {
    loadBufferLong(counter);
    delay(500);
  }
}

void scrollMessage(const unsigned char * messageString) {
  int counter = 0;
  int myChar = 0;
  do {
    myChar =  pgm_read_byte_near(messageString + counter);
    if (myChar != 0) {
      loadBufferLong(myChar);
    }
    counter++;
  }
  while (myChar != 0);
}

void loadBufferLong(int ascii) {
  if (ascii >= 0x20 && ascii <= 0xff) {
    for (int a = 0; a < 7; a++) {
      unsigned long c = pgm_read_byte_near(font5x7 + ((ascii - 0x20) * 8) + a);
      unsigned long x = bufferLong[a * 2];
      x = x | c;
      bufferLong[a * 2] = x;
    }
    byte count = pgm_read_byte_near(font5x7 + ((ascii - 0x20) * 8) + 7);
    for (byte x = 0; x < count; x++) {
      rotateBufferLong();
      printBufferLong();
      delay(scrollDelay);
    }
  }
}

void rotateBufferLong() {
  for (int a = 0; a < 7; a++) {
    unsigned long x = bufferLong[a * 2];
    byte b = bitRead(x, 31);
    x = x << 1;
    bufferLong[a * 2] = x;
    x = bufferLong[a * 2 + 1];
    x = x << 1;
    bitWrite(x, 0, b);
    bufferLong[a * 2 + 1] = x;
  }
}

void printBufferLong() {
  for (int a = 0; a < 7; a++) {
    unsigned long x = bufferLong[a * 2 + 1];
    byte y = x;
    lc.setRow(3, a, y);
    x = bufferLong[a * 2];
    y = (x >> 24);
    lc.setRow(2, a, y);
    y = (x >> 16);
    lc.setRow(1, a, y);
    y = (x >> 8);
    lc.setRow(0, a, y);
  }
}

/*
Overzicht van de werking
- scrollFont(): Loopt door een reeks ASCII-tekens en laadt ze één voor één in de buffer met een vertraging.
- scrollMessage(const unsigned char *messageString): Loopt door een string en laadt elk karakter in de buffer.
- loadBufferLong(int ascii): Laadt het bitpatroon van een ASCII-teken in de buffer en rotert de buffer voor animatie.
- rotateBufferLong(): Verschijft de bits in de buffer voor het scroll-effect.
- printBufferLong(): Schrijft de inhoud van de buffer naar de display. */
