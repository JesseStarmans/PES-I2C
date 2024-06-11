#define LED_PIN     D0 // pin waar de led op aangesloten is
#define NUM_LEDS    8
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define VERGROTING 10 // vergroting per helderheidsknop ingedrukt
#define OMHOOG D2 // helderheid omhoog ledstrip
#define OMLAAG D3 // helderheid omlaag ledstrip
#define KLEURWISSEL D1 // ledstrip aan/uit
#define UPDATES_PER_SECOND 100

#include <ESP8266WiFi.h> // library voor de raspberry pi
#include <WiFiClient.h>
#include <avr/pgmspace.h>
#include <LedControl.h>
#include <string>
#include <FastLED.h>

// const char* ssid = "NSELab";
// const char* password = "NSELabWiFi";
// const char* serverIPaddress = "145.52.127.184"; // IP address van de Pi 

const char* ssid = "PiNetGroepG";
const char* password = "GroepGNet";
const char* serverIPaddress = "10.42.0.251"; // IP address van de Pi 


const int port = 8080; // Port voor server
const int serverPort = 6060;

int rodepin = 14;
int groenepin = 12;
int blauwepin = 13;
int waarde = 0;
int status = 0;
int colorIndex = 0; // Index voor de huidige kleur
const unsigned long debounceDelay = 100; // verhoog de debounce delay

bool sendValue = false; 

uint8_t helderheid = 128; 

unsigned long lastButtonPress = 0;

CRGB leds[NUM_LEDS];
const CRGB HTMLColors[] = {
    CRGB::Red, CRGB::Orange, CRGB::Yellow, CRGB::Green, CRGB::Blue, CRGB::Indigo, CRGB::Violet, CRGB::White, CRGB::Black
};

WiFiServer server(serverPort);

void setup() {
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(helderheid);
    fill_solid(leds, NUM_LEDS, HTMLColors[colorIndex]);  

    FastLED.show();
    
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    pinMode(OMHOOG, INPUT_PULLUP);
    pinMode(OMLAAG, INPUT_PULLUP);
    pinMode(KLEURWISSEL, INPUT_PULLUP);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }

    Serial.println("Connected to WiFi");

    
    server.begin();
    Serial.println(WiFi.localIP());
    // set LED pins op output
    pinMode(rodepin, OUTPUT);
    pinMode(groenepin,OUTPUT);
    pinMode(blauwepin,OUTPUT);

    // zend ip adres naar server
    sendIP("Wemos2");

    // Lees bij start waarde uit
    waarde = analogRead(A0);
    if (waarde >= 500) {
      setColor(255, 0, 0); // Rood 
      status = 2;
    } else{
      setColor(0, 255, 0); // Groen
      status = 1;
    }

    Serial.println("Waiting for request");
}


// Automatisch eigen ip naar server sturen
void sendIP(String name) {
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
        IPAddress localIP = WiFi.localIP();
        char ipFull[25] = "";
        sprintf(ipFull, "%s %d.%d.%d.%d", name, localIP[0], localIP[1], localIP[2], localIP[3]);
        client.println(ipFull);
        delay(150);
        client.stop();
    } else {
        Serial.println("Unable to connect to server");
    }
}

void loop() { 
    serverCode();
    ledstripaansturen();
    FastLED.show();
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Reconnecting...");
        while (!WiFi.reconnect()) {}
    }
}

void serverCode() {
  WiFiClient client = server.available();
  FastLED.show();
  if (client) {
    String request = client.readStringUntil('\r');
    
    if (request == "Plant") {
      waarde = analogRead(A0);
      // hoge waarde == droog, lage waarde == nat
      if (waarde >= 500) {
        setColor(255, 0, 0); // droog = Rode led
        status = 2;
      } else{
        setColor(0, 255, 0); // nat = Groene led
        status = 1;
      }
      String deel1 = "RPl: " + String(status);

      clientCodeMetSend(deel1);
      
    }
       else if (request.indexOf("LEDstrip:") != -1) {
          String kleurqt = request.substring(10);
          colorIndex = kleurqt.toInt();
        fill_solid(leds, NUM_LEDS, HTMLColors[colorIndex]);
        FastLED.show();
        }


    delay(100);
    client.stop();
  }
}

// Zend data naar PI_B
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
            client.println(toSend.c_str());
            delay(150);

            client.stop();
        } else {
            Serial.println("Connection to server failed");
        }
        
        delay(100);
    }
}

// set RGB colors
void setColor(int redValue, int greenValue, int blueValue) { 
    analogWrite(rodepin, redValue);
    analogWrite(groenepin, greenValue);
    analogWrite(blauwepin, blueValue);
}

void ledstripaansturen() {
    unsigned long currentMillis = millis();
    if ((currentMillis - lastButtonPress) > debounceDelay) {
        if (digitalRead(OMHOOG) == LOW) {
           if((int)helderheid + VERGROTING >= 250){
              helderheid = 250;
           }
           else{
            helderheid += VERGROTING;
           }
           
           
            
            FastLED.setBrightness(helderheid);
            FastLED.show();
            Serial.println("Helderheid omhoog: " + String((int)FastLED.getBrightness()));
            lastButtonPress = currentMillis;
           
        }

        if (digitalRead(KLEURWISSEL) == LOW) {
            colorIndex = (colorIndex + 1) % (sizeof(HTMLColors) / sizeof(HTMLColors[0]));
            fill_solid(leds, NUM_LEDS, HTMLColors[colorIndex]);
            Serial.println("Kleur wisselen: " + String(colorIndex));
            FastLED.show();
            lastButtonPress = currentMillis;
        }

        if (digitalRead(OMLAAG) == LOW) {
           if((int)helderheid - VERGROTING <= 0){
              helderheid = 0;
           }
           else{
            helderheid -= VERGROTING;
           }
            
            FastLED.setBrightness(helderheid);
            Serial.println("Helderheid omlaag: " + String((int)FastLED.getBrightness()));
            FastLED.show();
            lastButtonPress = currentMillis;
        }
    }
    
}
