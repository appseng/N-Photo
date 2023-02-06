#include <QApplication>
#include <QFileInfo>
#include "server.h"
#include "databasework.h"

const quint16 defPort = 5500;

bool start (Server &server,bool ok = false, quint16 port = defPort) {

    if (!server.listen(QHostAddress::Any, (ok) ? port : defPort)) {
        qCritical() << "A server for N-Photo." << endl
                    << "It's impossible to launch server: " << server.errorString() << "."  << endl;
        return false;
    }
    qDebug() << "A server started on " << server.serverPort() << " port." << endl
             << "Later, start client N-Photo.";
    return true;
}
int main(int argc, char *argv[])
{
    QApplication  app(argc, argv);
    Server server;
    if (argc > 1) {
        QString str(argv[1]);
        bool ok;
        quint16 port = str.toUShort(&ok);
        if (ok) {
            return start(server, ok, port) && app.exec();
        }
        else if (str == QString("--rebuild")) {
            DatabaseWork db;
            if (db.rebuildDB()) {
                qDebug() << "Rebuild of database has completed.";
                return 0;
            }
            qCritical() << "An error occured.";
            return -2;
        }
        else if (str == QString("--export")) {
            DatabaseWork db;
            if (!db.exportDB()) {
                qCritical() << "An error has occured during export.";
                return -2;
            }
            qDebug() << "Export has been completed.";
            return 0;
        }
        else if (str == QString("--add-image") && argc > 2) {
            DatabaseWork db;
            str = QString(argv[2]);
            bool ret = db.addImage(QFileInfo(str).absoluteFilePath());
            qDebug() << "Image added.";
            return (ret)? 0 : -3;
        }
        else {//if (str == QString("--help")) {
            qDebug() << "N-Photo game." << endl
                     << "Usage:" << argv[0] << " [OPTION]" << endl
                     << "OPTION is the some of" << endl
                     << "   NUMBER\t\tset port to listen to and" << endl
                     << "         \t\tstart server without GUI" << endl
                     << "   --rebuild\t\trebuild SQLite database" << endl
                     << "         \t\tfrom \"images\" folder" << endl
                     << "   --export\t\texport files from database" << endl
                     << "         \t\tto \"export\" folder" << endl
                     << "   --add-image FILENAME\tadd image to database" << endl
                     << "   --help\t\tshow this text" << endl;
            return 0;
        }
    }
    return start(server) && app.exec();
}
