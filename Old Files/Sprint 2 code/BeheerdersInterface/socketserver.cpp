#include "socketserver.h"
#include "defines.h"
#include <QCoreApplication>
#include <QDebug>

SocketServer::SocketServer(QString host, quint16 port, QObject *parent) : QTcpServer(parent), IP(host), poort(port)
{
    connect(this, &SocketServer::newConnection, this, &SocketServer::handleNewConnection);
}

SocketServer::~SocketServer() {
    stopServer();
}

void SocketServer::startServer()
{
    QHostAddress hostAddress(IP);
    if (!listen(hostAddress, poort)) {
        qDebug() << "Failed to start server:" << errorString();
        return;
    }
    qDebug() << "Server started, listening on port" << poort;
}

void SocketServer::stopServer()
{
    for (QTcpSocket* client : clients) {
        client->disconnectFromHost();
    }
    close();
}

void SocketServer::handleNewConnection()
{
    while (hasPendingConnections()) {
        QTcpSocket* clientSocket = nextPendingConnection();
        qDebug() << "New connection from:" << clientSocket->peerAddress().toString();
        clients.append(clientSocket);
        connect(clientSocket, &QTcpSocket::readyRead, this, &SocketServer::readyRead);
        connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);
    }
}

void SocketServer::readyRead()
{
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (!clientSocket)
        return;

    QByteArray data = clientSocket->readAll();
    QString message = QString::fromUtf8(data);
    qDebug() << "Received message from" << clientSocket->peerAddress().toString() << ":" << message;

    // Voeg hier eventuele berichtverwerking toe of stuur het bericht naar alle andere clients
    checkReceived(message);
}

void SocketServer::sendMessageToClients(const QString& message)
{
    QByteArray byteArray = message.toUtf8();
    for (QTcpSocket* client : clients) {
        client->write(byteArray);
    }
}

void SocketServer::checkReceived(QString message) {
    if (message == "Voordeur open") {
        emit Voordeur(OPEN);
    }
    else if (message == "Voordeur dicht") {
        emit Voordeur(DICHT);
    }
    if (message == "Deur 1 open") {
        emit Deur1(OPEN);
    }
    else if (message == "Deur 1 dicht") {
        emit Deur1(DICHT);
    }
    if (message == "Deur 2 open") {
        emit Deur2(OPEN);
    }
    else if (message == "Deur 2 dicht") {
        emit Deur2(DICHT);
    }

}
