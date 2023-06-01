#ifndef DATABASEWORK_H
#define DATABASEWORK_H
#include <QSqlDatabase>
#include <QMutex>
#include <QMutexLocker>
#include <QObject>

class DatabaseWork :QObject
{
     Q_OBJECT
public:
    ~DatabaseWork();
    //work with images
    bool addImage(QString);
    QByteArray Image(bool&, int, QString&);
    QList<QString> listImages(bool &);
    //work with database
    bool fillDB(QString = QString("images"));
    bool cleanDB();
    bool rebuildDB();
    bool exportDB(QString = QString("export"));

    static DatabaseWork* Instance();

protected:
    DatabaseWork();

private:
    bool openConnetion();
    bool createConnection();
    bool closeConnetion();

    QSqlDatabase db;
    QMutex mutex;

    static DatabaseWork* instance;
};

#endif // DATABASEWORK_H
