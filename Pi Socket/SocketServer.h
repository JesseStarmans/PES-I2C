#include <iostream>
#include <arpa/inet.h>

using namespace std;

class SocketServer {
public: 
	SocketServer(int, char*);
	~SocketServer();
	void listenForClients();
	void sendData(const string&);
	string receiveData();
private:
	int serverSocket;
	sockaddr_in serverAddress;
	int clientSocket;
	int port;
	char* IP;
};
