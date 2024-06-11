#include "SocketServer.h"
#include "SocketClient.h"

#include <string>

#define  PI_B "10.42.0.1"
#define MY_IP "10.42.0.251"
#define QT "10.42.0.190"

// #define PI_B "145.52.127.177"
// #define MY_IP "145.52.127.184"
// #define QT "145.52.127.223"

vector<string> IPsWemos;

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
		if (received == "VoordeurKnop 120" || received == "VoordeurKnop 25") {
			SocketClient client(7070, PI_B);
			client.sendData(received);
			cout<<"Voordeur open/sluit"<<endl;
			return 0x40;
		}
		else if (received == "Deur1Knop 120" || received == "Deur1Knop 25") {
			SocketClient client(7070, PI_B);
			client.sendData(received);
			cout<<"Deur 1 open/sluit"<<endl;
			return 0x40;
		}
		else if (received == "Deur2Knop 120" || received == "Deur2Knop 25") {
			SocketClient client(7070, PI_B);
			client.sendData(received);
			cout<<"Deur 2 open/sluit"<<endl;
			return 0x40;
		}
		else if (received == "140"){
			SocketClient client(7070, PI_B);
			client.sendData(received);
			cout<<"Druk sensor opvragen"<<endl;
			cout << "test1 : " << received << endl;
			return 0x40;
		}
		else if (received == "Rruk: 1" || received == "Rruk: 2"){
			SocketClient client(9090, QT);
			client.sendData(received);
			cout<<"Rruk"<<endl;
			return 141;
		}
		else if (received == "141"){
			SocketClient client(7070, PI_B);
			client.sendData(received);
			cout<<"gordijn/deuren"<<endl;
			return 141;
		}
		else if (received == "203"){
			SocketClient client(7070, PI_B);
			client.sendData(received);
			string returning = client.receiveData();
			cout << returning << endl;
			SocketClient QTClient(9090, QT);
            QTClient.sendData(returning);
			return 0xD0;
		}
		else if (received == "210"){
			SocketClient client(6060, IPsWemos[0].c_str());
			client.sendData(received);
			string returning = client.receiveData();
			cout << returning << endl;
			SocketClient QTClient(9090, QT);
            QTClient.sendData(returning);
			return 0xD2;
		}
		else if (received == "VoordeurKnop Sluiten") {
			SocketClient client(7070, PI_B);
			client.sendData(received);
			cout<<"Voordeur is gesloten"<<endl;
			return 0x41;
		}
		else if (received == "Deur1Knop Openen") {
			SocketClient client(7070, PI_B);
			client.sendData(received);
			cout<<"Deur 1 is geopend"<<endl;
			return 0x42;
		}
		else if (received == "Deur1Knop Sluiten") {
			SocketClient client(7070, PI_B);
			client.sendData(received);
			cout<<"Deur 1 is gesloten"<<endl;
			return 0x43;
		}
		else if (received == "Deur2Knop Openen") {
			SocketClient client(7070, PI_B);
			client.sendData(received);
			cout<<"Deur 2 is geopend"<<endl;
			return 0x44;
		}
		else if (received == "Deur2Knop Sluiten") {
			SocketClient client(7070, PI_B);
			client.sendData(received);
			cout<<"Deur 2 is gesloten"<<endl;
			return 0x45;
		}
        else if (received == "Voordeur open") {
   		 	SocketClient QTClient(9090, QT);
			QTClient.sendData("Voordeur open");
            return 0x50;
        }
        else if (received == "Voordeur dicht") {
   		 	SocketClient QTClient(9090, QT);
			QTClient.sendData("Voordeur dicht");
            return 0x51;
        }
        else if (received == "Deur 1 open") {
   		 	SocketClient QTClient(9090, QT);
			QTClient.sendData("Deur1 open");
            return 0x52;
        }
		else if (received == "Deur 1 dicht") {
   		 	SocketClient QTClient(9090, QT);
			QTClient.sendData("Deur1 dicht");
			return 0x53;
		}
		else if (received == "912071d") {
			{
			SocketClient client(7070, PI_B);
			client.sendData(received);
			}
   		 	SocketClient QTClient(9090, QT);
			QTClient.sendData("Deur1 open");
			return 0x57;
		}
        else if (received == "Deur 2 open") {
   		 	SocketClient QTClient(9090, QT);
			QTClient.sendData("Deur2 open");
            return 0x54;
        }
		else if (received == "Deur 2 dicht") {
   		 	SocketClient QTClient(9090, QT);
			QTClient.sendData("Deur2 dicht");
			return 0x55;
		}
		else if (received.find("lichtkrant") != std::string::npos) {
			SocketClient wemosClient(6060, IPsWemos[0].c_str());
			wemosClient.sendData(received);
			string check = wemosClient.receiveData();
			cout<<check<<endl;
			return 0x80;
		}
		else if (received == "b8c72833") {
			{
			SocketClient client(7070, PI_B);
			client.sendData(received);
			}
   		 	SocketClient QTClient(9090, QT);
			QTClient.sendData("Deur2 open");
			return 0x56;
		}
		else if (received == "I2C Pi sturen") {
			SocketClient client(7070, PI_B);
			client.sendData("Wemos Pi sturen");
			return 0x70;
		} // Temperatuur logica
		else if (received == "200") {
			SocketClient client(7070, PI_B);
			client.sendData("CheckTemp");
			return 0x21;
		} 
		else if (received.find("201") != std::string::npos) {
			SocketClient client(7070, PI_B);
			//cout<<received<<endl;
			client.sendData(received);
			return 0xE1;
		}
		else if (received == "Check CO2") {
			SocketClient client(7070, PI_B);
			cout<<received<<endl;
			client.sendData("210");
			return 0x21;
		} 
		else if (received.find("Temperatuur") != std::string::npos) {
			SocketClient client(7070, PI_B);
			client.sendData(received);
			return 0x22;
		}
		else if (received == "I2C Pi return") {
			return 0x71;
		}
		else if (received.find("RTemp:") != std::string::npos) {
			SocketClient QTClient(9090, QT);
			QTClient.sendData(received);
			return 0xF0;
		}
		else if (received.find("Plant") != std::string::npos) {
			SocketClient wemosClient(6060, IPsWemos[2].c_str());
			wemosClient.sendData(received);
			//string returning = wemosClient.receiveData();
			return 0xD2;
		}
		else if (received.find("Rpan:") != std::string::npos){
			SocketClient QTClient(9090, QT);
			QTClient.sendData(received);
			return 0xD2;
		}
		else if (received.find("RPlnt:") != std::string::npos) {
			SocketClient QTClient(9090, QT);
			QTClient.sendData(received);
			return 0xD3;
		}
		else if (received.find("RCO2:") != std::string::npos) {
			SocketClient QTClient(9090, QT);
			QTClient.sendData(received);
			return 0xF0;
		}
		else if (received.find("204") != std::string::npos) {
			SocketClient client(7070, PI_B);
			//cout<<received<<endl;
			client.sendData(received);
			return 0xE2;
		}
		else {
			cout <<"Succesvol data ontvangen "<<received<<endl;
			return 0xFF;
		}
}
}

