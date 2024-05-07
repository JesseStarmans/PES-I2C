#include "SocketClient.h"
#include <iostream>

using namespace std;

int main(void) {
	char IP[16] = "145.52.127.169";
	SocketClient client(8080, IP);
	client.sendData("Test123");
	string returning = client.receiveData();
	cout<<"In de main: "<<returning<<endl;
}
