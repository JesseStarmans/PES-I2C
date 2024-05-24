/*!
	@file SocketClient.h
	@brief Header file voor de SocketClient klasse.

	Deze header file definieert de klasse voor het maken en gebruiken van een socket client.
*/

#include <iostream>
#include <arpa/inet.h>

using namespace std;

/*!
	@class SocketClient
	@brief Een klasse om een socket verbinding op te zetten naar een socket server.
*/
class SocketClient {
public:
	SocketClient(int, const char*);
	~SocketClient();
	void sendData(const string&);
	string receiveData();
private:
	int clientSocket;				/*!< File descriptor voor het openen van de socket client.*/
	sockaddr_in serverAddress;		/*!< File descriptor voor het IP van de socket server waarnaar de client zal verbinden.*/
	int port;						/*!< De poort waar de socket server op aan het luisteren is.*/
	const char* IP;					/*!< Het IP waar de socket server op aan het luisteren is.*/
};
