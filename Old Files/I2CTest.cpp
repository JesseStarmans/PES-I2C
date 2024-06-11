#include "SocketServer.h"
#include "SocketClient.h"
#include "I2CConnection.h"
#include <bitset>
#include <map>

#define tempSlave 0x20
#define buzzerSlave 0x10
#define servoSlave 0x30
//#define OTHER_PI "10.42.0.251"
//#define THIS_PI "10.42.0.1"
#define OTHER_PI "145.52.127.184"
#define THIS_PI "145.52.127.177"

using namespace std;
map<string, int> command{{"Error", 0x00}, {"Disconnected", 0x01}, {"CheckTemp", 200}, {"Temperatuur:", 0x22}, {"Check Luchtvochtig", 0x23}, 
{"Wemos Pi sturen", 0x70}, {"140", 140}, {"210", 203}, {"201", 201}, {"204", 204}, {"VoordeurKnop",138}, {"Deur1Knop",139}, {"141", 141},
{"Deur2Knop", 137}, {"912071d", 136}, {"b8c72833", 135}, {"Voordeur:", 134}};

void sendToSlave(uint8_t key, uint8_t set, uint8_t slaveAddr) {
    I2CConnection slave(slaveAddr);
    cout<<static_cast<int>(set)<<endl;
    vector<uint8_t> sendNum = {key, set};
    slave.sendI2CTo(1, sendNum, 3);
}

void sendToCO2(uint8_t set) {
    I2CConnection slave(tempSlave);

    vector<uint8_t> sendNum = {set};

    slave.sendI2CTo(1,sendNum,2);

    cout<<static_cast<int>(sendNum.at(0))<<endl;

    cout<<"Test2"<<endl;
    vector<uint8_t> received = {0};

    while(received.at(0) != 1) {
        received = slave.receive6OverI2C(0);
    }

    for (int i = 0; i < static_cast<int>(received.size()); i++) {
        cout<<static_cast<int>(received.at(i))<<endl;
    }

    cout<<bitset<8>(received.at(1)) << " " << bitset<8>(received.at(2)) << endl;
    uint16_t CO2 = (received.at(1) << 8 | received.at(2));

    cout<<bitset<16>(CO2)<<endl;

    cout<<"Test4"<<endl;
    string data = "RCO2: ";

    vector<uint8_t>::iterator it = received.begin() + 3;
    data += to_string(static_cast<uint16_t>(CO2)) + " ";
    for (; it != received.end(); it++) {
        //if (*it != 0) {
            data += to_string(static_cast<uint8_t>(*it)) + " ";
        //}
    }

    SocketClient client(8080, OTHER_PI);
    client.sendData(data);

    cout<<endl;
}

void sendToTemp(uint8_t set) {
    I2CConnection slave(tempSlave);

    vector<uint8_t> sendNum = {set};

    slave.sendI2CTo(1, sendNum, 2);

    cout<<static_cast<int>(sendNum.at(0))<<endl;

    cout<<"Test2"<<endl;
    vector<uint8_t> received = {0};

    while (received.at(0) != 1) {
        received = slave.receive6OverI2C(0);
    }
    
    for (int i = 0; i < static_cast<int>(received.size()); i++) {
        cout<<static_cast<int>(received.at(i))<<endl;
    }

    cout<<"Test4"<<endl;
    string data = "RTemp: ";
    vector<uint8_t>::iterator it = received.begin() + 1;
    for (; it != received.end(); it++) {
            data += to_string(static_cast<uint8_t>(*it))+ " ";
    }

    size_t p = data.find(' ', 8);
    if (p != string::npos) {
        data.at(p) = '.';
    }
    
    SocketClient client(8080, OTHER_PI);
    client.sendData(data);

    cout<<endl;
}

void sendServo(uint8_t key, uint8_t set){
    I2CConnection slave(servoSlave);
    cout<<static_cast<int>(set)<<endl;
    vector<uint8_t> sendNum = {key, set};

    slave.sendI2CTo(1, sendNum, 3);
}

