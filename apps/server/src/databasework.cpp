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
#include <QImageWriter>
#include <QFileInfoList>
#include "databasework.h"

DatabaseWork::DatabaseWork()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("ImageDB");
}
DatabaseWork::~DatabaseWork()
{
    closeConnection();
}
bool DatabaseWork::createConnection()
{
    if (!db.open()) {
        qDebug() << "Error:" << db.lastError();
        return false;
    }
    return true;
}
bool DatabaseWork::openConnection()
{
    if (db.isOpen() || db.open())
        return true;

    return createConnection();
}
bool DatabaseWork::closeConnection()
{
    if (db.isOpen())
        db.close();

    return !db.isOpen();
}
bool DatabaseWork::addImage(const QString fileName)
{
    QMutexLocker lock(&mutex);
    if (!openConnection())
        return false;

    QFile file (fileName);
    QFileInfo imageInfo(file);
    bool ret = false;
    if (file.exists()) {
        QImage image(fileName);
        if (!image.isNull()) {
            int size = qMin(image.width(), image.height());
            image = image.copy((image.width() - size)/2,
                         (image.height() - size)/2, size, size)
            .scaled(400, 400, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            QBuffer buffer;
            QImageWriter writer(&buffer, "JPG");
            writer.write(image);

            QSqlQuery query(db);
            query.prepare("INSERT INTO images VALUES (NULL, ?, ?)");
            query.addBindValue(imageInfo.baseName());
            file.open(QIODevice::ReadOnly);
            query.addBindValue(buffer.data());
            file.close();
            ret = query.exec();
        }
    }

    return closeConnection() && ret;
}
QByteArray DatabaseWork::getImage(bool &ok, int index, QString& name)
{
    QMutexLocker lock(&mutex);
    if (!openConnection()) {
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
    ok = closeConnection() && ok;
    return image;
}
QList<QString> DatabaseWork::listImages(bool &ok)
{
    QMutexLocker lock(&mutex);
    if (!openConnection())
        return QList<QString>();

    QList<QString> imageList;
    QSqlQuery query(db);
    ok = query.exec("SELECT name FROM images");
    int i = 0;
    while (query.next() && i++ < MAX)
        imageList.append(query.value(0).toString());

    ok = closeConnection() && ok;
    return imageList;
}
bool DatabaseWork::rebuildDB()
{
    QMutexLocker lock(&mutex);
    if (!openConnection())
        return false;

    db.transaction();
    bool ret = cleanDB() && fillDB();
    if (ret)
        db.commit();
    else
        db.rollback();

    return closeConnection() && ret;
}
bool DatabaseWork::fillDB(const QString folder)
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
    QFileInfoList list = dir.entryInfoList();
    int fileListSize = qMin(list.size(), MAX);
    for (int i = 0; i < fileListSize; i++) {
        QFileInfo df = list.at(i);
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
bool DatabaseWork::exportDB(const QString exportPath)
{
    QMutexLocker lock(&mutex);
    if (!openConnection())
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
    return closeConnection() && ret;
}
bool DatabaseWork::cleanDB()
{
    if (!db.isOpen())
        return false;

    QSqlQuery query(db);
    return query.exec("DROP TABLE IF EXISTS images");
}
QString DatabaseWork::getDBName() const
{
    return db.databaseName();
}
