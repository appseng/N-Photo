#ifndef DATABASEWORK_H
#define DATABASEWORK_H
#include <QSqlDatabase>
#include <QMutex>
#include <QMutexLocker>
#include <QObject>

#define MAX_LIST 500

class DatabaseWork :QObject
{
     Q_OBJECT
public:
    DatabaseWork();
    ~DatabaseWork();
    //work with images
    bool addImage(const QString);
    QByteArray getImage(bool&, const int, QString&);
    QList<QString> imageList(bool &);
    //work with database
    bool rebuild();
    bool exportIn(const QString = QString("export"));
    QString getDatabaseName() const;

private:
    bool openConnection();
    bool createConnection();
    bool closeConnection();

    bool fill(const QString = QString("images"));
    bool clean();

    QSqlDatabase db;
    QMutex mutex;
};

#endif // DATABASEWORK_H
