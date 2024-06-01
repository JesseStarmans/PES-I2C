#include <FastLED.h>
#include <ESP8266WiFi.h> // library voor de ESP8266 microcontroller
#include <WiFiClient.h>
#include <avr/pgmspace.h>
#include <LedControl.h>
#include <string>

// Definities van pinnen en instellingen
#define LED_PIN     D4 // Pin waar de LED-strip op aangesloten is
#define NUM_LEDS    20
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define VERGROTING 10 // Helderheidstoename per knopdruk
#define OMHOOG D2 // Pin voor helderheid omhoog knop
#define OMLAAG D3 // Pin voor helderheid omlaag knop
#define KLEURWISSEL D1 // Pin voor kleurwissel knop
#define UPDATES_PER_SECOND 100

// WiFi-instellingen
const char* ssid = "PiNetGroepG";
const char* password = "GroepGNet";
const char* serverIPaddress = "10.42.0.251"; // IP-adres van de server
const int port = 8080; // Poort voor server
const int serverPort = 6060; // Poort voor lokale server
const unsigned long debounceDelay = 200; // Debounce vertraging voor knoppen

// Definities voor RGB pins en andere variabelen
int rodepin = 14;
int groenepin = 12;
int blauwepin = 13;
int waarde = 0;
int helderheid = 128; 
int colorIndex = 0; // Index voor de huidige kleur
unsigned long lastButtonPress = 0;
bool sendValue = false; 

// Initialisatie van LED-strip en kleuren
CRGB leds[NUM_LEDS];
const CRGB HTMLColors[] = {
    CRGB::Red, CRGB::Orange, CRGB::Yellow, CRGB::Green, CRGB::Blue, CRGB::Indigo, CRGB::Violet, CRGB::White, CRGB::Black
};

// Initialisatie van de WiFi server
WiFiServer server(serverPort);

void setup() {
    Serial.begin(115200); // Start seriële communicatie
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip); // Initialiseer LED-strip
    FastLED.setBrightness(helderheid); // Stel initiële helderheid in
    fill_solid(leds, NUM_LEDS, HTMLColors[colorIndex]);  
    FastLED.show();
    
    // Stel pinnen in voor knoppen
    pinMode(OMHOOG, INPUT_PULLUP);
    pinMode(OMLAAG, INPUT_PULLUP);
    pinMode(KLEURWISSEL, INPUT_PULLUP);
    WiFi.begin(ssid, password); // Verbind met WiFi

    // Wacht tot de WiFi-verbinding tot stand is gebracht
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }

    Serial.println("Connected to WiFi");

    // Start de TCP-server
    server.begin();
    Serial.println("Server started");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    
    // Stuur IP-adressen van verschillende clients naar de server
    sendIP("Wemos1");
    sendIP("Wemos2");
    sendIP("Wemos3");
    Serial.println("Waiting for request");
    waarde = analogRead(A0); // Lees analoge waarde
}

void sendIP(String name) {
    // Functie om IP-adres naar server te sturen
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
    // Controleer de waarde en stel de kleur in
    if (waarde < 400) { //Plantensensor waarde
        setColor(255, 0, 0); // Rode kleur
    } else if (waarde > 400 && waarde < 600) { //Plantensensor waarde
        setColor(255, 128, 0); // Oranje kleur
    } else if (waarde > 600 && waarde < 700) { //Plantensensor waarde
        setColor(0, 255, 0); // Groene kleur
    }
    ledstripaansturen(); // Aansturen van de ledstrip
    serverCode(); // Afhandelen van server communicatie
      
    // Herstel WiFi-verbinding indien verbroken
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Reconnecting...");
        while (!WiFi.reconnect()) {}
    }

    // Lees seriële invoer
    if (Serial.available() > 0) {
        String inlees = Serial.readStringUntil('\n');
        Serial.println("Ontvangen data: " + inlees);
        clientCodeMetSend(inlees);
    }

    // Stuur waarde indien nodig
    if (sendValue) {
        clientCodeMetSend(String(waarde)); //Stuurt de waarde door naar de server
        delay(10);
        sendValue = false; 
    }
}

void serverCode() {
    // Functie om inkomende client-verbindingen te verwerken
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
    // Functie om verbinding met server te maken en gegevens te verzenden
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
    // Functie om gegevens naar de server te verzenden
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

// Functie om kleur van de LEDs in te stellen
void setColor(int redValue, int greenValue, int blueValue) {
    analogWrite(rodepin, redValue); //Schrijf de kleur naar de rodepin
    analogWrite(groenepin, greenValue); //Schrijf de kleur naar de groenepin
    analogWrite(blauwepin, blueValue); //Schrijf de kleur naar de oranjepin
}

// Functie om LED-strip aan te sturen
void ledstripaansturen() {
    unsigned long currentMillis = millis();
    if ((currentMillis - lastButtonPress) > debounceDelay) {
        if (digitalRead(OMHOOG) == LOW) {
            helderheid += VERGROTING;
            if (helderheid > 255) { // Beperk helderheid tot maximaal 255
                helderheid = 255;
            }
            FastLED.setBrightness(helderheid);
            FastLED.show();
            lastButtonPress = currentMillis - 100;
        }

        if (digitalRead(KLEURWISSEL) == LOW) {
            colorIndex = (colorIndex + 1) % (sizeof(HTMLColors) / sizeof(HTMLColors[0]));
            for (int i = 0; i < 255; i++) {
                fill_solid(leds, NUM_LEDS, blend(leds[0], HTMLColors[colorIndex], i));
                FastLED.show();
            }
            lastButtonPress = currentMillis;
        }

        if (digitalRead(OMLAAG) == LOW) {
            helderheid -= VERGROTING;
            if (helderheid < 0) { // Beperk helderheid tot minimaal 0
                helderheid = 0;
            }
            FastLED.setBrightness(helderheid);
            FastLED.show();
            lastButtonPress = currentMillis - 100;
        }
    }
}
