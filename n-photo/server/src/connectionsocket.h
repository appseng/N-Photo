#ifndef CONNECTIONSOCKET_H
#define CONNECTIONSOCKET_H

#include <QTcpSocket>
#include "databasework.h"
#define QDATASTEAMVERSION 17

class ConnectionSocket : public QTcpSocket
{
    Q_OBJECT
public:
    ConnectionSocket(int, DatabaseWork *, QObject* = nullptr);
private:
    DatabaseWork* db;
public slots:
    void readMessage();
};

#endif // CONNECTIONSOCKET_H
