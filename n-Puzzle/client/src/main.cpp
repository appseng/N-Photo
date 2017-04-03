#include <QApplication>
#include <QTextCodec>

#include "mainwindowimpl.h"
//
int main(int argc, char ** argv)
{
        QApplication app(argc, argv);
        QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

        MainWindowImpl win;
        win.show();

        app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
        return app.exec();
}
