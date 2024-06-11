#include "socketclient.h"
#include <QCoreApplication>
#include <QDebug>

SocketClient::SocketClient(QString host, quint16 port, QObject *parent) : QObject(parent), IP(host), poort(port), status(false)
{
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, this, &SocketClient::connected);
    connect(socket, &QTcpSocket::disconnected, this, &SocketClient::disconnected);
    connect(socket, &QTcpSocket::readyRead, this, &SocketClient::readyRead);
}

SocketClient::~SocketClient() {
    disconnectFromServer();
    //delete(socket); // Glenn: added want memory leak (27-05)
}

void SocketClient::connectToServer()
{
    socket->connectToHost(IP, poort);
    if (socket->waitForConnected(5000)){
        return;
    }
    else {
        qDebug() << "Fout bij het verbinden:" << socket->errorString();
    }
}

void SocketClient::disconnectFromServer()
{
    socket->disconnectFromHost();
}

void SocketClient::processMessage(const QString& message)
{
    if (message == "Deur 1 dicht")
    {
        // Sluit deur 1
        //emit closeDoor1();
    }
    // Voeg eventueel andere berichtverwerking hier toe
}


void SocketClient::sendMessage(QString message)
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

bool SocketClient::getStatus() {
    return status;
}

void SocketClient::connected()
{
    qDebug() << "Connected to server";
}

void SocketClient::disconnected()
{
    qDebug() << "Disconnected from server";
    //connectToServer();
}

void SocketClient::readyRead()
{
    QByteArray data = socket->readAll();
    QString message = QString::fromUtf8(data);
    qDebug() << "Received message:" << message;
    processMessage(message);
}
