#include "socketserver.h"
#include "defines.h"
#include <QCoreApplication>
#include <QDebug>
#include <string>

SocketServer::SocketServer(QString host, quint16 port, QObject *parent) : QTcpServer(parent), IP(host), poort(port){
    connect(this, &SocketServer::newConnection, this, &SocketServer::handleNewConnection);
}

SocketServer::~SocketServer() {
    stopServer();
    qDeleteAll(clients.begin(), clients.end()); // Glenn: added want memory leak (27-05)
}

void SocketServer::startServer(){
    QHostAddress hostAddress(IP);
    if (!listen(hostAddress, poort)) {
        qDebug() << "Failed to start server:" << errorString();
        return;
    }
    qDebug() << "Server started, listening on port" << poort;
}

void SocketServer::stopServer(){
    for (QTcpSocket* client : clients) {
        client->disconnectFromHost();
    }
    close();
}

void SocketServer::handleNewConnection(){
    while (hasPendingConnections()) {
        QTcpSocket* clientSocket = nextPendingConnection();
        qDebug() << "New connection from:" << clientSocket->peerAddress().toString();
        clients.append(clientSocket);
        connect(clientSocket, &QTcpSocket::readyRead, this, &SocketServer::readyRead);
        connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);
    }
}

void SocketServer::readyRead(){
    qInfo() << "Ready to read";
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket){
        return;
    }

    QByteArray data = clientSocket->readAll();
    QString message = QString::fromUtf8(data);

    message.remove('\r');
    message.remove('\n');

    qDebug() << "Received message from" << clientSocket->peerAddress().toString() << ":" << message;

    // Voeg hier eventuele berichtverwerking toe of stuur het bericht naar alle andere clients
    checkReceived(message);
    clientSocket->close();
}

void SocketServer::sendMessageToClients(const QString &message){
    QByteArray byteArray = message.toUtf8();
    for (QTcpSocket* client : clients) {
        client->write(byteArray);
    }
}

void SocketServer::checkReceived(QString message) {
    int msg, key;
    //qInfo() << message;

    std::string temp = message.toStdString().substr(0,message.indexOf(':') + 1);
    //qInfo() << "Temp: " << temp;
    if(temp == "RTemp:"){
        key = 200;
    }
    else if (temp == "Voordeur:") {
        key = 10;
    }
    else if(temp == "Deur1:") {
        key = 11;
    }
    else if(temp == "Deur2:") {
        key = 12;
    }
    else if(temp == "RCO2:") {
        key = 203;
    }
    else if (temp == "RPl:"){
        key = 210;
    }
    else if (temp == "RDruk:"){
        key = 220;
    }
    else if (temp == "Noodknop:") {
        key = 250;
    }
    else {
        key = 1000;
    }

    std::string secondPart = message.toStdString().substr(message.indexOf(' ') + 1); // rekening houden met spatie tussen firstPart en second
    qInfo() << secondPart;
    switch(key){
        case 10:
            qInfo() << "Voordeur open/dicht";
            if (secondPart == "open") {
                emit Voordeur(OPEN);
            }
            else {
                emit Voordeur(DICHT);
            }
            //emit Voordeur(DICHT);
            break;
        case 11:
            qInfo() << "Deur 1 open/dicht";
            if (secondPart == "open") {
                emit Deur1(OPEN);
            }
            else {
                emit Deur1(DICHT);
            }
            break;
        case 12:
            qInfo() << "Deur 2 open/dicht";
            if (secondPart == "open") {
                emit Deur2(OPEN);
            }
            else {
                emit Deur2(DICHT);
            }
            break;
        case 200:
            qInfo() << "Temperatuur update";
            emit Temperatuur(secondPart);
            break;
        case 203:
            qInfo() << "Co2 update";
            qInfo() << secondPart;
            emit Co2(secondPart);
            break;
        case 210:
            qInfo() << "Plant update";
            emit Plant(secondPart);
            break;
        case 220:
            qInfo() << "Druk update";
            emit Druk(secondPart);
            break;
        case 250:
            qInfo() << "Noodknop ingedrukt";
            emit Noodknop();
            break;
        default:
            qInfo() << "Incorrect key";
    }

    // std::string firstPart = message.toStdString().substr(0,13);
    // std::string secondPart = message.toStdString().substr(13);
    // if (message == "Voordeur open") {
    //     emit Voordeur(OPEN);
    // }
    // else if (message == "Voordeur dicht") {
    //     emit Voordeur(DICHT);
    // }
    // if (message == "Deur 1 open") {
    //     emit Deur1(OPEN);
    // }
    // else if (message == "Deur 1 dicht") {
    //     emit Deur1(DICHT);
    // }
    // if (message == "Deur 2 open") {
    //     emit Deur2(OPEN);
    // }
    // else if (message == "Deur 2 dicht") {
    //     emit Deur2(DICHT);
    // }
    // if (message.toStdString().substr(0,13); == "Temperatuur: "){
    //     // work in progess (23-05-2024)

    //     emit Temperatuur(true);
    // }
    // else if (message.toStdString().substr(0,5); == "Co2: "){
    //     // work in progess (23-05-2024)
    // }
}

int SocketServer::toInt(const std::string &message){
/*
    todo: Verplaats dit naar een algemene class. Want is een beetje raar dat dit in SocketServer staat terwijl het ook in mainWindow wordt gebruikt
*/
    /*------------ Veilig string to int met exception handeling ----------*/
    try{
        return std::stoi(message);
    }catch(const std::invalid_argument&  invalid){
        qDebug() << invalid.what();
    }catch(const std::out_of_range& out){
        qDebug() << out.what();
    }catch(...){
        qDebug() << "Error receiving message";
    }
    return -128;
}
