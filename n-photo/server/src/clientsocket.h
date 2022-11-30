#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <QDir>
#include <QFile>
#include <QImage>
#include <QTcpSocket>
#include <QFileInfoList>
#include <QBuffer>
#include <QByteArray>
#include <QImageWriter>
#include <QTcpSocket>
#include <QImageWriter>
#include <QDataStream>
#include <QTime>
#include "databasework.h"
class ClientSocket : public QTcpSocket
{
    Q_OBJECT
public:
    ClientSocket(DatabaseWork *,QObject * = 0);

private:
    quint16 nextBlockSize;
    DatabaseWork * db;
private slots:
    void readClient();
};

#endif // CLIENTSOCKET_H
