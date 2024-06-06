//code voor ALLEEN de ledstrip, plant/grondsensor werkt hierbij nog niet. zie void servercode

#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <avr/pgmspace.h>
#include <LedControl.h>
#include <string>

#define LED_PIN     D0 // pin waar de led op aangesloten is
#define NUM_LEDS    8
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define VERGROTING 10 // vergroting per helderheidsknop ingedrukt
#define OMHOOG D2 // helderheid omhoog ledstrip
#define OMLAAG D3 // helderheid omlaag ledstrip
#define KLEURWISSEL D1 // ledstrip aan/uit
#define UPDATES_PER_SECOND 100

const char* ssid = "NSELab";
const char* password = "NSELabWiFi";
const char* serverIPaddress = "145.52.127.184"; // IP address van de Pi 
const int port = 8080; // Port voor server
const int serverPort = 6060;
const unsigned long debounceDelay = 300; // verhoog de debounce delay
int status = 0;

int rodepin = 14;
int groenepin = 12;
int blauwepin = 13;
int waarde = 0;
int helderheid = 128; 
int colorIndex = 0; // Index voor de huidige kleur
unsigned long lastButtonPress = 0;

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

    pinMode(rodepin, OUTPUT);
    pinMode(groenepin, OUTPUT);
    pinMode(blauwepin, OUTPUT);

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

    sendIP("Wemos1");
    sendIP("Wemos2");
    sendIP("Wemos3");
    sendIP("Wemos4");
    Serial.println("Waiting for request");
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
    ledstripaansturen();
    serverCode();
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Reconnecting...");
        while (!WiFi.reconnect()) {}
    }
}

void serverCode() { //servercode zonder de grondsensor code erbij toegevoegd.
    WiFiClient client = server.available();
    if (client) {
        Serial.println("New Client");
        String request = "";
        while (client.connected() && client.available()) {
            request += (char)client.read();
        }
        if (request.length() > 0) {
            Serial.println("Request: " + request);
        }
        client.stop();
        Serial.println("Client disconnected");
    }
}

// void serverCode() {
//     WiFiClient client = server.available();
//     if (client) {
//         Serial.println("New Client");
//         String request = "";
//         while (client.connected() && client.available()) {
//             request += (char)client.read();
//         }
//         if (request.length() > 0) {
//             Serial.println("Request: " + request);
//             if (request.indexOf("Plant") != -1) { // Use indexOf to avoid exact string matching issues
//                 int waarde = analogRead(A0);
//                 Serial.println("Waarde: " + String(waarde));
//                 if (waarde < 500) {
//                     setColor(255, 0, 0);
//                 } else {
//                     setColor(0, 255, 0);
//                 }
//                 String deel1 = "Rpan: ";
//                 String deel2 = String(waarde < 500 ? 1 : 2);
//                 client.println(deel1 + deel2);
//             }
//         }
//         client.stop();
//         Serial.println("Client disconnected");
//     }
// }

void setColor(int redValue, int greenValue, int blueValue) {
    analogWrite(rodepin, redValue);
    analogWrite(groenepin, greenValue);
    analogWrite(blauwepin, blueValue);
}

void ledstripaansturen() {
    unsigned long currentMillis = millis();
    if ((currentMillis - lastButtonPress) > debounceDelay) {
        if (digitalRead(OMHOOG) == LOW) {
            helderheid += VERGROTING;
            if (helderheid > 255) {
                helderheid = 255;
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
            if (helderheid >= VERGROTING) {
                helderheid -= VERGROTING;
            } else {
                helderheid = 0;
            }
            FastLED.setBrightness(helderheid);
            Serial.println("Helderheid omlaag: " + String((int)FastLED.getBrightness()));
            FastLED.show();
            lastButtonPress = currentMillis;
        }
    }
}
