#include "SocketClient.h"
#include <iostream>

using namespace std;

int main(void) {
	const char IP[16] = "145.52.127.238";
	SocketClient client(8080, IP);
	client.sendData("Test123");
	string returning = client.receiveData();
	
	if (returning == "Error") {
		cout<<"Error receiving data"<<endl;
	}
	else if (returning == "Disconnected") {
		cout<<"Server disconnected"<<endl;
	}
	else {
		cout<<"In de main: "<<returning<<endl;
	}
	
	
}
