#include <QDataStream>

#include "clientsocket.h"

ClientSocket::ClientSocket(QObject* parent) :
    QTcpSocket(parent)
{
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(error(QAbstractSocket::SocketError)));
    connect(this, SIGNAL(readyRead()), this, SLOT(ready()));
    connect(this, SIGNAL(connected()), this, SLOT(clientConnected()));
    dataSize = 0;
}
void ClientSocket::clientConnect(const MessageType type, const QString host, const unsigned short ip, const int row)
{
    messageType = type;
    dataSize = 0;
    this->row = row;
    this->abort();
    this->connectToHost(host, ip);
}
void ClientSocket::clientConnected()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::ReadWrite);
    out << messageType;

    if (messageType == File) {
        out << row;
    }
    else
        out << int(0);

    this->write(block);
}
void ClientSocket::error(QAbstractSocket::SocketError error)
{
   if (error == QAbstractSocket::RemoteHostClosedError)
       return;

   emit sendError();
}
void ClientSocket::ready()
{
    if (this->bytesAvailable() < 2*sizeof(int)) return;

    QDataStream stream(this);
    unsigned int type;
    if (dataSize == 0)
        stream >> type >> dataSize;

    messageType = MessageType(type);
    if (messageType == File) {   // get image file
        if (this->bytesAvailable() < dataSize)
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
