#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include "databasework.h"
#include "connectionsocket.h"

class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server(QTcpServer * = nullptr);
private:
    void incomingConnection(int);
};

#endif // SERVER_H
