#include <ESP8266WiFi.h> // library voor de raspberry pi
#include <WiFiClient.h>
#include <avr/pgmspace.h>
#include <LedControl.h>
#include <string>


const char* ssid = "NSELab";
const char* password = "NSELabWiFi";
const char* serverIPaddress = "145.52.127.184"; // IP address van de Pi 

// const char* ssid = "PiNetGroepG";
// const char* password = "GroepGNet";
// const char* serverIPaddress = "10.42.0.251"; // IP address van de Pi 
const int port = 8080; // Port voor server
const int serverPort = 6060;


int rodepin = 14;
int groenepin = 12;
int blauwepin = 13;
int waarde = 0;

int status = 0;
bool sendValue = false; 

WiFiServer server(serverPort);

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }

    Serial.println("Connected to WiFi");

    
    server.begin();
    Serial.println("Server started");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    
   
    sendIP("Wemos1");
    sendIP("Wemos2");
    sendIP("Wemos3");
    sendIP("Wemos4");
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
    serverCode();
      
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Reconnecting...");
        while (!WiFi.reconnect()) {}
    }

    if (Serial.available() > 0) { //dit vindt de ledstrip niet leuk als je helderheid omhoog of omlaag wilt zetten.
        String inlees = Serial.readStringUntil('\n');
        Serial.println("Ontvangen data: " + inlees);
        clientCodeMetSend(inlees);
    }

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
            //if (client.available()) {
                Serial.println("Test1");
                String request = client.readStringUntil('\r');
                Serial.println("Request: " + request);
                if (request == "Plant") {
                  Serial.println("in plant");
                    waarde = analogRead(A0);
                    if (waarde < 500) {
                        setColor(255, 0, 0);
                        status = 1;
                    } else if (waarde >=500) {
                        setColor(0, 255, 0);
                        status = 2;
                    }
                    String deel1 = "RPlant: ";
                    String deel2 = String(status);
                    deel1 += deel2;
                    clientCodeMetSend(deel1);
                }
                delay(100);
                client.stop();
                Serial.println("Client disconnected");
           // }
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

void setColor(int redValue, int greenValue, int blueValue) {
    analogWrite(rodepin, redValue);
    analogWrite(groenepin, greenValue);
    analogWrite(blauwepin, blueValue);
}

