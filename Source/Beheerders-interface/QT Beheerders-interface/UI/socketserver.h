#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

class SocketServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit SocketServer(QString, quint16,QObject *parent = nullptr);
    virtual ~SocketServer();
    static int toInt(const std::string&);

public slots:
    void startServer();
    void stopServer();
    void sendMessageToClients(const QString& message);
    void checkReceived(QString);

private slots:
    void handleNewConnection();
    void readyRead();

signals:
    void Voordeur(bool);
    void Deur1(bool);
    void Deur2(bool);
    void Temperatuur(std::string&);
    void Co2(std::string);
    void Plant(std::string);
    void Druk(std::string);
    void Noodknop();

private:
    QList<QTcpSocket*> clients;
    QString IP;
    quint16 poort;
};

#endif // SOCKETSERVER_H
