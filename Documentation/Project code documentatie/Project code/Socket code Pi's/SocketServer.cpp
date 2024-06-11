/*!
	@file SocketServer.cpp
	@date 24 May 2024
	@author Jesse Starmans
	@brief CPP file voor de SocketServer klasse.

	De implementatie van de SocketServer klasse.

	Deze klasse maakt het mogelijk om een socket server aan te maken, waarnaar verbonden kan worden door socket clients om data te versturen en ontvangen.

	Created on 6 May 2024
	by Jesse Starmans. \n
	Modified on 7 May 2024
	by Jesse Starmans. \n
	Modified on 13 May 2024
	by Jesse Starmans. \n
	Modified on 24 May 2024
	by Jesse Starmans.
*/
#include "SocketServer.h"

#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

/*!
	@brief Constructor voor de SocketServer klasse.

	Deze constructor maakt een SocketServer object aan en maakt de socket server aan.

	@param poortNr De poort waar de socket server naar moet gaan luisteren.
	@param ipAddress Het IP adres waar de socket server naar moet gaan luisteren.
*/
SocketServer::SocketServer(int poortNr, const char* ipAddress) : port(poortNr), IP(ipAddress){
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1) {
		cout<<"Error creating socket"<<endl;
		exit(EXIT_FAILURE);
	}
	
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr(IP);
	serverAddress.sin_port = htons(port);
	
	if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
		cout<<"Error binding socket: "<<strerror(errno)<<endl;
		exit(EXIT_FAILURE);
	}
}

/*!
	@brief Destructor voor de SocketServer klasse.

	Deze destructor sluit mogelijk openstaande verbindingen met een client en sluit vervolgens de server.
*/
SocketServer::~SocketServer() {
	close(clientSocket);
	close(serverSocket);
}

/*!
	@brief Ontvangt de IP adressen van de Wemos bordjes en slaat deze op.

	Deze functie wacht tot de 3 Wemos bordjes hun IP adres hebben gestuurd. Deze worden vervolgens, aan de hand van een key die meegestuurd wordt, op volgorde in een vector gezet en deze wordt terug gegeven.

	@return Een vector<string> waar de IP adressen van de Wemos bordjes op volgorde in opgeslagen zijn.
*/
vector<string> SocketServer::setupWemosIP() {	
	serverListen();
	
	int wemosAantal = 3;
	
	string wemos1;
	string wemos2;
	string wemos3;
	
	for (int i = 0; i < wemosAantal; i++) {
		serverAccept();
		
		if (clientSocket < 0) {
			exit(EXIT_FAILURE);
		}
		
		string nummer = receiveData();
		if (nummer == "Error") {
			cout<<"Error receiving data. nummer: "<<nummer<<endl;
			exit(EXIT_FAILURE);
		}
		else if(nummer == "Disconnected") {
			cout<<"Client disconnected. nummer: "<<nummer<<endl;
			exit(EXIT_FAILURE);
		}
		else {
			string wemosSubString = "";
			string IPSubString= "";
			wemosSubString = nummer.substr(0, 6);
			IPSubString = nummer.substr(7);
			if (wemosSubString == "Wemos1") {
				wemos1 = IPSubString;
				cout<<"Wemos 1 ip: "<<wemos1<<endl;
			}
			else if (wemosSubString == "Wemos2") {
				wemos2 = IPSubString;
				cout<<"Wemos 2 ip: "<<wemos2<<endl;
			}
			else if (wemosSubString == "Wemos3") {
				wemos3 = IPSubString;
				cout<<"Wemos 3 ip: "<<wemos3<<endl;
			}
		}
	}

	vector<string> wemosIPs;
	wemosIPs.push_back(wemos1);
	wemosIPs.push_back(wemos2);
	wemosIPs.push_back(wemos3);
	
	return wemosIPs;
}

/*!
	@brief Zorgt dat de socket server gaat luisteren.

	Deze functie laat de socket server luisteren naar de eerder meegegeven IP en poort.
*/
void SocketServer::serverListen() {
	listen(serverSocket, 5);
	
	cout<<"Server listening on port "<<port<<endl;
}

/*!
	@brief Zorgt dat de socket server een socket client verbinding kan accepteren.

	Deze functie laat de socket server een verbindingsaanvraag van een socket client accepteren. 
*/
void SocketServer::serverAccept() {
	clientSocket = accept(serverSocket, nullptr, nullptr);
	if (clientSocket < 0) {
		cout<<"Error accepting connection"<<endl;
	}
}

/*!
	@brief Verstuurt data naar de socket client die op het moment verbonden is.

	Deze functie verstuurt een string naar de socket client die momenteel verbonden is met de socket server.

	@param message De string die verstuurd moet worden naar de socket client.
*/
void SocketServer::sendData(const string& message) {
	if(send(clientSocket, message.c_str(), message.length(), 0) < 0) {
		cout<<"Error sending data"<<endl;
		return;
	}
	else {
		cout<<"Message send: "<<message<<endl;
	}
}

/*!
	@brief Ontvangt data van de socket client die op het moment verbonden is.

	Deze functie ontvangt een string van de socket client die momenteel verbonden is en geeft deze string terug.

	@return De string die ontvangen is van de socket client.
*/
string SocketServer::receiveData() {
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

/*!
	@brief Sluit de connectie met de socket client.

	Deze functie sluit de verbinding met de momenteel verbonden socket client.
*/
void SocketServer::closeClientConnection() {
	close(clientSocket);
	cout<<"Server has disconnected the client"<<endl<<endl;
}
