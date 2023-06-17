#include "clientsocket.h"
#include <QDataStream>

ClientSocket::ClientSocket(QObject* parent) :
    QTcpSocket(parent)
{
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(socketError(QAbstractSocket::SocketError)));
    connect(this, SIGNAL(readyRead()), this, SLOT(socketReady()));
    connect(this, SIGNAL(connected()), this, SLOT(socketConnected()));
    dataSize = 0;
}
void ClientSocket::socketConnect(MessageType type, QString host, unsigned short ip, int row)
{
    messageType = type;
    dataSize = 0;
    this->row = row;
    this->abort();
    this->connectToHost(host, ip);
}
void ClientSocket::socketConnected()
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
void ClientSocket::socketError(QAbstractSocket::SocketError error)
{
   if (error == QAbstractSocket::RemoteHostClosedError)
       return;

   emit sendSocketError();
}
void ClientSocket::socketReady()
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

        emit getSocketImage(dataSize);
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
        emit sendSocketList(list);
        dataSize = 0;
    }
    else // incorrect type
        emit socketIncorrect(messageType);
}
