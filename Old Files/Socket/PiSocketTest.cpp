#include "SocketServer.h"
#include "SocketClient.h"
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <malloc.h>
#include <string>
#include <unistd.h>
#include <iostream>
#define tempSlave 0x20

int fd = open("/dev/i2c-1", O_RDWR);	
using namespace std;

void delaySec(int seconde) {
	usleep(seconde * 1000000);
}

void writeTo(uint8_t startPos, uint8_t array[], int sizeArray, int fdSlave) {
	uint8_t* toSend = (uint8_t*) malloc((sizeof(uint8_t)*sizeArray)+sizeof(uint8_t));
												//Maakt een array aan (afhankelijk van de grootte van de meegegeven array)
												//"(sizeof(uint8_t)*sizeArray)" om te zorgen dat de hele array er in opgeslagen kan worden
												//"+sizeof(uint8_t)" zodat de startPositie ook nog meegegeven kan worden
												
	toSend[0] = startPos;						//Zet het eerste te sturen getal gelijk aan de startPositie
	for (int i = 1; i < sizeArray; i++) {		//Loop door de volledige meegegeven array
		toSend[i] = array[i-1];					//Vul de te verzenden array met het getal in de meegegeven array - 1
		if (i == sizeArray-1) {					//Check of de teller bij de een naar laatste plek is 
			toSend[i+1] = array[i];				//Vul de laatste te verzenden array met het meegegeven array getal
		}
	}
	write(fdSlave, toSend, sizeArray+sizeof(uint8_t)); 
												//Stuur de toSend array over I2C naar de meegegeven slave
    free(toSend);
}

void read6From(uint8_t startPos, uint8_t* ingelezen, int fdSlave) {
	write(fdSlave, &startPos, 1);				//Stuur naar de slave op welke plek in het register gekeken moet worden
	usleep(1000);								//Wacht 1 miliseconde (1000 microsec = 1 milisec)
	read(fdSlave, ingelezen, 6);				//Lees 6 bytes van de meegegeven slave in
}

/*Deze functie bind de slave adressen aan de slave variabelen*/
void initI2C(){
	ioctl(fd, I2C_SLAVE, tempSlave);				//Initialiseer de eerste slave op het eerste slaveAdres
	//ioctl(fd2, I2C_SLAVE, SlaveAdd2);			//Initialiseer de tweede slave op het tweede slaveAdres
}

int checkReceived(std::string received) {
    if (received == "Error" ) {
		cout<<"Error receiving data"<<endl;
		return 0x00;
	}
	else if (received == "Disconnected") {
		cout<<"Client disconnected"<<endl;
		return 0x01;
	}
    else if (received == "Wemos Pi sturen") {
   		 	const char IP[16] = "145.52.127.184";
			SocketClient client(8080, IP);
			client.sendData("I2C Pi return");
			return 0x70;
    }
    else if (received == "Check Temp"){
        return 0x21;
    }
    else if (received == "Set Temp"){
        return 0x22;
    }
    else {
            return 0xFF;
    }
}

void sendToTemp(uint8_t set){
    initI2C();									//Roep de functie initI2C() aan
	uint8_t sendBuf[1][5] = {{67,104,101,99,107}};                         //Maak een te versturen array aan
    uint8_t recBuf[6] = {0};					//Maak een array aan waarin de ontvangen bytes komen van slave 1

    writeTo(1, sendBuf[0], sizeof(sendBuf[0])/sizeof(sendBuf[0][0]), fd);		//Roep de functie writeTo() aan en schrijf "Aan" naar de eerste slave

    while (recBuf[0] != 1) {
        read6From(0, recBuf, fd);
    }
    //read6From(1, recBuf, fd);											//Roep de functie read6From() aan en lees 6 bytes van slave 1 in
    std::string data = "";
    for (int j = 1; j < 6; j++) {										//Loop door de volledige ontvangen bytes array
        if (recBuf[j] != 0) {											//Check of de ontvangen byte van slave 1 niet 0 is
            printf("%de ingelezen waarde uit het REGISTER van slave 1: %d\n\r", j+1 ,recBuf[j]);
            data += std::to_string(recBuf[j]);
        }
    }
    
    const char IP[16] = "145.52.127.184";
    SocketClient client(8080, IP);
    client.sendData(data);
    
    printf("\n");
		
	delaySec(1);															//Wacht 1 seconde
}


int main(void) {
    const char IP[16] = "145.52.127.177";

    SocketServer server(7070, IP);

    // vector<string> IPsWemos = server.setupWemosIP();

    server.serverListen();

    while (true) {
        server.serverAccept();
        string received = server.receiveData();
        int respons = checkReceived(received);
        if (respons >= 0x70 && respons < 0x80) {
            cout<<"Van Wemos Pi info ontvangen"<<endl;
        }
        switch(respons){
            case 0x21:
                sendToTemp(200);
                break;
            case 0x22:
                break;
        }
        server.closeClientConnection();
    }
}