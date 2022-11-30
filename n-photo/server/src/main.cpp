#include <QApplication>
#include <QTextCodec>
#include <QFileInfo>
#include <QMessageBox>
#include "dialog.h"
#include "databasework.h"
//#include "view.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    if (argc > 1) {
        QString str(argv[1]);
        bool ok;
        quint16 port = str.toUShort(&ok);
        if (ok || (str == QString("--start"))) {
            Server server;
            if (!server.listen(QHostAddress::Any, (ok) ? port : 5500)) {
                qCritical() << QApplication::tr("A multi-threaded server n-Puzzle.\n"
                                                "It's impossible to launch server: %1.")
                        .arg(server.errorString());
                QApplication::beep();
                return -1;
            }
            qDebug() << QApplication::tr("A server started on %1.\n"
                                         "Later, start client n-Puzzle.")
                    .arg(server.serverPort());
            return app.exec();
        }
        else if (str == QString("--rebuild")) {
            DatabaseWork db;
            if (db.rebuildDB()) {
                qDebug() << QApplication::tr("Rebuild of database has complited.");
                return 0;
            }
            qCritical() << QApplication::tr("An error occured.");
            return -2;
        }
        else if (str == QString("--export")) {
            DatabaseWork db;
            if (!db.exportDB()) {
                qCritical() << QApplication::tr("An error has occured during expor.t");
                return -2;
            }
            qDebug() << QApplication::tr("Export has been complited.");
            return 0;
        }
        else if (str == QString("--add-image") && argc > 2) {
            DatabaseWork db;
            str = QString(argv[2]);
            bool ret = db.addImage(QFileInfo(str).absoluteFilePath());
            return (ret)? 0 : -3;
        }
        else {//if (str == QString("--help")) {
            QMessageBox::information(0,QString("n-Puzzle game."),
                      QString("Usage: %1 [OPTION]\n"
                              "OPTION is the some of\n"
                              "   --start\t\tstart server without GUI\n"
                              "   NUMBER\t\tset port to listen to and\n"
                              "         \t\tstart server without GUI\n"
                              "   --rebuild\t\trebuild SQLite database\n"
                              "         \t\tfrom \"images\" folder\n"
                              "   --export\t\texport files from database\n"
                              "         \t\tto \"export\" folder\n"
                              "   --add-image FILENAME\tadd image to database\n"
                              "   --help\t\tshow this text").arg(argv[0]));
            return 0;
        }
        //else
        //    qDebug() << QApplication::tr("A parameter is incorrect.");
    }
    Dialog dialog;
    return dialog.exec();
}
