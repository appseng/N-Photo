#include <QTcpSocket>
#include <QDataStream>
#include <QByteArray>
#include <QList>
#include <QString>

#include "server.h"
#include "enums.h"

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

    const int mask = 0x0FFFFFFF;
    // получение данных
    QDataStream in(socket);
    unsigned int read;
    in >> read;
    MessageType messageType = MessageType(read >> 28);

    // отправка данных
    QByteArray block;
    QDataStream data (&block, QIODevice::WriteOnly);
    if (messageType == File) {
        QString name;
        bool ok = true;
        int fileIndex = read & mask;

        QByteArray image = databaseWork.getImage(ok, fileIndex+1, name);

        if (ok) {
            int meta = (messageType << 28) | (image.size() & mask);
            data << meta << image;

            socket->write(block);
        }
    }
    else if (messageType == List) {
        bool ok = true;
        QList<QString> list = databaseWork.imageList(ok);
        if (ok) {
            QByteArray bites;
            QDataStream stream(&bites, QIODevice::WriteOnly);
            stream << list;
            int meta = (messageType << 28) | (bites.size() & mask);
            data << meta << list;

            socket->write(block);
        }
    }
}
