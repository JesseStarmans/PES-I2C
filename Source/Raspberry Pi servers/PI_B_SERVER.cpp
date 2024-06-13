#include "SocketServer.h"
#include "SocketClient.h"
#include "I2CConnection.h"
#include <bitset>
#include <map>
#include <unistd.h>

#define tempSlave 0x20
#define buzzerSlave 0x10
#define servoSlave 0x30

#define OTHER_PI "10.42.0.251"
#define THIS_PI "10.42.0.1"

// #define OTHER_PI "145.52.127.184"
// #define THIS_PI "145.52.127.177"

using namespace std;
map<string, int> command{{"Error", 0x00}, {"Disconnected", 0x01}, {"CheckTemp", 200}, {"Check Luchtvochtig", 0x23}, 
{"140", 140}, {"207", 207}, {"210", 203}, {"201", 201}, {"204", 204}, {"VoordeurKnop",138}, {"Deur1Knop",139}, {"141", 141},
{"Deur2Knop", 137}, {"912071d", 136}, {"b8c72833", 135}, {"Voordeur:", 134}};

void sendToSlave(uint8_t key, uint8_t set, uint8_t slaveAddr) {
    I2CConnection slave(slaveAddr);
    vector<uint8_t> sendNum = {key, set};
    slave.sendI2CTo(1, sendNum, 3);
}

void sendToCO2(uint8_t set) {
    // Stuur naar STM
    I2CConnection slave(tempSlave);
    vector<uint8_t> sendNum = {set};
    slave.sendI2CTo(1,sendNum,2);

    // receive van STM
    vector<uint8_t> received = {0};
    while(received.at(0) != 1) {
        received = slave.receive6OverI2C(0);
    }

    // bitshift op juiste Co2 waarde te ontvangen
    uint16_t CO2 = (received.at(1) << 8 | received.at(2));

    string data = "RCO2: ";
    {
        // Zend alarmstatus naar buzzer STM 
        I2CConnection alarmSlave(buzzerSlave);
        vector<uint8_t> sendNum = {206, received.at(3)};
        alarmSlave.sendI2CTo(1,sendNum,2);
    
    }

    // Stuur data naar PI_A
    vector<uint8_t>::iterator it = received.begin() + 3;
    data += to_string(static_cast<uint16_t>(CO2)) + " ";
    for (; it != received.end(); it++) {
            data += to_string(static_cast<uint8_t>(*it)) + " ";
    }
    SocketClient client(8080, OTHER_PI);
    client.sendData(data);

    cout<<endl;
}

void sendToTemp(uint8_t set) {
    // Stuur naar STM
    I2CConnection slave(tempSlave);
    vector<uint8_t> sendNum = {set};
    slave.sendI2CTo(1, sendNum, 2);

    // Receive van STM
    vector<uint8_t> received = {0};
    while (received.at(0) != 1) {
        received = slave.receive6OverI2C(0);
    }
    
    // Converteer uint8 naar string om te sturen
    string data = "RTemp: ";
    vector<uint8_t>::iterator it = received.begin() + 1;
    for (; it != received.end(); it++) {
            data += to_string(static_cast<uint8_t>(*it))+ " ";
    }
    // zet punt voor decimale temperatuur
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
    vector<uint8_t> sendNum = {key, set};

    slave.sendI2CTo(1, sendNum, 3);
}

void sendDrukSensor(uint8_t key) {
    I2CConnection slave(servoSlave);
    vector<uint8_t> sendNum = {key};

    slave.sendI2CTo(1,sendNum,2);

    vector<uint8_t> received = {0};

    while(received.at(0) != 1) {
        received = slave.receive6OverI2C(0);
    }

    string data = "RDruk: " + to_string(received.at(2));

    if (received.at(2) == 1) {
        I2CConnection slaveBeweging(buzzerSlave);
        vector<uint8_t> toSend = {208};
        slaveBeweging.sendI2CTo(1, toSend, 2);
    }

    SocketClient client(8080, OTHER_PI);
    client.sendData(data);

    cout<<endl;
}
void sendSetTemp(uint8_t key, uint8_t set) {
    // stuur naar STM
    I2CConnection slave(tempSlave);
    vector<uint8_t> sendNum = {key, set};
    slave.sendI2CTo(1, sendNum, 3);
}

void sendSetCO2(uint8_t key, uint16_t set)  {
    I2CConnection slave(tempSlave);
    // convert uint16 naar uint8 met bitshifting
    uint8_t part1 = (uint8_t) (set >> 8);
    uint8_t part2 = (uint8_t) (set & 0xFF);
    // stuur naar STM
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
    // server setup
    SocketServer server(7070, THIS_PI);
    server.serverListen();

    while(true) { // main loop
        server.serverAccept(); // wacht tot er een request binnenkomt
        string received = server.receiveData();
        int respons = checkReceived(received); // functie om received te converten naar juiste command
        
        if (respons == 134) {
            size_t pos = received.find(' ');
            if (received.substr(pos+1) == "912071d" || received.substr(pos+1) == "b8c72833") {
                sendToSlave(138, 120, servoSlave);
            }
        }
        else if (respons == 135) { // RFID tag voor deuren
            sendToSlave(137, 120, servoSlave);
        }
        else if (respons == 136) { // RFID tag voor deuren
            sendToSlave(139, 120, servoSlave);
        }
        else if (respons == 137 || respons == 138 || respons == 139){ // Open deuren            
            size_t pos = received.find(' ');
            uint8_t toSet = static_cast<uint8_t>(stoi(received.substr(pos)));
            sendToSlave(respons, toSet, servoSlave);
        }
        else if (respons == 140){ // lees druksensor uit
            sendDrukSensor(respons);
        }
        else if (respons == 141){ // switch mode van gordijnen naar deuren en visa versa 
            sendToSlave(respons, 0, servoSlave);
        }
        else if (respons == 200) { // vraag temperatuur op
            sendToTemp(respons);
        }
        else if (respons == 201) {
            size_t pos = received.find(' ');
            uint8_t toSet = static_cast<uint8_t>(stoi(received.substr(pos)));
            sendToSlave(respons, toSet, tempSlave);
        }
        if (respons == 203) { // vraag Co2 waarde op
            sendToCO2(respons);
        }
        else if (respons == 204) { // set grenswaarde alarm Co2
            size_t pos = received.find(' ');
            uint16_t toSet = static_cast<uint16_t>(stoi(received.substr(pos)));
            sendSetCO2(respons, toSet);
        }
        else if (respons == 207){ // buzzer reset
            sendSetCO2(204, 10000); // set hoge grenswaarde om alarm te resetten
            usleep(500000); // Delay anders kan STM het niet aan
            sendToCO2(203); // request nieuwe Co2 waarde
        }
        
        server.closeClientConnection();
    }
}
