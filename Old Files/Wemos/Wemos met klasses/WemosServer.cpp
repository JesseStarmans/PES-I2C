/*!
    @file WemosServer.cpp
    @date 29 May 2024
    @author Jesse Starmans
    @brief CPP file voor de WemosServer subklasse.

    De implemtatie van de WemosServer subklasse.

    Deze klasse is een subklasse van de hoofdklasse WiFiServer.

    Created on 29 May 2024
    by Jesse Starmans.
*/
#include "WemosServer.h"

/*!
    @brief Constructor voor de subklasse WemosServer.

    Deze constructor maakt gebruik van de WiFiServer hoofdklasse om data te ontvangen van clients.

    @param port De poort waarop deze server moet gaan luisteren.
*/
WemosServer::WemosServer(uint16_t port) : WiFiServer(port) {
    begin();
}

/*!
    @brief Destructor voor de subklasse WemosServer.

    Deze destructor stopt de WiFiServer en sloopt vervolgens de WemosServer.
*/
WemosServer::~WemosServer() {
    stop();
}

/*!
    @brief Checkt of er een client verbinding probeert te maken.

    Deze functie checkt of er een client verbinding probeert te maken met de server. Als dat zo is wordt die verbinding direct in de client opgeslagen wordt.
*/
void WemosServer::checkForConnection() {
    client = available();
    if (client) {
        Serial.println("Connection found");
    }
}

/*!
    @brief Checkt of er data gelezen kan worden van de client verbinding.

    Deze functie checkt of er data in te lezen is van de client en als dit zo is, leest deze de volledige string in die verstuurd is. 
    Deze string wordt dan weer teruggegeven.

    @return De string die verzonden is door de client die verbinding heeft gemaakt met deze server.
*/
String WemosServer::receiveRequest() {
    if (client.available()) {
        Serial.println("In if:");
        String request = client.readStringUntil('\r');
        Serial.println("Request: " + request);

        return request;
    }
    else {
        return "";
    }
}