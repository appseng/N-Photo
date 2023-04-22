#include "Downloader.h"

Downloader::Downloader(QObject *parent) :
    QObject(parent)
{
    url = QUrl("https://i.pravatar.cc/400");
    connect(&webCtrl, SIGNAL(finished(QNetworkReply*)), this, SLOT(fileDownloaded(QNetworkReply*)));
}
void Downloader::sendRequest()
{
    QNetworkRequest request(url);
    webCtrl.get(request);
}
void Downloader::fileDownloaded(QNetworkReply* pReply)
{
    downloadedData = pReply->readAll();
    pReply->deleteLater();

    emit downloaded();
}

QByteArray Downloader::getData() const
{
    return downloadedData;
}

