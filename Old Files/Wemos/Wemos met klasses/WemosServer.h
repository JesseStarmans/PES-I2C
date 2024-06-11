/*!
    @file WemosServer.h
    @brief Header file voor de WemosServer subklasse.

    Deze header file definieert de WemosServer subklasse voor het ontvangen van data
    vanaf de Raspberry Pi client naar de Wemos. Er wordt hier overgeërft vanuit de WiFiServer klasse.
*/
#ifndef WEMOSSERVER_H
#define WEMOSSERVER_H

#include <WiFiClient.h>
#include <ESP8266WiFi.h>

/*!
    @class WemosServer
    @brief Een klasse om data te ontvangen van een client.
*/
class WemosServer : public WiFiServer
{
public:
    WemosServer(uint16_t);
    ~WemosServer();
    void checkForConnection();
    String receiveRequest();
    WiFiClient client;          /*!< Een socket client waaraan de binnenkomende client verbinding aan verbonden wordt*/
private:
    
};

#endif //WEMOSSERVER_H