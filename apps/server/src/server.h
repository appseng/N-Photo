#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>

class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server(QTcpServer * = nullptr);
private slots:
    void connection();
    void ready(QTcpSocket*);
};

#endif // SERVER_H