int main(void) {
	try {	
	SocketServer server(8080, MY_IP);
	
	// IPsWemos = server.setupWemosIP();
	

	// for (auto it : IPsWemos) {
	// 	cout<<it<<endl;
	// }

    server.serverListen();

	/*Hieronder de constant runnende server*/
	while (true) {
		server.serverAccept();
	
		string received = server.receiveData();
		
		int respons = checkReceived(received);
		//switch(respons){
		//	case 0x40:
		if (respons >= 0x40 && respons < 0x50) {
			cout<<"UI event ontvangen"<<endl;
		}
		else if (respons >= 0x50 && respons < 0x60) {
			cout<<"UI event verstuurd"<<endl;
		}
		else if (respons == 0x71) {
			cout<<"Return bericht van I2C Pi ontvangen"<<endl;
		}
		else if (respons == 0x80) {
			cout<<"Lichtkrant data verstuurd"<<endl;
		}
		else if (respons == 0x21) {
			cout << "Vraag temperatuur op" << endl;
		}
		else if (respons == 0x22) {
			cout << "Set Temperatuur" << endl;
		}
		else if (respons == 0xD2){
			cout << "Vraag plantjes" << endl;
		}
		else {
			cout<<"Gestuurd"<<endl;
		}
		
		server.closeClientConnection();
	}
	}catch(...){
		cout << "Foutje oops" << endl;
	}
}
