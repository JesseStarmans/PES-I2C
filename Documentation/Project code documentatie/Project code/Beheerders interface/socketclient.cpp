/*!
    @file socketclient.cpp
    @date 25 May 2024
    @author Jesse Starmans
    @brief CPP file voor de QTSocketClient klasse.

    De implementatie van de QTSocketClient klasse.

    Deze klasse maakt het mogelijk om een socket verbinding met een socket server op te zetten, om vervolgens data te kunnen sturen en ontvangen.

    Created on 4 May 2024
    by Jesse Starmans. \n
    Modified on 13 May 2024
    by Jesse Starmans.
*/
#include "socketclient.h"
#include <QCoreApplication>
#include <QDebug>

/*!
    @brief Constructor voor de QTSocketClient klasse.

    Deze constructor maakt een QTSocketClient object aan met de informatie van de socket server waar deze naar moet verbinden.

    @param host Het IP adres van de socket server waar deze socket client naar moet gaan verbinden.
    @param port De poort waarop de socket server, waarnaar verbonden moet worden, luistert voor verbindingen.
*/
QTSocketClient::QTSocketClient(QString host, quint16 port, QObject *parent) : QObject(parent), IP(host), poort(port), status(false)
{
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, this, &QTSocketClient::connected);
    connect(socket, &QTcpSocket::disconnected, this, &QTSocketClient::disconnected);
    connect(socket, &QTcpSocket::readyRead, this, &QTSocketClient::readyRead);
}

/*!
    @brief Destructor voor de QTSocketClient klasse

    Deze destructor laat de socket client de verbinding verbreken naar de server, waarna het object wordt gesloopt.
*/
QTSocketClient::~QTSocketClient() {
    disconnectFromServer();
}

/*!
    @brief Verbindt de client met de server.

    Deze functie zorgt dat de socket client een verbinding gaat aanvragen met de socket server.
*/
void QTSocketClient::connectToServer()
{
    socket->connectToHost(IP, poort);
    if (socket->waitForConnected(5000)){
        return;
    }
    else {
        qDebug() << "Fout bij het verbinden:" << socket->errorString();
    }
}

/*!
    @brief Verbreekt de verbinding met de server.

    Deze functie verbreekt de verbinding tussen deze client en de server.
*/
void QTSocketClient::disconnectFromServer()
{
    socket->disconnectFromHost();
}

/*!
    @brief Verwerkt de binnengekomen data.

    Deze functie verwerkt meegegeven data.

    @param message Een QString met de te verwerken data.
*/
void QTSocketClient::processMessage(const QString& message)
{
    if (message == "Deur 1 dicht")
    {
        // Sluit deur 1
        //emit closeDoor1();
    }
    // Voeg eventueel andere berichtverwerking hier toe
}

/*!
    @brief Stuurt data naar de server.

    Deze functie stuurt een QString naar de socket server, als deze verbonden is aan een server.

    @param message De te versturen QString.
*/
void QTSocketClient::sendMessage(QString message)
{
    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        QByteArray byteArray = message.toUtf8();
        socket->write(byteArray);
        qDebug() << "Send message: " << byteArray;
    }
    else
    {
        qDebug() << "Socket is not connected.";
    }
}

/*!
    @brief Geeft de status van de socket verbinding terug.

    Deze functie geeft de status van de socket verbinding tussen de client en server terug.

    @return De status van de socket verbinding.
*/
bool QTSocketClient::getStatus() {
    return status;
}

/*!
    @brief Geeft aan dat de client verbonden is met een server.

    Deze functie geeft aan dat de client verbonden is met een server.
*/
void QTSocketClient::connected()
{
    qDebug() << "Connected to server";
}

/*!
    @brief Geeft aan dat de client niet (meer) verbonden is met een server.

    Deze functie geeft aan dat de client niet (meer) verbonden is met een server.
*/
void QTSocketClient::disconnected()
{
    qDebug() << "Disconnected from server";
}

/*!
    @brief Wordt aangeroepen wanneer er data ontvangen wordt.

    Deze functie wordt aangeroepen wanneer er data ontvangen wordt.
*/
void QTSocketClient::readyRead()
{
    QByteArray data = socket->readAll();
    QString message = QString::fromUtf8(data);
    qDebug() << "Received message:" << message;
    processMessage(message);
}
