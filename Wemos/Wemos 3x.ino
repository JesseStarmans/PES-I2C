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
  
  // sendIP functies bestaan om de server IP invulling te geven. Comment uit wat je niet gebruikt
  sendIP("Wemos1");
  sendIP("Wemos2");
  sendIP("Wemos3");
  Serial.println("Waiting for request");
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
        client.println("200 OK");
        
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

/*De volgende functie is voor het verzenden van string naar de Pi server*/
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
    
    // Connect to the server
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
    
    delay(5000);  // Wait for a few seconds before connecting again
  }
}