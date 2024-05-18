#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include <QtNetwork/QTcpSocket>

class SocketClient : public QObject
{
    Q_OBJECT
public:
    explicit SocketClient(QString, quint16, QObject *parent = nullptr);
    ~SocketClient();

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
    QTcpSocket* socket;
    QString IP;
    quint16 poort;
    bool status;
};

#endif // SOCKETCLIENT_H
