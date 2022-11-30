#include <QFileInfo>
#include "clientsocket.h"

ClientSocket::ClientSocket(DatabaseWork * dbase, QObject *parent)
        :QTcpSocket(parent), db(dbase)
{
    connect(this, SIGNAL(readyRead()), this, SLOT(readClient()));
    connect(this, SIGNAL(disconnected()), this, SLOT(deleteLater()));
    nextBlockSize = 0;
}
void ClientSocket::readClient()
{
    int fileIndex;
    quint32 messageType = 8;
    QDataStream in(this);
    in.setVersion(QDataStream::Qt_4_4);
    if (nextBlockSize == 0) {
        if (bytesAvailable() < sizeof(quint32)) {
            readAll();
            return;
        }
        in >> messageType;
        if (messageType == 4){
            if (bytesAvailable() < sizeof(int)) {
                readAll();
                return;
            }
            in >> fileIndex;
        }
    }
    QByteArray data;
    QDataStream stream (&data, QIODevice::WriteOnly);
    stream.setVersion(QDataStream::Qt_4_4);
    if (messageType == 4) {
        QString path;
        bool ok = false;
        QByteArray image = db->Image(ok, fileIndex, path);
        stream << messageType << image.size() << path;
        data.append(image);
    }
    else if (messageType == 8) {
        bool ok = true;
        QList<QString> entries = (db)?db->listImages(ok):QList<QString>();
        quint32 size = (quint32)entries.count();
        for (quint32 i = 0; i < size; i++)
            stream << entries.at(i) << (int)0;
        QByteArray dataD;
        QDataStream streamD (&dataD, QIODevice::WriteOnly);
        streamD.setVersion(QDataStream::Qt_4_4);
        streamD << (quint32)messageType << quint32(data.size() + sizeof(quint32)) << size;
        data.prepend(dataD);
    }

    write(data);
    close();
}
