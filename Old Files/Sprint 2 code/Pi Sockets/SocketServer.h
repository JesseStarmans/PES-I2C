#include <iostream>
#include <arpa/inet.h>
#include <vector>

using namespace std;

class SocketServer {
public: 
	SocketServer(int, const char*); 
	~SocketServer();
	vector<string> setupWemosIP();
	void listenForClients();
	void serverListen();
	void serverAccept();
	void sendData(const string&);
	string receiveData();
	void closeClientConnection();
private:
	int serverSocket;
	sockaddr_in serverAddress;
	int clientSocket;
	int port;
	const char* IP;
};
