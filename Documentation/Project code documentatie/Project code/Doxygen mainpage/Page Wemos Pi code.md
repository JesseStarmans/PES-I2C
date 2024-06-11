@page subMainPage2 Wemos/beheerders-interface Raspberry Pi main
@tableofcontents

@section sectionWemos1 Inleiding
Op deze pagina is de volledige implementatie te vinden van de Raspberry Pi die verbonden is aan de 3 Wemos bordjes en de beheerders-interface over socket verbindingen. \n
Voor de verbinding met de andere Raspberry Pi, de Wemos bordjes en de beheerders-interface wordt er gebruik gemaakt van de SocketClient en SocketServer klasses.

@subsection subsectionWemos1Builden Het programma builden
Om de code te kunnen builden en runnen, kan er gebruik gemaakt worden van de volgende commando's:
1. Ga naar de map waar het programma staat:
@code
cd /home/pigroepg/Documents/Socket
@endcode
2. Compileer de main met de afhankelijkheden:
@code
g++ -Wall -o QTTest QTTest.cpp SocketClient.cpp SocketServer.cpp
@endcode
3. Run het programma (in een nieuwe terminal voor de overzichtelijkheid):
@code
lxterminal -e ./QTTest
@endcode

@section sectionWemos2 Klasses

Om de \ref QTTest.cpp "Wemos Raspberry Pi main" code correct te laten werken zijn er drie klasses geschreven:\n
- SocketClient
- SocketServer

@section sectionWemos3 Main code

Om de documentatie van deze main te zien, zie: \ref QTTest.cpp "Wemos Raspberry Pi main".\n

Hieronder is de losse main te zien:
@code
int main(void) {	
	SocketServer server(MY_PORT, MY_IP);
	
	IPsWemos = server.setupWemosIP(); 
    {
        SocketClient client(MY_PORT, IPsWemos[0].c_str());
        
        client.sendData("Test123");
        string returning = client.receiveData();
    }
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
@endcode