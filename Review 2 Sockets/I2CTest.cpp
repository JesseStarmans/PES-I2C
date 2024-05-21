#include "SocketServer.h"
#include "SocketClient.h"
#include "I2CConnection.h"

#define tempSlave 0x20
#define OTHER_PI "145.52.127.184"
#define THIS_PI "145.52.127.177"

using namespace std;
map<string, int> command{{"Error", 0x00},{"Disconnected", 0x01},{"CheckTemp", 0x21}, {"Temperatuur:", 0x22}, {"Check Luchtvochtig", 0x23}, {"Wemos Pi sturen", 0x70}}; 

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

int checkReceived(std::string received) {
    size_t pos = received.find(' ');
    std::string cmd = (pos == std::string::npos) ? received : received.substr(0, pos);
    auto it = command.find(cmd);
    if (it != command.end()){
        return it->second;
    } else {
            return 0xFF;
    }
}

int main(void) {
    SocketServer server(7070, THIS_PI);

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
