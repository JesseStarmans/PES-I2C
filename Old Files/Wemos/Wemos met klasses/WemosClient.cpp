/*!
    @file WemosClient.cpp
    @date 29 May 2024
    @author Jesse Starmans
    @brief CPP file voor de WemosClient subklasse.

    De implemtatie van de WemosClient subklasse.

    Deze klasse is een subklasse van de hoofdklasse WiFiClient.

    Created on 29 May 2024
    by Jesse Starmans.
*/
#include "WemosClient.h"

/*!
    @brief Constructor voor de subklasse WemosClient.

    Deze constructor maakt gebruik van de WiFiClient hoofdklasse om data te versturen naar een server
    waarvan het IP en de poort meegegeven wordt.

    @param ip Het IP adres van de socket server waarnaar deze client moet verbinden.
    @param p De poort van de socket server waarnaar deze client moet verbinden.
*/
WemosClient::WemosClient(IPAddress ip, const int p) : IP(ip), port(p) {
    
}

/*!
    @brief Destructor voor de subklasse WemosClient.

    Deze destructor sloopt de subklasse WemosClient.
*/
WemosClient::~WemosClient() {

}

/*!
    @brief Laat de socket client verbinden met de server.

    Deze functie zorgt ervoor dat de client verbinding maakt met de socket server.

    @return Geeft terug of de client verbonden is of niet.
*/
bool WemosClient::connect() {
    return WiFiClient::connect(IP, port);
}

/*!
    @brief Stuurt het IP van de Wemos naar de socket server.

    Deze functie verbindt met de server om zijn meegegeven string en zijn IP naar de server te sturen. Hierdoor kunnen de IP's
    van de Wemos bordjes automatisch zich aanmelden bij de server met hun IP. Hierdoor hoeft dit niet meer handmatig gedaan te worden.

    @param wemos De string die aangeeft welke Wemos dit is (1, 2 of 3)
*/
void WemosClient::sendIP(String wemos) {
    if (WiFi.status() == WL_CONNECTED) {
        if (connect()) {
            Serial.println("Connected to server");

            IPAddress localIP = WiFi.localIP();
            char ipFull[50] = "";
            sprintf(ipFull, "%s %d.%d.%d.%d", wemos.c_str(), localIP[0], localIP[1],localIP[2],localIP[3]);
            Serial.println(ipFull);
            WiFiClient::println(ipFull);
            //delay(150);
            WiFiClient::stop();
        }
        else {
            Serial.println("Unable to connect to server");
        }
    }
    WiFiClient::stop();
}

/*!
    @brief Stuurt de meegegeven string naar de server

    Deze functie stuurt de meegegeven string naar de server.

    @param in De string om naar de server te sturen.
*/
void WemosClient::sendInput(String in) {
    if (WiFi.status() == WL_CONNECTED) {
        if (connect()) {
            Serial.println("Connected to server");

            WiFiClient::println(in);
            delay(150);

            WiFiClient::stop();
        }
        else {
            Serial.println("Connection to server failed");
        }
    }
    delay(500);
}