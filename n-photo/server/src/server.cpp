#include "server.h"

Server::Server(QTcpServer *parent)
        :QTcpServer(parent)
{
}
void Server::incomingConnection(int desctiptor)
{
    DatabaseWork* db = DatabaseWork::Instance();
    new ConnectionSocket(this, desctiptor, db);
}
