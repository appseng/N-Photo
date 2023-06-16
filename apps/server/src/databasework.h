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
    bool addImage(const QString);
    QByteArray getImage(bool&, const int, QString&);
    QList<QString> listImages(bool &);
    //work with database
    bool fillDB(const QString = QString("images"));
    bool cleanDB();
    bool rebuildDB();
    bool exportDB(const QString = QString("export"));
    QString getDBName() const;
    static DatabaseWork* getInstance();

protected:
    DatabaseWork();

private:
    bool openConnection();
    bool createConnection();
    bool closeConnection();

    QSqlDatabase db;
    QMutex mutex;

    static DatabaseWork* instance;
};

#endif // DATABASEWORK_H
