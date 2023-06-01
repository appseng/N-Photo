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

ConnectionSocket::ConnectionSocket(QObject* parent, int descriptor, DatabaseWork* dbase)
    :QTcpSocket(parent), db(dbase)
{
   connect(this, SIGNAL(readyRead()), this, SLOT(readMessage()));
   connect(this, SIGNAL(disconnected()), this, SLOT(deleteLater()));
   this->setSocketDescriptor(descriptor);
}

void ConnectionSocket::readMessage()
{
    if(this->bytesAvailable() == 0) return;

    // получение данных
    int fileIndex;
    unsigned int messageType;
    QDataStream in(this);
    in >> messageType;
    if (messageType == None)
        return;

    in >> fileIndex;

    // отправка данных
    QByteArray data;
    QDataStream stream (&data, QIODevice::ReadWrite);
    if (messageType == File) {
        QString name;
        bool ok = false;
        QByteArray image = db->Image(ok, fileIndex+1, name);
        stream << messageType << image.size();
        data.append(image);
        this->write(data);
    }
    else if (messageType == List) {
        bool ok = true;
        QList<QString> entries = (db)?db->listImages(ok):QList<QString>();

        int count = entries.count();
        stream << messageType << count;

        for (int i = 0; i < count; i++)
            stream << entries.at(i) << int(0);

        this->write(data);
    }
}

