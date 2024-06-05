#include <FastLED.h>
#include <ESP8266WiFi.h> // library voor de raspberry pi
#include <WiFiClient.h>
#include <avr/pgmspace.h>
#include <LedControl.h>
#include <string>

#define LED_PIN     D4 // pin waar de led op aangesloten is
#define NUM_LEDS    8
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define VERGROTING 10 // vergroting per helderheidsknop ingedrukt
#define OMHOOG D2 // helderheid omhoog ledstrip
#define OMLAAG D3 // helderheid omlaag ledstrip
#define KLEURWISSEL D1 // ledstrip aan/uit
#define UPDATES_PER_SECOND 100

const char* ssid = "PiNetGroepG";
const char* password = "GroepGNet";
const char* serverIPaddress = "10.42.0.251"; // IP address van de Pi 
const int port = 8080; // Port voor server
const int serverPort = 6060;
const unsigned long debounceDelay = 200;

int rodepin = 14;
int groenepin = 12;
int blauwepin = 13;
int waarde = 0;
int helderheid = 128; 
int colorIndex = 0; // Index voor de huidige kleur
unsigned long lastButtonPress = 0;

bool sendValue = false; 

CRGB leds[NUM_LEDS];
const CRGB HTMLColors[] = {
    CRGB::Red, CRGB::Orange, CRGB::Yellow, CRGB::Green, CRGB::Blue, CRGB::Indigo, CRGB::Violet, CRGB::White, CRGB::Black
};

WiFiServer server(serverPort);

void setup() {
    Serial.begin(115200);
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(helderheid);
    fill_solid(leds, NUM_LEDS, HTMLColors[colorIndex]);  
    FastLED.show();
    
    pinMode(OMHOOG, INPUT);
    pinMode(OMLAAG, INPUT);
    pinMode(KLEURWISSEL, INPUT);
    pinMode(OMHOOG, INPUT_PULLUP);
    pinMode(OMLAAG, INPUT_PULLUP);
    pinMode(KLEURWISSEL, INPUT_PULLUP);
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
    sendIP("Wemos2");
    sendIP("Wemos3");
    Serial.println("Waiting for request");
    waarde = analogRead(A0);
}

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
        Serial.println("Connected to server");
        IPAddress localIP = WiFi.localIP();
        Serial.println(localIP);
        char ipFull[50] = "";
        sprintf(ipFull, "%s %d.%d.%d.%d", name, localIP[0], localIP[1], localIP[2], localIP[3]);
        Serial.println(ipFull);
        client.println(ipFull);
        delay(150);
        client.stop();
    } else {
        Serial.println("Unable to connect to server");
    }
}

void loop() { 
  
                    if (waarde < 400) {
                        setColor(255, 0, 0); // Red Color
                    } else if (waarde > 400 && waarde < 600) {
                        setColor(255, 128, 0); // oranje Color
                    } else if (waarde > 600 && waarde < 700) {
                        setColor(0, 255, 0); // groen Color
                    }
    ledstripaansturen();
     //ledaansturen();
    serverCode();
      
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Reconnecting...");
        while (!WiFi.reconnect()) {}
    }

    /*if (Serial.available() > 0) { //dit vindt de ledstrip niet leuk als je helderheid omhoog of omlaag wilt zetten.
        String inlees = Serial.readStringUntil('\n');
        Serial.println("Ontvangen data: " + inlees);
        clientCodeMetSend(inlees);
    }*/

    if (sendValue) {
        clientCodeMetSend(String(waarde));
        delay(10);
        sendValue = false; 
    }
}

void serverCode() {
    WiFiClient client = server.available();

    if (client) {
        Serial.println("New Client");
        while (client.connected()) {
            if (client.available()) {
                String request = client.readStringUntil('\r');
                Serial.println("Request: " + request);

                if (request == "Plant") {
                  waarde = analogRead(A0);
                    clientCodeMetSend(String(waarde));

                }
                
                delay(100);
                client.stop();
                Serial.println("Client disconnected");
            }
        }
    }
}

void clientCode() {
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
}

// led gedeelte
void setColor(int redValue, int greenValue, int blueValue) {
    analogWrite(rodepin, redValue);
    analogWrite(groenepin, greenValue);
    analogWrite(blauwepin, blueValue);
}

// void ledaansturen() {
  
// }

void ledstripaansturen() {
    unsigned long currentMillis = millis();
    if ((currentMillis - lastButtonPress) > debounceDelay) {
        if (digitalRead(OMHOOG) == LOW) {
            helderheid += VERGROTING;
            if (helderheid > 255) { // zorg voor een randwaarde
                helderheid = 255;
            }
            FastLED.setBrightness(helderheid);
            FastLED.getBrightness();
            Serial.println((int)FastLED.getBrightness());
            FastLED.show();
            lastButtonPress = currentMillis;
        }

        if (digitalRead(KLEURWISSEL) == LOW) {
            colorIndex = (colorIndex + 1) % (sizeof(HTMLColors) / sizeof(HTMLColors[0]));
            //for (int i = 0; i < 255; i++) {
                fill_solid(leds, NUM_LEDS, HTMLColors[colorIndex]);
                FastLED.show();
            lastButtonPress = currentMillis;
        }

        if (digitalRead(OMLAAG) == LOW) {
            if (helderheid >= VERGROTING) {
                helderheid -= VERGROTING;
            } else {
                helderheid = 0;
            }
            FastLED.setBrightness(helderheid);
            FastLED.show();
            lastButtonPress = currentMillis;
        }
    }
}
