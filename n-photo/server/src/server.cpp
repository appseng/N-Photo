#include "server.h"

Server::Server(QTcpServer *parent)
        :QTcpServer(parent)
{
    db = new DatabaseWork(this);
}
void Server::incomingConnection(int desctiptor)
{
    socket = new ConnectionSocket(desctiptor, db, this);
}
