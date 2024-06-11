/*!
    @file socketserver.h
	@brief Header file voor de QTSocketServer klasse.

	Deze header file definieert de klasse voor het maken en gebruiken van een socket server.
*/
#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

/*!
    @class QTSocketServer
    @brief Een klasse om socket verbindingen mee op te vangen.
*/
class QTSocketServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit QTSocketServer(QString, quint16,QObject *parent = nullptr);
    ~QTSocketServer();

public slots:
    void startServer();
    void stopServer();
    void sendMessageToClients(const QString& message);
    void checkReceived(QString);

private slots:
    void handleNewConnection();
    void readyRead();

signals:
    ///@brief Signaal om de voordeur open of dicht te maken.
    ///@param isOpen Bepaalt of de deur open of dicht moet.
    void Voordeur(bool isOpen);

    ///@brief Signaal om deur 1 te openen of te sluiten.
    ///@param isOpen Bepaalt of de deur open of dicht moet.
    void Deur1(bool isOpen);

    ///@brief Signaal om deur 2 te openen of te sluiten.
    ///@param isOpen Bepaalt of de deur open of dicht moet.
    void Deur2(bool isOpen);

private:
    QList<QTcpSocket*> clients;     /*!< Lijst met de verbonden clients.*/
    QString IP;                     /*!< Het IP van deze server.*/
    quint16 poort;                  /*!< De poort van deze server.*/
};

#endif // SOCKETSERVER_H
