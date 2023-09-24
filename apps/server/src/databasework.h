#ifndef DATABASEWORK_H
#define DATABASEWORK_H
#include <QSqlDatabase>
#include <QMutex>
#include <QMutexLocker>
#include <QObject>

#define MAX 500

class DatabaseWork :QObject
{
     Q_OBJECT
public:
    DatabaseWork();
    ~DatabaseWork();
    //work with images
    bool addImage(const QString);
    QByteArray getImage(bool&, const int, QString&);
    QList<QString> listImages(bool &);
    //work with database
    bool fillDB(const QString = QString("images"));
    bool cleanDB();
    bool rebuildDB();
    bool exportDB(const QString = QString("export"));
    QString getDBName() const;

private:
    bool openConnection();
    bool createConnection();
    bool closeConnection();

    QSqlDatabase db;
    QMutex mutex;
};

#endif // DATABASEWORK_H
