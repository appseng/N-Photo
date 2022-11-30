#include <QMessageBox>
#include "server.h"
#include "clientsocket.h"

Server::Server(QObject *parent)
        :QTcpServer(parent)
{
    db = new DatabaseWork(this);
}
void Server::incomingConnection(int descriptor)
{
    ClientSocket *socket = new ClientSocket(db,this);
    socket->setSocketDescriptor(descriptor);
}