void sendDrukSensor(uint8_t key) {
    I2CConnection slave(servoSlave);
    vector<uint8_t> sendNum = {key};

    slave.sendI2CTo(1,sendNum,2);

    cout<<static_cast<int>(sendNum.at(0))<<endl;

    vector<uint8_t> received = {0};

    while(received.at(0) != 1) {
        received = slave.receive6OverI2C(0);
    }

    for (int i = 0; i < static_cast<int>(received.size()); i++) {
        cout<<static_cast<int>(received.at(i))<<endl;
    }

    string data = "RDruk: " + to_string(received.at(2));
    cout << data << endl;

    SocketClient client(8080, OTHER_PI);
    client.sendData(data);

    cout<<endl;
}
void sendSetTemp(uint8_t key, uint8_t set) {
    I2CConnection slave(tempSlave);
    cout<<static_cast<int>(set)<<endl;
    vector<uint8_t> sendNum = {key, set};

    slave.sendI2CTo(1, sendNum, 3);
}

void sendSetCO2(uint8_t key, uint8_t set)  {
    I2CConnection slave(tempSlave);
    cout<<static_cast<int>(set)<<endl;

    uint8_t part1 = (uint8_t) (set >> 8);
    //cout<<static_cast<int>(part1)<<endl;
    uint8_t part2 = (uint8_t) (set & 0xFF);
    //cout<<static_cast<int>(part2)<<endl;

    vector<uint8_t> sendNum = {key, part1, part2};

    slave.sendI2CTo(1, sendNum, 4);
}

int checkReceived(string received) {
    size_t pos = received.find(' ');
    string cmd = (pos == string::npos) ? received : received.substr(0, pos);
    auto it = command.find(cmd);
    if (it != command.end()) {
        return it->second;
    }
    else {
        return 0xFF;
    }
}

int main(void) {
    SocketServer server(7070, THIS_PI);

    server.serverListen();

    while(true) {
        server.serverAccept();
        string received = server.receiveData();
        int respons = checkReceived(received);
        cout << "Respons int: " << (int)respons << endl;
        if (respons >= 0x70 && respons < 0x80) {
            cout<<"Van Wemos Pi info ontvangen"<<endl;
        }
        if (respons == 203) {
            sendToCO2(respons);
        }
        else if (respons == 138 || respons == 139 || respons == 137){           
            size_t pos = received.find(' ');
            uint8_t toSet = static_cast<uint8_t>(stoi(received.substr(pos)));
            cout << "respons: " << respons << " set: " << (int)toSet << endl;
            sendToSlave(respons, toSet, servoSlave);
            //sendServo(respons, toSet);
        }
        else if (respons == 135) {
            sendToSlave(137, 120, servoSlave);
        }
        else if (respons == 136) {
            sendToSlave(139, 120, servoSlave);
        }
        else if (respons == 134) {
            size_t pos = received.find(' ');
            cout<<"Substring: " << received.substr(pos);
            if (received.substr(pos+1) == "912071d" || received.substr(pos+1) == "b8c72833") {
                sendToSlave(138, 120, servoSlave);
            }
        }
        else if (respons == 141){           
            sendToSlave(respons, 0, servoSlave);
            //sendServo(respons, 0);
        }
        else if (respons == 140){
            sendDrukSensor(respons);
        }
        else if (respons == 200) {
            sendToTemp(respons);
        }
        else if (respons == 204) {
            size_t pos = received.find(' ');
            uint16_t toSet = static_cast<uint16_t>(stoi(received.substr(pos)));
            sendSetCO2(respons, toSet);
        }
        else if (respons == 201) {
            size_t pos = received.find(' ');
            uint8_t toSet = static_cast<uint8_t>(stoi(received.substr(pos)));
            sendToSlave(respons, toSet, tempSlave);
        }
    
        server.closeClientConnection();
    }
}
