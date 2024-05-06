#include "SocketServer.h"

#include <iostream>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

SocketServer::SocketServer(int poortNr) : port(poortNr) {
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1) {
		cout<<"Error creating socket"<<endl;
		exit(EXIT_FAILURE);
	}
	
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = inet_addr("145.52.127.169");
	serverAddress.sin_port = htons(port);
	
	if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
		cout<<"Error binding socket: "<<strerror(errno)<<endl;
		exit(EXIT_FAILURE);
	}
}

SocketServer::~SocketServer() {
	close(serverSocket);
}

void SocketServer::listenForClients() {
	listen(serverSocket, 5);
	
	cout<<"Server listening on port "<<port<<endl;
	
	while(1) {
		int clientSocket = accept(serverSocket, nullptr, nullptr);
		if (clientSocket < 0) {
			cout<<"Error accepting connection"<<endl;
			continue;
		}
		
		
		char recData[1024] = "";
		for (int i = 0; recData[i] != '\0'; i++) {
			cout<<recData[i];
		}
		cout<<endl;
		ssize_t bytesReceived = recv(clientSocket, recData, sizeof(recData), 0);
		string stringRecData = string(recData);
		
		if (bytesReceived < 0) {
			cout<<"Error receiving data"<<endl;
		}
		else if (bytesReceived == 0) {
			cout<<"Client disconnected"<<endl;
		}
		else {
			if (stringRecData == "VoordeurKnop Openen") {
				cout<<"Voordeur is geopend"<<endl;
			}
			else if (stringRecData == "VoordeurKnop Sluiten") {
				cout<<"Voordeur is gesloten"<<endl;
			}
			else if (stringRecData == "Deur1Knop Openen") {
				cout<<"Deur 1 is geopend"<<endl;
			}
			else if (stringRecData == "Deur1Knop Sluiten") {
				cout<<"Deur 1 is gesloten"<<endl;
			}
			else if (stringRecData == "Deur2Knop Openen") {
				cout<<"Deur 2 is geopend"<<endl;
			}
			else if (stringRecData == "Deur2Knop Sluiten") {
				cout<<"Deur 2 is gesloten"<<endl;
			}
		}
		
		
		//string test = "test";
		//sendData(test);
		close(clientSocket);
	}
}

void SocketServer::sendData(const string& message, int clientSocket) {
	if(send(clientSocket, message.c_str(), message.length(), 0) < 0) {
		cout<<"Error sending data"<<endl;
		return;
	}
	else {
		cout<<"Message send: "<<message<<endl;
	}
}
