#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QThreadStorage>
#include "databasework.h"

class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server(QObject * = 0);
protected:
    void incomingConnection(int);
private:
    DatabaseWork *db;

};

#endif // SERVER_H
