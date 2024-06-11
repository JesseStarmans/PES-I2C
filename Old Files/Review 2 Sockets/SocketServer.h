/*! 
	@file SocketServer.h
	@brief Header file voor de SocketServer klasse.

	Deze header file definieert de klasse voor het maken en gebruiken van een socket server.
*/

#include <iostream>
#include <arpa/inet.h>
#include <vector>

using namespace std;

/*!
	@class SocketServer
	@brief Een klasse om socket verbindingen mee op te vangen en te verwerken.
*/
class SocketServer {
public: 
	SocketServer(int, const char*); 
	~SocketServer();
	vector<string> setupWemosIP();
	void serverListen();
	void serverAccept();
	void sendData(const string&);
	string receiveData();
	void closeClientConnection();
private:
	int serverSocket;				/*!< File descriptor voor het openen van de socket server.*/
	sockaddr_in serverAddress;		/*!< File descriptor voor het IP van de socket server.*/
	int clientSocket;				/*!< File descriptor voor de ontvangen socket client verbinding.*/
	int port;						/*!< De poort waar de socket server op aan het luisteren is.*/
	const char* IP;					/*!< Het IP waar de socket server op aan het luisteren is.*/
};
