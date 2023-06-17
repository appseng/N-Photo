#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <QTcpSocket>

#include "enums.h"

class ClientSocket: public QTcpSocket
{
    Q_OBJECT
public:
    ClientSocket(QObject* = nullptr);
    void clientConnect(const MessageType, const QString, const unsigned short, const int);
private slots:
    void clientConnected();
    void error(QAbstractSocket::SocketError);
    void ready();
signals:
    void sendImage(int);
    void sendError();
    void sendImageList(QList<QString>);
    void incorrectType(MessageType);
private:
    int dataSize;
    MessageType messageType;
    int row;
};

#endif // CLIENTSOCKET_H
