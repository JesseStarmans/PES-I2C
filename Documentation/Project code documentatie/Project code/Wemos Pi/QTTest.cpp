/*!
    @file QTTest.cpp
    @date 24 May 2024
    @author Jesse Starmans
    @brief Main CPP file voor de Wemos Raspberry Pi.

    Dit is de main voor de Raspberry Pi die verbonden is over sockets aan de Wemos bordjes en de QT beheerders-interface. \n
    Deze main maakt gebruik van de SocketServer en SocketClient klasses voor het verbinden met de andere Raspberry Pi, de Wemos bordjes en de QT beheerdersinterface.
*/
#include "SocketServer.h"
#include "SocketClient.h"

#include <string>

/*!
	@def PI_B
	@brief Definieert het IP adres van de andere Raspberry Pi.

	Deze macro definieert het IP adres van de I2C Raspberry Pi.
*/
#define  PI_B "145.52.127.177"

/*!
	@def MY_IP
	@brief Definieert het IP adres van deze Raspberry Pi.

	Deze macro definieert het IP adres van deze Raspberry Pi die verbonden is aan de Wemos bordjes en de QT beheerdersinterface.
*/
#define MY_IP "145.52.127.184"

/*!
	@def MY_PORT
	@brief Definieert de poort van de socket server op deze Raspberry Pi.

	Deze macro definieert de poort waar de socket server op deze Raspberry Pi naar moet gaan luisteren. Deze poort moet gebruikt worden door de socket clients die een verbinding willen opzetten met deze socket server. 
*/
#define MY_PORT 8080

vector<string> IPsWemos;		/*!< De vector met string waar de IP addressen van de Wemos borjes in opgeslagen worden.*/

/*!
	@brief Check of de meegegeven string een bekende commando is.

	Deze functie checkt of de meegegeven string een bekend commando is. Als dit zo is, wordt de juiste afhandeling van deze commando aangeroepen.
*/
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
        else if (received == "Voordeur open") {
   		 	const char IP[16] = "145.52.127.223";
            SocketClient QTClient(9090, IP);
            QTClient.sendData("Voordeur open");
            return 0x50;
        }
        else if (received == "Voordeur dicht") {
   		 	const char IP[16] = "145.52.127.223";
            SocketClient QTClient(9090, IP);
            QTClient.sendData("Voordeur dicht");
            return 0x51;
        }
        else if (received == "Deur 1 open") {
   		 	const char IP[16] = "145.52.127.223";
            SocketClient QTClient(9090, IP);
            QTClient.sendData("Deur 1 open");
            return 0x52;
        }
		else if (received == "Deur 1 dicht") {
   		 	const char IP[16] = "145.52.127.223";
            SocketClient QTClient(9090, IP);
            QTClient.sendData("Deur 1 dicht");
			return 0x53;
		}
		else if (received == "912071d") {
   		 	const char IP[16] = "145.52.127.223";
			SocketClient QTClient(9090, IP);
			QTClient.sendData("Deur 1 open");
			return 0x57;
		}
        else if (received == "Deur 2 open") {
   		 	const char IP[16] = "145.52.127.223";
            SocketClient QTClient(9090, IP);
            QTClient.sendData("Deur 2 open");
            return 0x54;
        }
		else if (received == "Deur 2 dicht") {
   		 	const char IP[16] = "145.52.127.223";
            SocketClient QTClient(9090, IP);
            QTClient.sendData("Deur 2 dicht");
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
   		 	const char IP[16] = "145.52.127.223";
			SocketClient QTClient(9090, IP);
			QTClient.sendData("Deur 2 open");
			return 0x56;
		}
		else if (received == "I2C Pi sturen") {
			SocketClient client(7070, PI_B);
			client.sendData("Wemos Pi sturen");
			return 0x70;
		} // Temperatuur logica
		else if (received == "Check Temp") {
			SocketClient client(7070, PI_B);
			client.sendData("Check Temp");
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
		else {
			cout <<"Succesvol data ontvangen "<<received<<endl;
			return 0xFF;
		}
	}
}

/*!
	@brief De main voor het programma runnen.

	Dit is de main die het volledige programma laat lopen.
*/
int main(void) {	
	SocketServer server(MY_PORT, MY_IP);
	
	//IPsWemos = server.setupWemosIP();
	// {
    //     SocketClient client(MY_PORT, IPsWemos[0].c_str());
        
    //     client.sendData("Test123");
    //     string returning = client.receiveData();
    // }
    server.serverListen();

	/*Hieronder de constant runnende server*/
	while (true) {
		server.serverAccept();
	
		string received = server.receiveData();
		
		int respons = checkReceived(received);
		
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
		else {
			cout<<"Gestuurd"<<endl;
		}
		
		server.closeClientConnection();
	}
}
