/*!
    @file I2CTest.cpp
    @date 24 May 2024
    @author Glenn de Jonge & Jesse Starmans
    @brief Main CPP file voor de I2C Raspberry Pi.

    Dit is de main voor de Raspberry Pi die verbonden is over I2C.
    Deze main maakt gebruik van de SocketServer en SocketClient klasses voor het verbinden met de andere Raspberry Pi. \n
    Deze main maakt gebruik van de I2CConnection klasse voor het verbinden met de NUCLEO-L432KC bordjes die gebruikt worden als I2C slaves.
*/
#include "SocketServer.h"
#include "SocketClient.h"
#include "I2CConnection.h"

/*!
    @def tempSlave
    @brief Definieert het adres van de temperatuur slave.

    Deze macro definieert het adres van de slave waar de temperatuur & luchtvochtigheids sensor aan verbonden zit.
*/
#define tempSlave 0x20

/*!
    @def OTHER_PI
    @brief Definieert het IP adres van de andere Raspberry Pi.

    Deze macro definieert het IP adres van de andere Raspberry Pi en dus de socket waar data van en naar gestuurd kan worden.
*/
#define OTHER_PI "145.52.127.184"

/*!
    @def THIS_PI
    @brief Definieert het IP adres van deze Raspberry Pi.

    Deze macro definieert het IP adres van de huidige Raspberry Pi.
*/
#define THIS_PI "145.52.127.177"

using namespace std;

/*!
    @brief Een map met string en de bijbehorende hexadecimale codes.

    Een map waarin alle strings gekoppeld kunnen worden aan hexadecimale getallen, die als codes gebruikt kunnen worden.
*/
map<string, int> command{{"Error", 0x00},{"Disconnected", 0x01},{"CheckTemp", 0x21}, {"Temperatuur:", 0x22}, {"Check Luchtvochtig", 0x23}, {"Wemos Pi sturen", 0x70}}; 

/*!
    @brief Stuurt de meegegeven waarde naar de slave met de temperatuur & luchtvochtigheids sensor.

    Deze functie stuurt de meegegeven waarde naar de slave met de temperatuur & luchtvochtigheids sensor met een "set" command. Als er 200 meegegeven wordt, wordt er "check" naar de slave gestuurd om de huidige waardes op te vragen.

    @param set De waarde die naar de slave gestuurd moet worden.
*/
void sendToTemp(uint8_t set) {
	I2CConnection slave(tempSlave);

    vector<uint8_t> sendBufCheck = {67,104,101,99,107};             //Spelt "Check"

    vector<uint8_t> sendBufSet = {115, 101, 116, set, 0};           //Spelt "Set"

    if (set == 200){
        slave.sendI2CTo(1, sendBufCheck, 6);
    }
    else {
        slave.sendI2CTo(1, sendBufSet, 6);
    }

    vector<uint8_t> received = {0};

    while (received.at(0) != 1) {
        received = slave.receive6OverI2C(0);
    }
    std::string data = "";
    std::vector<uint8_t>::iterator it = received.begin() + 1;
    for (; it != received.end(); it++) {
        if (*it != 0) {
            data += to_string(static_cast<uint8_t>(*it)) + " ";
        }
    }
    
    size_t p = data.find(' ');
    if (p != std::string::npos) {
        data.at(p) = '.';
    }

    SocketClient client(8080, OTHER_PI);
    client.sendData(data);
    
    cout<<endl;
}

/*!
    @brief Checkt of de meegegeven string in de map staat.

    Deze functie splitst de ontvangen data op in zijn key woord en de rest en checkt of deze key voorkomt in de map. Als dit zo is, wordt de bijbehorende hexadecimale waarde teruggegeven, anders wordt 0xFF teruggegeven.

    @param received De ontvangen data van de andere Raspberry Pi.
    @return De hexadecimale waarde die bij de meegegeven (opgesplitste) string hoort.
*/
int checkReceived(std::string received) {
    size_t pos = received.find(' ');
    std::string cmd = (pos == std::string::npos) ? received : received.substr(0, pos);
    auto it = command.find(cmd);
    if (it != command.end()){
        return it->second;
    } 
    else {
        return 0xFF;
    }
}

/*!
    @brief De main voor het programma runnen.

    Dit is de main die het volledige programma laat lopen.
*/
int main(void) {
    SocketServer server(7070, THIS_PI);

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
                size_t posi = received.find_first_of("0123456789");
                if (posi != std::string::npos) {
                    // Extract the substring starting from the first digit
                    std::string numStr = received.substr(posi);
                    // Convert to integer
                    int num = std::stoi(numStr);
                    std::cout << "The integer is: " << num << std::endl;
                    sendToTemp(num);
                } else {
                    std::cout << "No digits found in the string." << std::endl;
                }
                break;
        }
        server.closeClientConnection();
    }
}
