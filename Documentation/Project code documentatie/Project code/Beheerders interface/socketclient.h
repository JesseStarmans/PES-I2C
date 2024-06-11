/*!
    @file socketclient.h
	@brief Header file voor de QTSocketClient klasse.

	Deze header file definieert de klasse voor het maken en gebruiken van een socket client.
*/
#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include <QtNetwork/QTcpSocket>

/*!
    @class QTSocketClient
    @brief Een klasse om socket verbindingen mee op te zetten.
*/
class QTSocketClient : public QObject
{
    Q_OBJECT
public:
    explicit QTSocketClient(QString, quint16, QObject *parent = nullptr);
    ~QTSocketClient();

public slots:
    void connectToServer();
    void disconnectFromServer();
    void sendMessage(QString);
    void processMessage(const QString&);

    bool getStatus();

    void connected();

private slots:
    void disconnected();
    void readyRead();

private:
    QTcpSocket* socket;     /*!< Aangemaakte socket.*/
    QString IP;             /*!< Het IP van de server.*/
    quint16 poort;          /*!< De poort van de server.*/
    bool status;            /*!< Of de client verbonden is aan een server of niet.*/
};

#endif // SOCKETCLIENT_H
