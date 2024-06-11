/*!
    @file socketserver.cpp
	@date 25 May 2024
	@author Jesse Starmans
	@brief CPP file voor de QTSocketServer klasse.

    De implementatie van de QTSocketServer klasse.

    Deze klasse maakt het mogelijk om een socket server aan te maken, waarnaar verbonden kan worden door socket clients om data te versturen en ontvangen.

    Created on 14 May 2024
    by Jesse Starmans. \n 
    Modified on 16 May 2024
    by Jesse Starmans
*/
#include "socketserver.h"
#include "defines.h"
#include <QCoreApplication>
#include <QDebug>

/*!
    @brief Constructor voor de QTSocketServer klasse.

    Deze constructor maak een QTSocketServer onject aan.

    @param host Het IP adres waar de socket server naar moet gaan luisteren.
    @param port De pport waar de socket server naar moet gaan luisteren.
*/
QTSocketServer::QTSocketServer(QString host, quint16 port, QObject *parent) : QTcpServer(parent), IP(host), poort(port)
{
    connect(this, &SocketServer::newConnection, this, &SocketServer::handleNewConnection);
}

/*!
    @brief Destructor voor de QTSocketServer klasse.

    Deze destructor stopt de socket server en sloopt het QTSocketServer object.
*/
QTSocketServer::~QTSocketServer() {
    stopServer();
}

/*!
    @brief Start de socket server.

    Deze functie start de socket server op.
*/
void QTSocketServer::startServer()
{
    QHostAddress hostAddress(IP);
    if (!listen(hostAddress, poort)) {
        qDebug() << "Failed to start server:" << errorString();
        return;
    }
    qDebug() << "Server started, listening on port" << poort;
}

/*!
    @brief Stopt de socket server.

    Deze functie verbreekt eerst de verbindingen met de clients en stopt vervolgens de socket server.
*/
void QTSocketServer::stopServer()
{
    for (QTcpSocket* client : clients) {
        client->disconnectFromHost();
    }
    close();
}

/*!
    @brief Accepteert een nieuwe client verbinding.

    Deze functie wordt aangeroepen op het moment dat een client probeert te verbinden met de server.
*/
void QTSocketServer::handleNewConnection()
{
    while (hasPendingConnections()) {
        QTcpSocket* clientSocket = nextPendingConnection();
        qDebug() << "New connection from:" << clientSocket->peerAddress().toString();
        clients.append(clientSocket);
        connect(clientSocket, &QTcpSocket::readyRead, this, &SocketServer::readyRead);
        connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);
    }
}

/*!
    @brief Checkt of er data binnengekomen is en leest dit in.

    Deze functie checkt of er data binnengekomen is van een client en leest dit dan in.
*/
void QTSocketServer::readyRead()
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

/*!
    @brief Stuurt de meegegeven QString naar alle clients.

    Deze functie stuurt de meegegeven QString naar alle clients die verbonden zijn met de server.

    @param message De te versturen QString.
*/
void QTSocketServer::sendMessageToClients(const QString& message)
{
    QByteArray byteArray = message.toUtf8();
    for (QTcpSocket* client : clients) {
        client->write(byteArray);
    }
}

/*!
    @brief Checkt wat er gedaan moet worden met de binnengekomen data.

    Deze functie checkt wat er gedaan moet worden met de meegegeven QString in de interface.
*/
void QTSocketServer::checkReceived(QString message) {
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
