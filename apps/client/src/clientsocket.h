#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <QTcpSocket>

#include "enums.h"

class ClientSocket: public QTcpSocket
{
    Q_OBJECT
public:
    ClientSocket(QObject* = nullptr);
    void socketConnect(MessageType, QString, unsigned short, int);
private slots:
    void socketConnected();
    void socketError(QAbstractSocket::SocketError);
    void socketReady();
signals:
    void getSocketImage(int);
    void sendSocketError();
    void sendSocketList(QList<QString>);
    void socketIncorrect(MessageType);
private:
    int dataSize;
    MessageType messageType;
    int row;
};

#endif // CLIENTSOCKET_H
