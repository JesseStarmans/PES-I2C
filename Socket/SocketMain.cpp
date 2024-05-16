

#include "SocketServer.h"
#include "SocketClient.h"

#include <string>

int checkReceived(std::string received) {
	if (received == "Error" ) {
		cout<<"Error receiving data"<<endl;
		return 0x00;
	}
	else if (received == "Disconnected") {
		cout<<"Client disconnected"<<endl;
		return 0x01;
	}
	else {
		if (received == "VoordeurKnop Openen") {
			cout<<"Voordeur is geopend"<<endl;
			return 0x40;
		}
		else if (received == "VoordeurKnop Sluiten") {
			cout<<"Voordeur is gesloten"<<endl;
			return 0x41;
		}
		else if (received == "Deur1Knop Openen") {
			cout<<"Deur 1 is geopend"<<endl;
			return 0x42;
		}
		else if (received == "Deur1Knop Sluiten") {
			cout<<"Deur 1 is gesloten"<<endl;
			return 0x43;
		}
		else if (received == "Deur2Knop Openen") {
			cout<<"Deur 2 is geopend"<<endl;
			return 0x44;
		}
		else if (received == "Deur2Knop Sluiten") {
			cout<<"Deur 2 is gesloten"<<endl;
			return 0x45;
		}
		else if (received == "Deur 1 dicht") {
			return 0x53;
		}
		else if (received == "b8c72833"){
			return 0x54;
		}
		else {
			cout <<"Succesvol data ontvangen"<<endl;
			return 0xFF;
		}
	}
}

int main(void) {
	const char IP[16] = "145.52.127.177";
	
	bool VD = false;
	bool D1 = false;
	bool D2 = false;
	
	SocketServer server(8080, IP);
	
	vector<string> IPsWemos = server.setupWemosIP();
	
	//SocketClient client(8080, IPsWemos[0].c_str());
	
	//client.sendData("Test123");
	//string returning = client.receiveData();
	server.serverListen();
	
	/*Hieronder de constant runnende server*/
	while (true) {
		server.serverAccept();
		string received = server.receiveData();
		int respons = checkReceived(received);
		if (respons >= 0x40 && respons < 0x50) {
			cout<<"UI event ontvangen"<<endl;
			if (VD) {
				server.sendData("Voordeur dicht");
			}
			else if (D1) {
				server.sendData("Deur 1 dicht");
			}
			else if (D2) {
				server.sendData("Deur 2 dicht");
			}
			else {
				server.sendData("Data succesvol ontvangen");
			}
		}
		else if (respons == 0x54) {
			D1 = true;
			cout<<"kaartje tegen RFID"<<endl;
		}
		server.closeClientConnection();
	}
	
	server.listenForClients();
}