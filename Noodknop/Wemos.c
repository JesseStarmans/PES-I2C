#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "ssid";
const char* password = "wachtwoord";
const char* serverName = "http://ip-rasPi/noodKnop";

WiFiClient client;
HTTPClient http;

void setup() {
  Serial.begin(115200);
  pinMode(D1, INPUT_PULLUP); // Stel de knop in op pin D1

// Verbinden met wifi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  int buttonState = digitalRead(D1);
  if (buttonState == LOW) { // Check of de knop is ingedrukt
    if (http.begin(client, serverName)) {
      int httpCode = http.GET();  // Verzend een HTTP request
      if (httpCode > 0) {
        String payload = http.getString(); // Ontvang bericht van de server
        Serial.println(payload); 
      } else {
        Serial.println("Error on sending GET request");
      }
      http.end();
    }
    delay(1000); // Anti dender
  }
}
