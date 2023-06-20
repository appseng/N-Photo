#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <QTcpSocket>

#include "enums.h"

class ClientSocket: public QTcpSocket
{
    Q_OBJECT
public:
    ClientSocket(QObject* = nullptr);
    void clientConnect(const MessageType, const QString,
                       const unsigned short, const int);
private slots:
    void clientConnected();
    void error(QAbstractSocket::SocketError);
    void ready();
signals:
    void sendImage(const QByteArray);
    void sendError();
    void sendImageList(const QList<QString>);
    void incorrectType(const MessageType);
private:
    int dataSize;
    MessageType messageType;
    int row;
};

#endif // CLIENTSOCKET_H
