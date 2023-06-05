#ifndef CONNECTIONSOCKET_H
#define CONNECTIONSOCKET_H

#include <QTcpSocket>
#include "databasework.h"

class ConnectionSocket : public QTcpSocket
{
    Q_OBJECT
public:
    ConnectionSocket(QObject*, int);
public slots:
    void messageExchange();
};

#endif // CONNECTIONSOCKET_H
