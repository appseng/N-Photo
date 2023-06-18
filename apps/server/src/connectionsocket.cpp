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
    if (bytesAvailable() == 0) return;

    // получение данных
    unsigned int messageType;
    QDataStream in(this);
    in >> messageType;
    if (messageType == None)
        return;

    int fileIndex;
    in >> fileIndex;

    // отправка данных
    QByteArray data;
    QDataStream stream (&data, QIODevice::ReadWrite);
    if (messageType == File) {
        QString name;
        bool ok = false;
        QByteArray image = DatabaseWork::getInstance()->getImage(ok, fileIndex+1, name);
        stream << messageType << image.size();
        data.append(image);
        write(data);
    }
    else if (messageType == List) {
        bool ok = true;
        QList<QString> entries = DatabaseWork::getInstance()->listImages(ok);

        int count = entries.count();
        stream << messageType << count;

        for (int i = 0; i < count; i++)
            stream << entries.at(i) << int(0);

        write(data);
    }
}

