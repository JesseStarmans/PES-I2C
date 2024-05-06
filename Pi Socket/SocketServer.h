#include <iostream>
#include <arpa/inet.h>

using namespace std;

class SocketServer {
public: 
	SocketServer(int);
	~SocketServer();
	void listenForClients();
	void sendData(const string&, int);
private:
	int serverSocket;
	sockaddr_in serverAddress;
	int port;
};
