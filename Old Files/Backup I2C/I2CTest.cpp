
#include "SocketServer.h"
#include "SocketClient.h"
#include "I2CConnection.h"
#include <bitset>

#define tempSlave 0x20
#define OTHER_PI "145.52.127.184"
#define THIS_PI "145.52.127.177"

using namespace std;
map<string, int> command{{"Error", 0x00},{"Disconnected", 0x01},{"CheckTemp", 200}, {"Temperatuur:", 0x22}, {"Check Luchtvochtig", 0x23}, {"Wemos Pi sturen", 0x70}, {"210", 203}}; 

void sendToCO2(uint8_t set) {
	I2CConnection slave(tempSlave);
    vector<uint8_t> sendNum = {set};

    slave.sendI2CTo(1,sendNum,2);

    cout<<static_cast<int>(sendNum.at(0))<<endl;

    cout<<"Test2"<<endl;
    vector<uint8_t> received = {0};
    //vector<uint8_t> received2 = {0};

    while (received.at(0) != 1) {
        received = slave.receive6OverI2C(0);
    }
    
    for (int i = 0; i < static_cast<int>(received.size()); i++) {
        cout<<received.at(i)<<endl;
    }
    
    // cout<<"Test3"<<endl;
    // while (received2.at(0) != 1) {
    //     received2 = slave.receive6OverI2C(0);
    // }
    
    uint16_t CO2;
    cout<<std::bitset<8>(received.at(1)) << " " << std::bitset<8>(received.at(2))<<endl;
    CO2 = (received.at(1) << 8 | received.at(2));

    cout<<std::bitset<16>(CO2)<<endl;

    cout<<"Test4"<<endl;
    std::string data = "RCO2: ";
    
    std::vector<uint8_t>::iterator it = received.begin() + 3;
    data += to_string(static_cast<uint16_t>(CO2)) + " ";
    for (; it != received.end(); it++) {
        if (*it != 0) {
            data += to_string(static_cast<uint8_t>(*it)) + " ";
        }
    }

    SocketClient client(8080, OTHER_PI);
    client.sendData(data);

    // for (int i = 0; i < static_cast<int>(received2.size()); i++) {
    //     std::cout<<received2.at(i)<<std::endl;
    // }
    
    cout<<endl;
}

void sendToTemp(uint8_t set) {
	I2CConnection slave(tempSlave);

    vector<uint8_t> sendNum = {set};

    slave.sendI2CTo(1, sendNum, 2);

    cout<<static_cast<int>(sendNum.at(0))<<endl;

    cout<<"Test2"<<endl;
    vector<uint8_t> received = {0};
    //vector<uint8_t> received2 = {0};

    while (received.at(0) != 1) {
        received = slave.receive6OverI2C(0);
    }
    
    for (int i = 0; i < static_cast<int>(received.size()); i++) {
        cout<<received.at(i)<<endl;
    }
    
    // cout<<"Test3"<<endl;
    // while (received2.at(0) != 1) {
    //     received2 = slave.receive6OverI2C(0);
    // }

    cout<<"Test4"<<endl;
    std::string data = "RTemp:";
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

    // for (int i = 0; i < static_cast<int>(received2.size()); i++) {
    //     std::cout<<received2.at(i)<<std::endl;
    // }
    
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
            case 203:
                sendToCO2(203);
                break;
            case 200:
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
