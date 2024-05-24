/*!
	@file SocketClient.cpp
	@date 24 May 2024
	@author Jesse Starmans
	@brief CPP file voor de SocketClient klasse.

	De implementatie van de SocketClient klasse.

	Deze klasse maakt het mogelijk om een verbinding aan te gaan met een socket server, om vervolgens data te kunnen versturen en ontvangen.

	Created on 7 May 2024
	by Jesse Starmans
	Modified on 24 May 2024
	by Jesse Starmans
*/

#include "SocketClient.h"

#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

/*!
	@brief Constructor voor de SocketClient klasse

	Deze constructor maakt een SocketClient object aan en maakt de socket client aan.

	@param poort De poort van de socket server waarnaar verbonden moet worden.
	@param serverIPAddress Het IP adres van de socket server waarnaar verbonden moet worden.
*/
SocketClient::SocketClient(int poort, const char* serverIPAddress) : port(poort), IP(serverIPAddress) {
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == -1) {
		cout<<"Error maken clientSocket"<<endl;
		exit(EXIT_FAILURE);
	}
	
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(IP);
	serverAddress.sin_port = htons(port);
	
	if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
		cout<<"Error connecting to server."<<endl;
		exit(EXIT_FAILURE);
	}
}

/*!
	@brief Destructor voor de SocketClient klasse

	Deze destructor sluit de client verbinding naar de server.
*/
SocketClient::~SocketClient() {
	close(clientSocket);
}

/*!
	@brief Verstuurt een string naar de socket server.

	Deze functie stuurt een string naar de socket server.

	@param message De string die verstuurt moet worden naar de socket server.
*/
void SocketClient::sendData(const string& message) {
	if (send(clientSocket, message.c_str(), message.length(), 0) < 0) {
		cout<<"Error sending data"<<endl;
		return;
	}
	else {
		cout<<"Message send: "<<message<<endl;
	}
}

/*!
	@brief Ontvangt een string terug van de socket server.

	Deze functie ontvangt een string die door de socket server is gestuurd en returnt deze string.

	@return Een string met de ontvangen data.
*/
string SocketClient::receiveData() {
	string str = "";
	char buffer[1024] = "";
	ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
	if (bytesReceived < 0) {
		str = "Error";
	}
	else if (bytesReceived == 0) {
		str = "Disconnected";
	}
	else {
		str.assign(buffer);
		cout<<"Received data from server: "<<str<<endl;
	}
	return str;
}

