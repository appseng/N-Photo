#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class Downloader : public QObject
{
    Q_OBJECT
public:
    Downloader(QObject *parent = nullptr);
    QByteArray getData() const;
    void sendRequest();
signals:
    void downloaded();
private slots:
    void ready(QNetworkReply* pReply);
private:
    QUrl url;
    QNetworkAccessManager webCtrl;
    QByteArray downloadedData;
};

#endif // DOWNLOADER_H

