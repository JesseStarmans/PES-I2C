/*!
    @file WemosClient.h
    @brief Header file voor de WemosClient subklasse.

    Deze header file definieert de WemosClient subklasse voor het verzenden van data
    vanaf de Wemos naar de Raspberry Pi server. Er wordt hier overgeërft vanuit de WiFiClient klasse.
*/
#ifndef WEMOSCLIENT_H
#define WEMOSCLIENT_H

#include <WiFiClient.h>
#include <ESP8266WiFi.h>

/*!
    @class WemosClient
    @brief Een klasse om data te versturen naar een server.
*/
class WemosClient : public WiFiClient
{
public:
    WemosClient(IPAddress, const int);
    ~WemosClient();
    bool connect();
    void sendIP(String);
    void sendInput(String);
private:
    IPAddress IP;           /*!< Het IP adres van de server*/
    int port;               /*!< De port van de server*/
};

#endif //WEMOSCLIENT_H