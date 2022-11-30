#include "filedownloader.h"

FileDownloader::FileDownloader(QUrl imageUrl, QObject *parent) :
    QObject(parent)
{
    connect(&webCtrl, SIGNAL(finished(QNetworkReply*)),
                SLOT(fileDownloaded(QNetworkReply*)));

    QNetworkRequest request(imageUrl);
    webCtrl.get(request);
}

void FileDownloader::fileDownloaded(QNetworkReply* pReply)
{
    downloadedData = pReply->readAll();
    pReply->deleteLater();

    emit downloaded();
}

QByteArray FileDownloader::getDownloadedData() const
{
    return downloadedData;
}

