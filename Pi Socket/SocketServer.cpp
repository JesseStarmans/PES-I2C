#include "SocketServer.h"

#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

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

SocketServer::~SocketServer() {
	close(serverSocket);
}

vector<string> SocketServer::setupWemosIP() {
	listen(serverSocket, 5);
	
	cout<<"Listening for Wemos setup"<<endl;
	
	int wemosAantal = 3;
	
	string wemos1;
	string wemos2;
	string wemos3;
	
	for (int i = 0; i < wemosAantal; i++) {
		clientSocket = accept(serverSocket, nullptr, nullptr);
		if (clientSocket < 0) {
			cout<<"Error accepting Wemos connection"<<endl;
			exit(EXIT_FAILURE);
		}
		
		string nummer = receiveData();
		//usleep(50000);
		//string buffer = receiveData();
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

void SocketServer::listenForClients() {
	listen(serverSocket, 5);
	
	cout<<"Server listening on port "<<port<<endl;
	
	while(1) {
		clientSocket = accept(serverSocket, nullptr, nullptr);
		if (clientSocket < 0) {
			cout<<"Error accepting connection"<<endl;
			continue;
		}
		
		string received = receiveData();
		if (received == "Error" ) {
			cout<<"Error receiving data"<<endl;
		}
		else if (received == "Disconnected") {
			cout<<"Client disconnected"<<endl;
		}
		else {
			if (received == "VoordeurKnop Openen") {
				cout<<"Voordeur is geopend"<<endl;
			}
			else if (received == "VoordeurKnop Sluiten") {
				cout<<"Voordeur is gesloten"<<endl;
			}
			else if (received == "Deur1Knop Openen") {
				cout<<"Deur 1 is geopend"<<endl;
			}
			else if (received == "Deur1Knop Sluiten") {
				cout<<"Deur 1 is gesloten"<<endl;
			}
			else if (received == "Deur2Knop Openen") {
				cout<<"Deur 2 is geopend"<<endl;
			}
			else if (received == "Deur2Knop Sluiten") {
				cout<<"Deur 2 is gesloten"<<endl;
			}
			else {
				cout <<"Succesvol data ontvangen"<<endl;
			}
		}
		
		
		string test = "test";
		sendData(test);
		
		close(clientSocket);
	}
}

void SocketServer::sendData(const string& message) {
	if(send(clientSocket, message.c_str(), message.length(), 0) < 0) {
		cout<<"Error sending data"<<endl;
		return;
	}
	else {
		cout<<"Message send: "<<message<<endl;
	}
}

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
