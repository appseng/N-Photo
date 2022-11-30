#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class FileDownloader : public QObject
{
    Q_OBJECT
public:
    explicit FileDownloader(QUrl imageUrl, QObject *parent = nullptr);
    QByteArray getDownloadedData() const;
signals:
    void downloaded();
private slots:
    void fileDownloaded(QNetworkReply* pReply);
private:
    QNetworkAccessManager webCtrl;
    QByteArray downloadedData;
};

#endif // FILEDOWNLOADER_H

