#include <ESP8266WiFi.h> // library voor de raspberry pi
#include <WiFiClient.h>
#include <avr/pgmspace.h>
#include <LedControl.h>
#include <string>

int rodepin= 14;
int groenepin = 12;
int  blauwepin = 13;
int waarde = 0;

const char* ssid = "NSELab";
const char* password = "NSELabWiFi";
const char* serverIPaddress = "145.52.127.184"; // IP address van de Pi 
const int port = 8080; // Port voor server
const int serverPort = 6060;

const int numDevices = 4;      // number of MAX7219s used
const long scrollDelay = 50;   // adjust scrolling speed
unsigned long bufferLong [14] = {0};

LedControl lc = LedControl(12, 14, 15, 4); //din cs


WiFiServer server(serverPort);

void setup() { 
  Serial.begin(115200);
   pinMode(rodepin, OUTPUT);
  pinMode(groenepin, OUTPUT);
  pinMode(blauwepin, OUTPUT);
  pinMode(A0,INPUT);
  //WiFi.begin(ssid, password);

  //  while (WiFi.status() != WL_CONNECTED) {
  //    delay(1000);
  //    Serial.println("Connecting to WiFi...");
  //  }

  Serial.println("Connected to WiFi");

  // start TCP server
  //server.begin();
  Serial.println("Server started");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  
  // sendIP functies bestaan om de server IP invulling te geven. Comment uit wat je niet gebruikt
  // sendIP("Wemos1");
  // sendIP("Wemos2");
  // sendIP("Wemos3");
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
  //serverCode();
  ledaansturen();
  //if (Serial.available() > 0) {
    //String inlees = Serial.readStringUntil('\n');
    //Serial.println("Ontvangen data: " + inlees);
    //clientCodeMetSend(inlees);
  //}
}

void serverCode(){
  WiFiClient client = server.available();

  if(client){
    Serial.println("New Client");
    while (client.connected()) {
      if (client.available()) {
        String request = client.readStringUntil('\r');
        Serial.println("Request: " + request);

        String weerTeGevenTekst;
        if (request.indexOf("lichtkrant:") != -1) {
          weerTeGevenTekst = request.substring(12);
        }
        
        delay(100);

        client.stop();
        Serial.println("Client disconnected");
    }
  }
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
    
    delay(5000);
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
    
    delay(5000);
  }
}
void setColor(int redValue, int greenValue,  int blueValue) {
  analogWrite(rodepin, redValue);
  analogWrite(groenepin,  greenValue);
  analogWrite(blauwepin, blueValue);
}
void ledaansturen(){
  waarde = analogRead(A0);
  Serial.println(waarde); 
  if(waarde < 400){
    setColor(255, 0, 0); // Red Color
  }
  else if(waarde > 400 && waarde < 600){
    setColor(128, 128, 0); // Red Color
  }
  else if(waarde > 600 && waarde < 700){
    setColor(0, 255, 0); // Red Color
  }
 
}
