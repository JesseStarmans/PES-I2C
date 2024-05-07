#include "SocketServer.h"
#include "SocketClient.h"

#include <string>

int main(void) {
	char IP[16] = "145.52.127.169";
	
	SocketServer server(8080, IP);
	
	vector<string> IPsWemos = server.setupWemosIP();
	SocketClient client(8080, IPsWemos[0].c_str());
	
	client.sendData("Test123");
	string returning = client.receiveData();
	
	cout<<"In de main: "<<returning<<endl;
	
	server.listenForClients();
}
