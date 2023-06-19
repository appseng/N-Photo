#include <QDir>
#include <QFile>
#include <QImage>
#include <QFileInfoList>
#include <QBuffer>
#include <QByteArray>
#include <QImageWriter>
#include <QImageWriter>
#include <QDataStream>
#include "connectionsocket.h"
#include "enums.h"

ConnectionSocket::ConnectionSocket(QObject* parent, int descriptor)
    :QTcpSocket(parent)
{
    connect(this, SIGNAL(readyRead()), this, SLOT(messageExchange()));
    connect(this, SIGNAL(disconnected()), this, SLOT(deleteLater()));
    this->setSocketDescriptor(descriptor);
}

void ConnectionSocket::messageExchange()
{
    if (bytesAvailable() < 4) return;

    // получение данных
    QDataStream in(this);
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

        write(block);
    }
    else if (messageType == List) {
        bool ok = true;
        QList<QString> list = DatabaseWork::getInstance()->listImages(ok);

        int send = messageType << 28;
        stream << send << list;

        write(block);
    }
}

