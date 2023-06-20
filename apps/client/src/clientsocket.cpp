#include <QDataStream>
#include <QByteArray>

#include "clientsocket.h"

ClientSocket::ClientSocket(QObject* parent) :
    QTcpSocket(parent)
{
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)),
            SLOT(error(QAbstractSocket::SocketError)));
    connect(this, SIGNAL(readyRead()), SLOT(ready()));
    connect(this, SIGNAL(connected()), SLOT(clientConnected()));
    dataSize = 0;
}
void ClientSocket::clientConnect(const MessageType type, const QString host, const unsigned short port, const int row) 
{
    messageType = type;
    dataSize = 0;
    this->row = row;
    abort();
    connectToHost(host, port);
}
void ClientSocket::clientConnected()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    unsigned int data = 0;
    if (messageType == File)
        data = (row & 0x0FFFFFFF) | (messageType << 28);
    else if (messageType == List)
        data = messageType << 28;

    out << data;

    write(block);
}
void ClientSocket::error(QAbstractSocket::SocketError error)
{
   if (error == QAbstractSocket::RemoteHostClosedError)
       return;

   emit sendError();
}
void ClientSocket::ready()
{
    if (bytesAvailable() < 4) return;

    QDataStream stream(this);
    unsigned int data;
    if (dataSize == 0) {
        stream >> data;
        dataSize = data & 0x0FFFFFFF;
        messageType = MessageType(data >> 28);
    }
    if (messageType == File) {   // get image file
        if (bytesAvailable() < dataSize)
            return;

        QByteArray image;
        stream >> image;

        emit sendImage(image);
        dataSize = 0;
    }
    else if (messageType == List) { // get directory list
        QList<QString> list;
        stream >> list;
        emit sendImageList(list);
        dataSize = 0;
    }
    else // incorrect type
        emit incorrectType(messageType);
}
