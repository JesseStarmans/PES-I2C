#include <iostream>
#include <arpa/inet.h>

using namespace std;

class SocketClient {
public:
	SocketClient(int, char*);
	~SocketClient();
	void sendData(const string&);
	string receiveData();
private:
	int clientSocket;
	sockaddr_in serverAddress;
	int port;
	char* IP;
};
