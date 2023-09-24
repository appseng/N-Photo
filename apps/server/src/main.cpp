#include <QCoreApplication>
#include <QFileInfo>

#include "server.h"
#include "databasework.h"

#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
#define ENDL Qt::endl
#else
#define ENDL endl
#endif

const quint16 defaultPort = 5500;

bool startServer (QCoreApplication &app, bool ok = false, quint16 port = defaultPort)
{
    Server server;
    if (server.listen(QHostAddress::Any, (ok) ? port : defaultPort)) {
        qDebug() << "A server started on the port"
                 << server.serverPort() << "." << ENDL
                 << "Later, start a client of N-Photo.";
        return app.exec();
    }
    qCritical() << "A server for N-Photo." << ENDL
                << "It's impossible to launch a server: "
                << server.errorString() << "."  << ENDL;
    return -1;
}
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    if (argc > 1) {
        QString param(argv[1]);
        bool ok;
        quint16 port = param.toUShort(&ok);
        if (ok)
            return startServer(app, ok, port);
        else if (param == QString("--rebuild")) {
            DatabaseWork dw;
            if (dw.rebuildDB()) {
                qDebug() << "Rebuilding of the database"
                         << dw.getDBName() << "has completed.";
                return 0;
            }
            qCritical() << "An error occured.";
            return -2;
        }
        else if (param == QString("--export")) {
            DatabaseWork dw;
            if (!dw.exportDB()) {
                qCritical() << "An error has occured during export.";
                return -2;
            }
            qDebug() << "Exporting has been completed.";
            return 0;
        }
        else if (param == QString("--add-image") && argc > 2) {
            param = QString(argv[2]);
            DatabaseWork dw;
            bool ret = dw.addImage(QFileInfo(param).absoluteFilePath());
            qDebug() << "Image added.";
            return (ret) ? 0 : -3;
        }
        else {//if (str == QString("--help")) {
            qDebug() << "N-Photo game server." << ENDL
                     << "Usage:" << argv[0] << " [OPTION]" << ENDL
                     << "OPTION is one of" << ENDL
                     << "   NUMBER\t\tset port to listen to and" << ENDL
                     << "         \t\tstart server without GUI" << ENDL
                     << "   --rebuild\t\trebuild SQLite database" << ENDL
                     << "         \t\tfrom \"images\" folder" << ENDL
                     << "   --export\t\texport files from database" << ENDL
                     << "         \t\tto \"export\" folder" << ENDL
                     << "   --add-image FILENAME\tadd image to database" << ENDL
                     << "   --help\t\tshow this text" << ENDL;
            return 0;
        }
    }
    return startServer(app);
}
