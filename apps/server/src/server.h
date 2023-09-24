#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include "databasework.h"

class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server(QTcpServer * = nullptr);
private slots:
    void connection();
    void ready(QTcpSocket*);
private:
    DatabaseWork DBWork;
};

#endif // SERVER_H
