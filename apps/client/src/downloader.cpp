#include "downloader.h"

Downloader::Downloader(QObject *parent) :
    QObject(parent)
{
    url = QUrl("https://i.pravatar.cc/400");
    connect(&webCtrl, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(ready(QNetworkReply*)));
}
void Downloader::sendRequest()
{
    QNetworkRequest request(url);
    webCtrl.get(request);
}
void Downloader::ready(QNetworkReply* reply)
{
    downloadedData = reply->readAll();
    reply->deleteLater();

    emit downloaded();
}

QByteArray Downloader::getData() const
{
    return downloadedData;
}

