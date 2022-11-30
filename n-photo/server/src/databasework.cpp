#include <QDir>
#include <QMap>
#include <QDebug>
#include <QImage>
#include <QBuffer>
#include <QVariant>
#include <QFileInfo>
#include <QSqlError>
#include <QSqlQuery>
#include <QByteArray>
#include <QMessageBox>
#include <QImageWriter>
#include <QFileInfoList>
#include "databasework.h"

DatabaseWork::DatabaseWork(QObject *parent, QString Name)
        :QObject(parent), dbName(Name)
{
}
DatabaseWork::~DatabaseWork()
{
    QMutexLocker mut(&mutex);
    removeDB();
    //~QObject();
}
QSqlDatabase DatabaseWork::SqlDatabase()
{
    QMutexLocker mut(&mutex);
    if (openConnetion())
        return db;
    return QSqlDatabase();
}
bool DatabaseWork::createConnection()
{
    db.close();
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbName);
    if (!db.open()) {
        qDebug() << db.lastError().text();
        return false;
    }
    return true;
}
bool DatabaseWork::openConnetion()
{
    if (db.isOpen() || db.open())
        return true;
    return createConnection();
}
bool DatabaseWork::closeConnetion()
{
    if (db.isOpen())
        db.close();
    return !db.isOpen();
}
bool DatabaseWork::addImage(QString name)
{
    QMutexLocker mut(&mutex);
    if (!openConnetion())
        return false;

    QFile image (name);
    QFileInfo imageInfo(image);
    bool ret = false;
    if (image.exists()) {
        QImage im(name);
        if (!im.isNull()) {
            int size = qMin(im.width(), im.height());
            im = im.copy((im.width() - size)/2,
                         (im.height() - size)/2, size, size)
            .scaled(400, 400, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            QBuffer buffer;
            QImageWriter writer(&buffer, "JPG");
            writer.write(im);

            QSqlQuery query(db);
            query.prepare("INSERT INTO images VALUES (NULL, ?, ?)");
            query.addBindValue(imageInfo.baseName());
            image.open(QIODevice::ReadOnly);
            query.addBindValue(buffer.data());
            image.close();
            ret = query.exec();
        }
    }

    return (closeConnetion() && ret);
}
QByteArray DatabaseWork::Image(bool &ok, int index, QString& name)
{
    QMutexLocker mut(&mutex);
    if (!openConnetion()) {
        ok = false;
        return QByteArray();
    }

    QSqlQuery query(db);
    query.prepare("SELECT name, imagedata FROM images WHERE id = ?");
    query.addBindValue(index);
    ok = query.exec();
    QByteArray image;
    if (query.next()) {
        name = query.value(0).toString();
        image = query.value(1).toByteArray();
    }
    ok = (closeConnetion() && ok);
    return image;
}
QList<QString> DatabaseWork::listImages(bool &ok)
{
    QMutexLocker mut(&mutex);
    if (!openConnetion())
        return QList<QString>();

    QList<QString> imageList;
    QSqlQuery query(db);
    ok = query.exec("SELECT name FROM images");
    while (query.next())
        imageList.append(query.value(0).toString());
    ok = closeConnetion() && ok;
    return imageList;
}
bool DatabaseWork::rebuildDB()
{
    QMutexLocker mut(&mutex);
    if (!openConnetion())
        return false;

    db.transaction();
    bool ret = cleanDB() && fillDB();
    if (ret)
        db.commit();
    else
        db.rollback();

    return (closeConnetion() && ret);
}
bool DatabaseWork::fillDB(QString folder)
{
    if (!db.isOpen())
        return false;

    QSqlQuery query(db);
    bool ret = query.exec("CREATE TABLE IF NOT EXISTS images ("
                          "id INTEGER PRIMARY KEY ASC,"
                          "name TEXT,"
                          "imagedata BLOB)");

    QDir dir (folder);
    dir.setFilter(QDir::Files);
    QFileInfoList fil = dir.entryInfoList();
    int fileListSize = fil.size();
    for (int i = 0; i < fileListSize; i++) {
        QFileInfo df = fil.at(i);
        QImage image = QImage(df.absoluteFilePath());
        if (image.isNull())
            continue;
        int size = qMin(image.width(), image.height());
        image = image.copy((image.width() - size)/2,
                           (image.height() - size)/2, size, size)
        .scaled(400, 400, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        QBuffer buffer;
        QImageWriter writer(&buffer, "JPG");
        writer.write(image);

        query.prepare("INSERT INTO images VALUES (NULL, ?, ?)");
        query.addBindValue(df.baseName());
        query.addBindValue(buffer.data());
        ret = query.exec() && ret;
    }
    return ret;
}
void DatabaseWork::removeDB()
{
    db.close();
    db.removeDatabase(db.connectionName());
}
bool DatabaseWork::exportDB(QString exportPath)
{
    QMutexLocker mut(&mutex);
    if (!openConnetion())
        return false;
    QDir exp(exportPath);
    if (!exp.exists())
        QDir().mkdir(exportPath);
    QDir::setCurrent(exportPath);
    QFile file;
    QSqlQuery query(db);
    bool ret = query.exec("SELECT name, imagedata FROM images");
    while (query.next()){
        file.setFileName(QString("%1.jpg").arg(query.value(0).toString()));
        file.open(QIODevice::WriteOnly);
        file.write(query.value(1).toByteArray());
        file.close();
    }
    return (closeConnetion() && ret);
}
bool DatabaseWork::cleanDB()
{
    if (!db.isOpen())
        return false;

    QSqlQuery query(db);
    return query.exec("DROP TABLE IF EXISTS images");
}
