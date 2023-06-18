#include <QDataStream>

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
    unsigned int type = (unsigned int)messageType;
    out << type;

    if (messageType == File)
        out << row;
    else
        out << int(0);

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
    if (bytesAvailable() < int(2*sizeof(int))) return;

    QDataStream stream(this);
    unsigned int type;
    if (dataSize == 0)
        stream >> type >> dataSize;

    messageType = MessageType(type);
    if (messageType == File) {   // get image file
        if (bytesAvailable() < dataSize)
            return;

        emit sendImage(dataSize);
        dataSize = 0;
    }
    else if (messageType == List) { // get directory list
        QList<QString> list;
        QString name;
        int separator;
        while (dataSize > 0) {
            stream >> name >> separator;
            list.append(name);
            dataSize--;
        }
        emit sendImageList(list);
        dataSize = 0;
    }
    else // incorrect type
        emit incorrectType(messageType);
}
