#include <ESP8266WiFi.h> // library voor de raspberry pi
#include <WiFiClient.h>


const char* ssid = "NSELab";
const char* password = "NSELabWiFi";
const char* serverIPaddress = "145.52.127.169"; // IP address van de Pi 
const int port = 8080; // Port voor server

WiFiServer server(port);

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
  sendIP();
}

void sendIP(){
  IPAddress serverIP(145, 52, 127, 169);
  
  WiFiClient client;
  if (client.connect(serverIP, port)){
    Serial.println("Connected to server");
  
  // Send a message to the server
  IPAddress localIP = WiFi.localIP();
  char ipFull[16] = "";
  sprintf(ipFull, "Wemos1 %d.%d.%d.%d", localIP[0], localIP[1],localIP[2],localIP[3]);

  client.println(ipFull);
  delay(150);
  client.stop();
  }
  else {
    Serial.println("Unable to connect to server");
  }
}

void loop() {

}

void serverCode(){
  WiFiClient client = server.available();

  if(client){
    Serial.println("New Client");
    // Wait for data from the client
    while (client.connected()) {
      if (client.available()) {
        // Read data from the client
        String request = client.readStringUntil('\r');
        Serial.println("Request: " + request);

        // Send response back to the client
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println();
        client.println("<h1>Hello from Wemos Lolin Mini!</h1>");
        client.println();

        delay(100); // Allow time for client to receive response

        // Close the connection
        client.stop();
        Serial.println("Client disconnected");
      }
    }
  }
}

void clientCode(){
  if (WiFi.status() == WL_CONNECTED) {
    // IP address of the server you want to connect to
    IPAddress serverIP(145, 52, 127, 169);
  
    WiFiClient client;
    
    // Connect to the server
    if (client.connect(serverIP, port)) {
      Serial.println("Connected to server");
      
      // Send a message to the server
      client.println("Hello from Wemos Lolin Mini!");
      delay(150);
      // Wait for response from the server
      while (client.available()> 0) {
        String response = client.readStringUntil('\r');
        Serial.println(response);
      }

      // Close the connection
      client.stop();
    } else {
      Serial.println("Connection to server failed");
    }
    
    delay(5000);  // Wait for a few seconds before connecting again
  }
}