#include <ESP8266WiFi.h>
#include "WemosServer.h"
#include "WemosClient.h"

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

IPAddress getIPAddress();

WemosServer server(serverPort);             /*!< Globale aanmaak van de WemosServer*/
WemosClient client(getIPAddress(), port);   /*!< Globale aanmaak van de WemosClient*/

/*!
  @brief De setup code die één keer wordt uitgevoerd.
*/
void setup() {
  Serial.begin(115200);

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
  serverCode();
  if (Serial.available() > 0) {
    String inlees = Serial.readStringUntil('\n');
    Serial.println("Ontvangen data: " + inlees);
    client.sendInput(inlees);
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
        
        server.client.println("200 OK");      //Pas dit aan om iets te doen met de ingelezen string.

        delay(100);

        server.client.stop();
        Serial.println("Server client has been disconnected");
      }
    }
  }
}
