@page subMainPage1 I2C Raspberry Pi main
@tableofcontents

@section sectionI2C1 Inleiding
Op deze pagina is de volledige implementatie te vinden van de Raspberry Pi die verbonden is aan de NUCLEO-L432KC bordjes over I2C.
Voor de verbinding met de andere Raspberry Pi wordt er gebruik gemaakt van de SocketClient en SocketServer klasses.
Voor de verbinding naar de microcontrollers wordt er gebruik gemaakt van de I2CConnection klasse.

@subsection subsectionI2C1Builden Het programma builden
Om de code te kunnen builden en runnen, kan er gebruik gemaakt worden van de volgende commando's:
1. Ga naar de map waar het programma staat:
@code
cd /home/pigroepg/Documents/Socket
@endcode
2. Compileer de main met de afhankelijkheden:
@code
g++ -Wall -o I2CTest I2CTest.cpp SocketClient.cpp SocketServer.cpp I2CConnection.cpp
@endcode
3. Run het programma (in een nieuwe terminal voor de overzichtelijkheid):
@code
lxterminal -e ./I2CTest
@endcode

@section sectionI2C2 Klasses

Om de \ref I2CTest.cpp "I2C Raspberry Pi main" code correct te laten werken zijn er drie klasses geschreven:\n
- SocketClient
- SocketServer
- I2CConnection

@section sectionI2C3 Main code

Om de documentatie van deze main te zien, zie: \ref I2CTest.cpp "I2C Raspberry Pi main".\n

Hieronder is de losse main te zien:
@code
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
@endcode