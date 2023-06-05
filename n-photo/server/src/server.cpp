#include "server.h"

Server::Server(QTcpServer *parent)
        :QTcpServer(parent)
{
}
void Server::incomingConnection(int desctiptor)
{
    new ConnectionSocket(this, desctiptor);
}
