#include "SocketServer.h"
#include "SocketClient.h"

#include <string>

//#define  PI_B "10.42.0.1"
//#define MY_IP "10.42.0.251"
//#define QT "10.42.0.190"

#define PI_B "145.52.127.177"
#define MY_IP "145.52.127.184"
#define QT "145.52.127.223"

vector<string> IPsWemos;

int checkReceived(std::string received) {
	if (received == "Error" ) {
		cout<<"Error receiving data"<<endl;
		return 0x01;
	}
	else if (received == "Disconnected") {
		cout<<"Client disconnected"<<endl;
		return 0x02;
	}
	else {
		if (received == "VoordeurKnop 120" || received == "VoordeurKnop 25") {			//Voordeur openen/dichtmaken (naar STM)
			SocketClient client(7070, PI_B);
			client.sendData(received);
			//cout<<"Voordeur open/sluit"<<endl;
		}
		else if (received == "Deur1Knop 120" || received == "Deur1Knop 25") {			//D1 openen/dichtmaken (naar STM)
			SocketClient client(7070, PI_B);
			client.sendData(received);
			//cout<<"Deur 1 open/sluit"<<endl;
		}
		else if (received == "Deur2Knop 120" || received == "Deur2Knop 25") {			//D2 openen/dichtmaken (naar STM)
			SocketClient client(7070, PI_B);
			client.sendData(received);
			//cout<<"Deur 2 open/sluit"<<endl;
		}
		else if (received == "140"){													//Druksensor opvragen (naar STM)
			SocketClient client(7070, PI_B);
			client.sendData(received);
			//cout<<"Druk sensor opvragen"<<endl;
			//cout << "test1 : " << received << endl;
		}
		else if (received.find("RDruk:") != std::string::npos){							//Return drukwaarde (naar QT)
			SocketClient client(9090, QT);
			client.sendData(received);
		}
		else if (received == "141"){													//Toggle gordijn/deur modus (naar STM)
			SocketClient client(7070, PI_B);
			client.sendData(received);
		}
		// else if (received == "203"){													//Oude Check CO2
		// 	SocketClient client(7070, PI_B);
		// 	client.sendData(received);
		// 	string returning = client.receiveData();
		// 	cout << returning << endl;
		// 	SocketClient QTClient(9090, QT);
        //     QTClient.sendData(returning);
		// }
		// else if (received == "210"){													//Oude vraag plant op
		// 	SocketClient client(6060, IPsWemos[0].c_str());
		// 	client.sendData(received);
		// 	string returning = client.receiveData();
		// 	cout << returning << endl;
		// 	SocketClient QTClient(9090, QT);
        //     QTClient.sendData(returning);
		// }
		// else if (received == "VoordeurKnop Sluiten") {								//Oude voordeur sluiten vanaf QT (naar STM)
		// 	SocketClient client(7070, PI_B);
		// 	client.sendData(received);
		// 	cout<<"Voordeur is gesloten"<<endl;
		// }
		// else if (received == "Deur1Knop Openen") {									//Oude D1 openen vanaf QT (naar STM)
		// 	SocketClient client(7070, PI_B);
		// 	client.sendData(received);
		// 	cout<<"Deur 1 is geopend"<<endl;
		// }
		// else if (received == "Deur1Knop Sluiten") {									//Oude D1 sluiten vanaf QT (naar STM)
		// 	SocketClient client(7070, PI_B);
		// 	client.sendData(received);
		// 	cout<<"Deur 1 is gesloten"<<endl;
		// }
		// else if (received == "Deur2Knop Openen") {									//Oude D2 openen vanaf QT (naar STM)
		// 	SocketClient client(7070, PI_B);
		// 	client.sendData(received);
		// 	cout<<"Deur 2 is geopend"<<endl;
		// }
		// else if (received == "Deur2Knop Sluiten") {									//Oude D2 sluiten vanaf QT (naar STM)
		// 	SocketClient client(7070, PI_B);
		// 	client.sendData(received);
		// 	cout<<"Deur 2 is gesloten"<<endl;
		// }
        else if (received == "Voordeur: open") {										//Voordeur in QT openen
   		 	SocketClient QTClient(9090, QT);
			QTClient.sendData("Voordeur: open");
        }
        // else if (received == "Voordeur: dicht") {
   		//  	SocketClient QTClient(9090, QT);
		// 	QTClient.sendData("Voordeur: dicht");
        // }
        else if (received == "Deur 1: open") {											//D1 in QT openen
   		 	SocketClient QTClient(9090, QT);
			QTClient.sendData("Deur1: open");
        }
		// else if (received == "Deur 1: dicht") {
   		//  	SocketClient QTClient(9090, QT);
		// 	QTClient.sendData("Deur1: dicht");
		// }
		else if (received.find("912071d") != std::string::npos) {						//RFID data voor deuren openen in STM en QT
			{
			SocketClient client(7070, PI_B);
			client.sendData(received);
			}
			SocketClient QTClient(9090, QT);
			if (received.find("Voordeur: ") != std::string::npos) {						//Check of het voor de voordeur bedoeld is of voor de binnendeur
				QTClient.sendData("Voordeur: open");
			}
			else {
				QTClient.sendData("Deur1: open");
			}
		}
        else if (received == "Deur 2: open") {											//D2 in QT openen
   		 	SocketClient QTClient(9090, QT);
			QTClient.sendData("Deur2: open");
        }
		// else if (received == "Deur 2: dicht") {
   		//  	SocketClient QTClient(9090, QT);
		// 	QTClient.sendData("Deur2: dicht");
		// }
		else if (received.find("lichtkrant") != std::string::npos) {					//Stuur lichtkrant tekst naar lichtkrant Wemos
			SocketClient wemosClient(6060, IPsWemos[0].c_str());
			wemosClient.sendData(received);
			//string check = wemosClient.receiveData();
			//cout<<check<<endl;
		}
		else if (received.find("b8c72833") != std::string::npos) {						//RFID data voor deuren openen in STM en QT
			{
			SocketClient client(7070, PI_B);
			client.sendData(received);
			}
			SocketClient QTClient(9090, QT);
			if (received.find("Voordeur: ") != std::string::npos) {						//Check of het voor de voordeur bedoeld is of voor de binnendeur
				QTClient.sendData("Voordeur: open");
			}
			else {
				QTClient.sendData("Deur2: open");
			}
		}
		// else if (received == "I2C Pi sturen") {
		// 	SocketClient client(7070, PI_B);
		// 	client.sendData("Wemos Pi sturen");
		// } 
		else if (received == "200") {													//Temperatuur opvragen (naar STM)
			SocketClient client(7070, PI_B);
			client.sendData("CheckTemp");
		} 
		else if (received.find("201") != std::string::npos) {							//Set gewenste Temperatuur (naar STM)
			SocketClient client(7070, PI_B);
			client.sendData(received);
		}
		else if (received == "Check CO2") {												//CO2 opvragen (naar STM)
			SocketClient client(7070, PI_B);
			//cout<<received<<endl;
			client.sendData("210");
		} 
		// else if (received.find("Temperatuur") != std::string::npos) {				//Oude Get Temp functie
		// 	SocketClient client(7070, PI_B);
		// 	client.sendData(received);
		// }
		else if (received.find("RTemp:") != std::string::npos) {						//Return Temperatuurwaarde naar QT
			SocketClient QTClient(9090, QT);
			QTClient.sendData(received);
		}
		else if (received.find("RPl:") != std::string::npos) {							//Return Plant waarde naar QT
			SocketClient client(9090, QT);
			client.sendData(received);
		}
		else if (received.find("Plant") != std::string::npos) {							//Plant waarde opvragen (naar Wemos)
			SocketClient wemosClient(6060, IPsWemos[1].c_str());
			wemosClient.sendData(received);
		}
		// else if (received.find("RPlant:") != std::string::npos){						//Oude versie, werkte niet
		// 	SocketClient QTClient(7070, PI_B);
		// 	QTClient.sendData(received);
		// 	return 0xD2;
		// }
		// else if (received.find("RPlnt:") != std::string::npos) {						//Oude versie, werkte ook niet
		// 	SocketClient QTClient(9090, QT);
		// 	QTClient.sendData(received);
		// 	return 0xD3;
		// }
		else if (received.find("RCO2:") != std::string::npos) {							//Returnt CO2 waarde naar QT
			SocketClient QTClient(9090, QT);
			QTClient.sendData(received);
		}
		else if (received.find("204") != std::string::npos) {							//Set grenswaarde CO2 (naar STM)
			SocketClient client(7070, PI_B);
			client.sendData(received);
		}
		else if (received.find("LEDstrip: ") != std::string::npos) {					//Verander de kleur van de LED strip (naar Wemos)
			SocketClient wemosClient(6060, IPsWemos[0].c_str());
			wemosClient.sendData(received);
		}
		else if (received.find("207") != std::string::npos) {							//Reset alarm (naar STM)
			SocketClient client(7070, PI_B);
			client.sendData(received);
		}
		else {
			cout <<"Onbekende data ontvangen: "<<received<<endl;
		}
		return 0x00;
	}
}

int main(void) {
	SocketServer server(8080, MY_IP);
	
	IPsWemos = server.setupWemosIP();

	for (auto it : IPsWemos) {
		cout<<it<<endl;
	}

	server.serverListen();

	/*Hieronder de constant runnende server*/
	while (true) {
		server.serverAccept();
	
		string received = server.receiveData();
		
		int respons = checkReceived(received);
		if (respons) {
			cout<<"Errortje"<<endl;
		}
		
		server.closeClientConnection();
	}
}
