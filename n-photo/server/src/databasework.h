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
    DatabaseWork(QObject * = nullptr, QString = QString("ImageDB"));
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

private:
    bool openConnetion();
    bool createConnection();
    bool closeConnetion();
    QSqlDatabase db;
    QString dbName;
    QMutex mutex;
};

#endif // DATABASEWORK_H
