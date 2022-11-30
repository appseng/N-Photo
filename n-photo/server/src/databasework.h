#ifndef DATABASEWORK_H
#define DATABASEWORK_H
#include <QSqlDatabase>
#include <QMutex>
#include <QMutexLocker>

class DatabaseWork :QObject
{
public:
    DatabaseWork(QObject * = 0, QString = QString("ImageDB"));
    ~DatabaseWork();
    //work with images
    bool addImage(QString);
    QByteArray Image(bool&, int, QString&);
    QList<QString> listImages(bool &);
    //work with database
    bool fillDB(QString = QString("images"));
    bool cleanDB();
    bool rebuildDB();
    void removeDB();
    bool exportDB(QString = QString("export"));
    QSqlDatabase SqlDatabase();

private:
    bool openConnetion();
    bool createConnection();
    bool closeConnetion();
    QSqlDatabase db;
    QString dbName;
    QMutex mutex;
};

#endif // DATABASEWORK_H
