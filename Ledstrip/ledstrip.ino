#include <ESP8266WiFi.h>
#include <FastLED.h>
#include "WemosServer.h"
#include "WemosClient.h"

#define LED_PIN     D4 // pin waar de led op aangesloten is
#define NUM_LEDS    20
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define VERGROTING 10 // vergroting per helderheidsknop ingedrukt
#define OMHOOG D2 // helderheid omhoog ledstrip
#define OMLAAG D3 // helderheid omlaag ledstrip
#define KLEURWISSEL D1 // ledstrip aan/uit

int rodepin = 14;
int groenepin = 12;
int blauwepin = 13;
int waarde = 0;
int helderheid = 128; 

///SSID van het WiFi netwerk om mee te verbinden.
const char* ssid = "NSELab";

///Wachtwoord van het WiFi netwerk om mee te verbinden.
const char* password = "NSELabWiFi";

///IP address van de Raspberry Pi server.
const char* serverIPaddress = "145.52.127.184";

///Port van de Raspberry Pi server.
const int port = 8080;

///Port voor eigen server.
const int serverPort = 6060;

///Bool voor het versturen van de waarde
bool sendValue = false; 

unsigned long lastButtonPress = 0;
const unsigned long debounceDelay = 200;

IPAddress getIPAddress();

WemosServer server(serverPort);             /*!< Globale aanmaak van de WemosServer*/
WemosClient client(getIPAddress(), port);   /*!< Globale aanmaak van de WemosClient*/

int colorIndex = 0; // Index voor de huidige kleur

// Array van HTML kleuren
const CRGB HTMLColors[] = {
  CRGB::Red, CRGB::Orange, CRGB::Yellow, CRGB::Green, CRGB::Blue, CRGB::Indigo, CRGB::Violet, CRGB::White, CRGB::Black
};

/*!
  @brief De setup code die één keer wordt uitgevoerd.
*/
void setup() {
  Serial.begin(115200);

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(helderheid);
  fill_solid(leds, NUM_LEDS, HTMLColors[colorIndex]);  
  FastLED.show();
  
  pinMode(OMHOOG, INPUT_PULLUP);
  pinMode(OMLAAG, INPUT_PULLUP);
  pinMode(AANUIT, INPUT_PULLUP);

  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to the WiFi");
    delay(1000);
  }

  Serial.println("Connected to the WiFi");

  server.begin();
  Serial.println("Server started");
  Serial.println("IP: ");
  Serial.println(WiFi.localIP());

  
  client.sendIP("Wemos1");
  client.sendIP("Wemos2");
  client.sendIP("Wemos3");

  Serial.println("Waiting for request");
}

/*!
  @brief Maakt een automatisch een IPAddress object aan.

  Deze functie maakt automatisch een IPAddress object aan, aan de hand van het serverIPaddress dat bovenaan gedefinieert staat.

  @return Geeft het IPAddress object terug om te gebruiken in de WemosClient aanmaak.
*/
IPAddress getIPAddress() {
  int ipDelen[4];
  int i = 0;
  String tempString = serverIPaddress;
  String bufferIP;
  while (tempString.length() > 0) {
    bufferIP = tempString.substring(0, tempString.indexOf('.'));
    ipDelen[i++] = bufferIP.toInt();
    tempString = tempString.substring(tempString.indexOf('.') + 1);
  }
  return IPAddress(ipDelen[0], ipDelen[1], ipDelen[2], ipDelen[3]);
}

/*!
  @brief De loop waarin de code herhaald blijft worden.
*/
void loop() {
  //serverCode();
  ledstripaansturen();
  ledaansturen();
  if (sendValue) {
    clientCodeMetSend(String(waarde));
    delay(10);
    sendValue = false; 
  }
}

void clientCodeMetSend(String toSend) {
  if (WiFi.status() == WL_CONNECTED) {
    // IP address of the server you want to connect to
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
      
      // Send a message to the server
      client.println(toSend.c_str());
      delay(150);

      // Close the connection
      client.stop();
    } else {
      Serial.println("Connection to server failed");
    }
    
    delay(1000);
  }
}

/*!
  @brief Checkt of er een client verbinding is binnengekomen.

  Deze functie checkt of er een nieuwe client verbinding is binnengekomen en leest dan de gestuurde string in.
*/
void serverCode() {
  server.checkForConnection();

  if (server.client) {
    while (server.client.connected()) {
      if (server.client.available()) {
        String request = server.receiveRequest();
        Serial.println("Request: " + request);
        if(request == "210"){
            sendValue = true; 
        }

      }
    }
  }
}

// led gedeelte
void setColor(int redValue, int greenValue, int blueValue) {
  analogWrite(rodepin, redValue);
  analogWrite(groenepin, greenValue);
  analogWrite(blauwepin, blueValue);
}

void ledaansturen() {
  waarde = analogRead(A0);
  //Serial.println(waarde);
  if (waarde < 400) {
    setColor(255, 0, 0); // Red Color
  } else if (waarde > 400 && waarde < 600) {
    setColor(255, 128, 0); // oranje Color
  } else if (waarde > 600 && waarde < 700) {
    setColor(0, 255, 0); // groen Color
  }
}

void ledstripaansturen(){
  unsigned long currentMillis = millis();
  if ((currentMillis - lastButtonPress) > debounceDelay) {
    if (digitalRead(OMHOOG) == LOW) {
      helderheid += VERGROTING;
      if (helderheid > 255) { // zorg voor een randwaarde
        helderheid = 255;
      }
      FastLED.setBrightness(helderheid);
      FastLED.show();
      lastButtonPress = currentMillis-100;
    }

    if (digitalRead(KLEURWISSEL) == LOW) {
      colorIndex = (colorIndex + 1) % (sizeof(HTMLColors) / sizeof(HTMLColors[0]));
      for (int i = 0; i < 255; i++) {
        fill_solid(leds, NUM_LEDS, blend(leds[0], HTMLColors[colorIndex], i));
        FastLED.show();
        delay(5); 
      }
      lastButtonPress = currentMillis;
    }

    if (digitalRead(OMLAAG) == LOW) {
      helderheid -= VERGROTING;
      if (helderheid < 0) { // zorg voor een randwaarde
        helderheid = 0;
      }
      FastLED.setBrightness(helderheid);
      FastLED.show();
      lastButtonPress = currentMillis-100;
    }

  
  }
}
