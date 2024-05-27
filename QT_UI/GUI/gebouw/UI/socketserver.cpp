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
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket)
        return;

    QByteArray data = clientSocket->readAll();
    QString message = QString::fromUtf8(data);
    qDebug() << "Received message from" << clientSocket->peerAddress().toString() << ":" << message;

    // Voeg hier eventuele berichtverwerking toe of stuur het bericht naar alle andere clients
    checkReceived(message);
}

void SocketServer::sendMessageToClients(const QString &message){
    QByteArray byteArray = message.toUtf8();
    for (QTcpSocket* client : clients) {
        client->write(byteArray);
    }
}

void SocketServer::checkReceived(const QString &message) {
    int msg;
    // Veilig string to int met exception handeling
    int key = toInt(message.toStdString().substr(0,5));

    std::string secondPart = message.toStdString().substr(7); // rekening houden met spatie tussen firstPart en second

    switch(key){
        case 0x10:
            qInfo() << "Voordeur open/dicht";
            msg = toInt(secondPart);
            emit Voordeur(msg);
            break;
        case 0x11:
            qInfo() << "Deur 1 open/dicht";
            msg = toInt(secondPart);
            emit Deur1(msg);
            break;
        case 0x12:
            qInfo() << "Deur 2 open/dicht";
            msg = toInt(secondPart);
            emit Deur2(msg);
            break;
        case 0x20:
            qInfo() << "Temperatuur update";
            emit Temperatuur(secondPart);
            break;
        case 0x21:
            qInfo() << "Co2 update";
            msg = toInt(secondPart);
            emit Co2(msg);
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
