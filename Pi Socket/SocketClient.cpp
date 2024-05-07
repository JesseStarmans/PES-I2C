#include "SocketClient.h"

#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

SocketClient::SocketClient(int poort, char* serverIPAddress) : port(poort), IP(serverIPAddress) {
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == -1) {
		cout<<"Error maken clientSocket"<<endl;
		exit(EXIT_FAILURE);
	}
	
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
	serverAddress.sin_addr.s_addr = inet_addr(IP);
	
	if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
		cout<<"Error connecting to server."<<endl;
		exit(EXIT_FAILURE);
	}
}

SocketClient::~SocketClient() {
	close(clientSocket);
}

void SocketClient::sendData(const string& message) {
	if (send(clientSocket, message.c_str(), message.length(), 0) < 0) {
		cout<<"Error sending data"<<endl;
		return;
	}
	else {
		cout<<"Message send: "<<message<<endl;
	}
}

string SocketClient::receiveData() {
	string str = "";
	char buffer[1024] = "";
	ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
	if (bytesReceived < 0) {
		cout<<"Error receiving data"<<endl;
	}
	else if (bytesReceived == 0) {
		cout<<"Server disconnected"<<endl;
	}
	else {
		str.assign(buffer);
		cout<<"Received data from server: "<<str<<endl;
	}
	return str;
}

