#include <QTcpSocket>
#include <QDataStream>
#include <QByteArray>
#include <QList>
#include <QString>

#include "server.h"
#include "enums.h"
#include "databasework.h"

Server::Server(QTcpServer *parent)
        :QTcpServer(parent)
{
    connect(this, SIGNAL(newConnection()), SLOT(connection()));
}

void Server::connection() {
    if (!hasPendingConnections()) return;
    
    QTcpSocket* socket = nextPendingConnection();
    connect(socket, &QTcpSocket::readyRead, this, [=](){ this->ready(socket); });
    connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
}

void Server::ready(QTcpSocket* socket)
{
    if (socket->bytesAvailable() < 4) return;

    // получение данных
    QDataStream in(socket);
    unsigned int read;
    in >> read;
    MessageType messageType = MessageType(read >> 28);
    int fileIndex = read & 0x0FFFFFFF;

    // отправка данных
    QByteArray block;
    QDataStream stream (&block, QIODevice::WriteOnly);
    if (messageType == File) {
        QString name;
        bool ok = false;

        QByteArray image = DatabaseWork::getInstance()->getImage(ok, fileIndex+1, name);
        int send = (messageType << 28) | (image.size() & 0x0FFFFFFF);
        stream << send << image;

        socket->write(block);
    }
    else if (messageType == List) {
        bool ok = true;
        QList<QString> list = DatabaseWork::getInstance()->listImages(ok);
        QByteArray blist;
        QDataStream slist(&blist, QIODevice::WriteOnly);
        slist << list;
        int send = (messageType << 28) | (blist.size() & 0x0FFFFFFF);
        stream << send << list;

        socket->write(block);
    }
}
