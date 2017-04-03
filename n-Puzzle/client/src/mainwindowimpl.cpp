/****************************************************************************
 **
 ** Copyright (C) 2009 Digia Plc and/or its subsidiary(-ies).
 ** Contact: http://www.qt-project.org/legal
 **
 ** Copyright (C) 2015 Dmitry Kuznetsov
 **
 ** $QT_BEGIN_LICENSE:BSD$
 ** You may use this file under the terms of the BSD license as follows:
 **
 ** "Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 **   * Redistributions of source code must retain the above copyright
 **     notice, this list of conditions and the following disclaimer.
 **   * Redistributions in binary form must reproduce the above copyright
 **     notice, this list of conditions and the following disclaimer in
 **     the documentation and/or other materials provided with the
 **     distribution.
 **   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
 **     of its contributors may be used to endorse or promote products derived
 **     from this software without specific prior written permission.
 **
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 **
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

#include <QListView>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QPixmap>
#include <QTimer>
#include <QDataStream>
#include <QByteArray>
#include <QBuffer>
#include <QImage>
#include <QImageReader>
#include <QThread>
#include <QTextStream>
#include <QFileInfo>
#include <QDir>
#include <QSettings>
#include "aboutdialogimpl.h"
#include "piecesmodel.h"
#include "puzzlewidget.h"
#include "mainwindowimpl.h"

#define TimeFormat "hh:mm:ss"
//
MainWindowImpl::MainWindowImpl(QWidget * parent, Qt::WindowFlags f)
    : QMainWindow(parent, f)
{
    setupUi(this);
    // чтение настроек
    readSettings();
    QTime now = QTime::currentTime();
    log->append(trUtf8("<b>Добро пожаловать!</b><br>На данный момент <b>%1</b> день года, <b>%2:%3:%4</b>")
                 .arg(QDate::currentDate().dayOfYear())
                 .arg(now.hour()).arg(now.minute()).arg(now.second()));
    //step->setVisible(false);
    // действия меню
    // файл
    connect(aExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(aOpen, SIGNAL(triggered()), this, SLOT(openImage()));
    connect(aSave, SIGNAL(triggered()), this, SLOT(saveImage()));
    // таймер
    connect(aTimerOn, SIGNAL(triggered()), this, SLOT(timerOn()));
    connect(aTimerOff, SIGNAL(triggered()), this, SLOT(timerOff()));
    connect(aTimerReset, SIGNAL(triggered()), this, SLOT(timerReset()));
    // настройки
    connect(aServerSource, SIGNAL(triggered()), this, SLOT(setServerSettings()));
    connect(aLocalSource, SIGNAL(triggered()), this, SLOT(setLocalFolderSettings()));
    connect(aInternetSource, SIGNAL(triggered()), this, SLOT(setInternetSettings()));
    connect(aGameSettings, SIGNAL(triggered()), this, SLOT(setGameSettings()));
    connect(aComplication, SIGNAL(triggered()), this, SLOT(setComplicationSettings()));
    connect(aAll, SIGNAL(triggered()), this, SLOT(setSettingsAll()));
    connect(aLog, SIGNAL(triggered()), this, SLOT(setLogVisible()));
    // язык
    connect(aRussian, SIGNAL(triggered()), this, SLOT(setRussian()));
    connect(aEnglish, SIGNAL(triggered()), this, SLOT(setEnglish()));
    connect(aSpanish, SIGNAL(triggered()), this, SLOT(setSpanish()));
    // помощь
    connect(aAboutProgram, SIGNAL(triggered()), this, SLOT(about()));
    connect(aAboutQt, SIGNAL(triggered()), this, SLOT(aboutQt()));
    /////////
    // кнопки
    connect(scramble, SIGNAL(clicked()), this, SLOT (clickScramble()));
    connect(solve, SIGNAL (clicked()), this, SLOT (clickSolve()));
    connect(checkConnection, SIGNAL (clicked()), this, SLOT(getFileList()));
    //-------------------------------
    // загрузка изображения
    connect(listImage, SIGNAL(currentRowChanged(int)), this, SLOT(getImage(int)));
    connect(download, SIGNAL(clicked()), this, SLOT(getRandomImage()));
    // через сеть
    connect(&socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(tcpError(QAbstractSocket::SocketError)));
    connect(&socket, SIGNAL(readyRead()), this, SLOT(tcpReady()));
    connect(&socket, SIGNAL(connected()), this, SLOT(tcpConnected()));
    connect(&socket, SIGNAL(disconnected()), this, SLOT(tcpDisconnected()));
    //  из локальной папки
    connect(choseFolder, SIGNAL(clicked()), this, SLOT(chooseDirectory()));
    //connect(this, SIGNAL(destroyed()), this, SLOT(show()));
    //connect(this, SIGNAL())
    setupWidgets();

    ptimer = new QTimer(this);
    connect(ptimer, SIGNAL(timeout()), this, SLOT(updateTime()));
    setTimer(true);
    timerOn();
    multi = true;
    curRow = -1;
    downloadedImage = 0;
    qsrand(QCursor::pos().x() ^ QCursor::pos().y());

    getFileList();
}
void MainWindowImpl::writeSettings()
{
    QSettings settings(trUtf8("npuzzle.codeplex.com"), trUtf8("n-Puzzle"));

    settings.setValue(trUtf8("geometry"), geometry());
    settings.setValue(trUtf8("host"), host->text());
    settings.setValue(trUtf8("ip"), ip->text());
    settings.setValue(trUtf8("gameType"), gameType1->isChecked());
    settings.setValue(trUtf8("logVisible"), aLog->isChecked());
    settings.setValue(trUtf8("imageSource"),static_cast<int>(imageSource));
    settings.setValue(trUtf8("localFolder"), localFolder->text());
    settings.setValue(trUtf8("gameTypeSettings"),aGameSettings->isChecked());
    settings.setValue(trUtf8("complicationSettings"),aComplication->isChecked());
    int rBut = 5;
    if (rButton_3->isChecked())
        rBut = 3;
    else if (rButton_4->isChecked())
        rBut = 4;

    settings.setValue(trUtf8("complication"), rBut);
    QString lang("en_US");
    if (aRussian->isChecked())
        lang = QString("ru_RU");
    else if (aSpanish->isChecked())
        lang = QString("es_ES");

    settings.setValue(trUtf8("language"), lang);
}
void MainWindowImpl::readSettings()
{
    QSettings settings(trUtf8("npuzzle.codeplex.com"),trUtf8("n-Puzzle"));

    QRect rect = settings.value(trUtf8("geometry"), QRect(0,0,700,489)).toRect();
    move(rect.topLeft());

    resize(rect.size());

    localFolder->setText(settings.value(trUtf8("localFolder")).toString());

    host->setText(settings.value(trUtf8("host"), trUtf8("localhost")).toString());
    ip->setText(settings.value(trUtf8("ip"), trUtf8("5500")).toString());

    bool gameType = settings.value(trUtf8("gameType"), false).toBool();
    gameType1->setChecked(gameType);

    bool actLog = settings.value(trUtf8("logVisible"), false).toBool();
    aLog->setChecked(actLog);
    int minHeight = qMin(rect.size().height(),(actLog)?600:489);
    setMinimumHeight(minHeight);
    resize(rect.size());
    log->setVisible(actLog);

    imageSource = static_cast<ImageSourceType>(settings.value(trUtf8("imageSource"),Internet).toInt());
    switch(imageSource) {
    case (Net):
        aServerSource->setEnabled(false);
        aServerSource->setChecked(true);
        Source->setVisible(false);
        break;
    case (Local):
        aLocalSource->setEnabled(false);
        aLocalSource->setChecked(true);
        serverAddressBox->setVisible(false);
        break;
    case (Internet):
        aInternetSource->setEnabled(false);
        aInternetSource->setChecked(true);
        Source->setVisible(false);
        serverAddressBox->setVisible(false);
        break;
    }
    bool gameSettingsVisible = settings.value(trUtf8("gameTypeSettings"),true).toBool();
    gameTypeBox->setVisible(gameSettingsVisible);
    aGameSettings->setChecked(gameSettingsVisible);

    bool complicationSettingsVisible = settings.value(trUtf8("complicationSettings"),true).toBool();
    complicationBox->setVisible(complicationSettingsVisible);
    aComplication->setChecked(complicationSettingsVisible);;

    int rBut = settings.value(trUtf8("complication"), 4).toInt();
    if (rBut == 3)
        rButton_3->setChecked(true);
    else if (rBut == 4)
        rButton_4->setChecked(true);
    else if (rBut == 5)
        rButton_5->setChecked(true);
    else {
        rBut = 4;
        rButton_4->setChecked(true);
    }
    setComplication();

    QString lang = settings.value(trUtf8("language"),QString("en_US")).toString();
    if (lang == QString("ru_RU")) {
        aRussian->setEnabled(false);
        aRussian->setChecked(true);
    }
    else if (lang == QString("es_ES")) {
        aSpanish->setEnabled(false);
        aSpanish->setChecked(true);
    }
    else {
        lang = QString("en_US");
        aEnglish->setEnabled(false);
        aEnglish->setChecked(true);
    }
    loadLanguage(lang);
}
void MainWindowImpl::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}
void MainWindowImpl::aboutQt()
{
    QMessageBox::aboutQt(this);
}
void MainWindowImpl::setupWidgets()
{
    piecesList = new QListView(this);
    piecesList->setDragEnabled(true);
    piecesList->setViewMode(QListView::IconMode);
    piecesList->setIconSize(QSize(60, 60));
    piecesList->setGridSize(QSize(80, 80));
    piecesList->setSpacing(10);
    piecesList->setMovement(QListView::Snap);
    piecesList->setAcceptDrops(true);
    piecesList->setDropIndicatorShown(true);

    model = new PiecesModel(this);
    piecesList->setModel(model);
    piecesList->setVisible(false);
    puzzleWidget = new PuzzleWidget(this);
    //puzzleWidget->changeRelation(relation);
    connect(puzzleWidget, SIGNAL(puzzleCompleted()),
            this, SLOT(setCompleted()), Qt::QueuedConnection);
    connect(puzzleWidget, SIGNAL(blockMoved()),
            this, SLOT(incStep()), Qt::QueuedConnection);
    puzzleWidget->addPieces(QPixmap(QString::fromUtf8(":/images/images/example.jpg")));
    horizontalLayout->addWidget(piecesList);
    horizontalLayout->addWidget(puzzleWidget);
}
void MainWindowImpl::incStep()
{
    step->display(++moves);
}
void MainWindowImpl::clickSolve()
{
    setupPuzzle();
}
void MainWindowImpl::updateTime()
{
    QTime curTime = QTime::fromString(timerText->text(), TimeFormat).addSecs(1);
    setTimer(false, curTime);
}
void MainWindowImpl::clickScramble()
{
    if (puzzleWidget) {
        puzzleWidget->scramble();
        scramble->setEnabled(false);
        ptimer->start(1000);
    }
}
void MainWindowImpl::showPuzzleField()
{
    piecesList->setVisible(!multi);
    puzzleWidget->changeType(int(multi));
}
void  MainWindowImpl::setSettingsAll()
{
    bool unChecked = !aAll->isChecked();
    settingsBox->setVisible(unChecked);
    aSource->setEnabled(unChecked);
    aGameSettings->setEnabled(unChecked);
    aComplication->setEnabled(unChecked);
}
void MainWindowImpl::setServerSettings()
{
    bool checked = aServerSource->isChecked();
    aServerSource->setEnabled(!checked);
    aLocalSource->setEnabled(checked);
    aInternetSource->setEnabled(checked);
    serverAddressBox->setVisible(checked);
    Source->setVisible(!checked);
    aLocalSource->setChecked(!checked);
    aInternetSource->setChecked(!checked);
    aAll->setChecked(!checked && !gameTypeBox->isVisible() && !complicationBox->isVisible());
    imageSource = Net;
    setSettingsAll();
    getFileList();
}
void MainWindowImpl::setInternetSettings()
{
    bool checked = aInternetSource->isChecked();
    aInternetSource->setEnabled(!checked);
    aServerSource->setEnabled(checked);
    aLocalSource->setEnabled(checked);
    serverAddressBox->setVisible(!checked);
    Source->setVisible(!checked);
    aLocalSource->setChecked(!checked);
    aServerSource->setChecked(!checked);
    aAll->setChecked(!checked && !gameTypeBox->isVisible() && !complicationBox->isVisible());
    imageSource = Internet;
    setSettingsAll();
    getInternetImage();

}
void MainWindowImpl::setLocalFolderSettings()
{
    bool checked = aLocalSource->isChecked();
    aLocalSource->setEnabled(!checked);
    aServerSource->setEnabled(checked);
    aInternetSource->setEnabled(checked);
    Source->setVisible(checked);
    serverAddressBox->setVisible(!checked);
    aServerSource->setChecked(!checked);
    aInternetSource->setChecked(!checked);
    aAll->setChecked(!checked && !gameTypeBox->isVisible() && !complicationBox->isVisible());
    imageSource = Local;
    setSettingsAll();
    getFileList();
}
void MainWindowImpl::setGameSettings()
{
    bool checked = aGameSettings->isChecked();
    gameTypeBox->setVisible(checked);
    aAll->setChecked(!checked && !serverAddressBox->isVisible() && !complicationBox->isVisible());
    setSettingsAll();
 }
void MainWindowImpl::setComplicationSettings()
{
    bool checked = aComplication->isChecked();
    complicationBox->setVisible(checked);
    aAll->setChecked(!checked && !gameTypeBox->isVisible() && !serverAddressBox->isVisible());
    setSettingsAll();
}
void MainWindowImpl::setLogVisible()
{
    bool ch = aLog->isChecked();
    log->setVisible(ch);
    QSize ss = this->size();
    int height;
    if (ch) {
        height = ss.height() + log->height();
        setMinimumHeight(minimumHeight() + log->height());
    }
    else {
        height = ss.height() - log->height();
        setMinimumHeight(minimumHeight() - log->height());
    }
    resize(ss.width(), height);
}
void MainWindowImpl::openImage(const QString &path)
{
    QString fileName = path;
    if (fileName.isNull())
        fileName = QFileDialog::getOpenFileName(this,
                                                trUtf8("Открыть файл"),
                                                "", trUtf8("Файлы изображений (*.png *.jpg *.bmp)"));

    if (!fileName.isEmpty()) {
        QPixmap newImage;
        if (!newImage.load(fileName)) {
            log->append(trUtf8("Открытие файла : изображение не может быть загружено."));
            return;
        }
        int size = qMin(newImage.width(), newImage.height());
        puzzleImage = newImage.copy((newImage.width() - size)/2,
                                    (newImage.height() - size)/2, size, size)
        .scaled(puzzleWidget->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        setupPuzzle();
    }
}
void MainWindowImpl::setCompleted()
{
    log->append(trUtf8("Головоломка решена за %1 ходов и время %2")
                .arg(step->intValue())
                .arg(timerText->text()));
    QMessageBox::information(this, trUtf8("Головоломка решена."),
                             trUtf8("Поздравляем! Вы решили головоломку успешно.\nНажмите OK для продолжения."),
                             QMessageBox::Ok);
    setupPuzzle();
}
void MainWindowImpl::setupPuzzle()
{
    if (puzzleImage.isNull())
        return;
    puzzleWidget->clear();
    setComplication();
    detectGameType();
    showPuzzleField();
    puzzleWidget->changeRelation(relation);

    if (multi) {
        puzzleWidget->addPieces(puzzleImage);
        setTimer(true);
    }
    else {
        model->addPieces(puzzleImage, relation);
        timerReset();
        if (timerText->isEnabled())
            ptimer->start(1000);
    }
    scramble->setEnabled(multi);
    step->setEnabled(multi);
    moves = 0;
    step->display(0);
}
void MainWindowImpl::saveImage()
{
    if (puzzleImage.isNull())
        return;
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    trUtf8("Сохранить изображение"),
                                                    "", trUtf8("Изображения (*.png *.jpg *.bmp)"));
    puzzleImage.save(fileName);
}
void MainWindowImpl::timerOn()
{
    aTimerOn->setChecked(true);
    aTimerOff->setChecked(false);
    timerText->setEnabled(true);
}
void MainWindowImpl::timerOff()
{
    aTimerOn->setChecked(false);
    aTimerOff->setChecked(true);
    timerText->setEnabled(false);
    setTimer(true);
}
void MainWindowImpl::setTimer(const bool reset, const QTime time)
{
    if (reset) {
        ptimer->stop();
        timerText->setText(trUtf8("00:00:00"));
    }
    else
        timerText->setText(time.toString(TimeFormat));
}
void MainWindowImpl::timerReset()
{
    setTimer(true);
}

void MainWindowImpl::loadLanguage(const QString& rLanguage)
{
    if(currLang != rLanguage)
    {
        currLang = rLanguage;
        QLocale locale = QLocale(currLang);
        QLocale::setDefault(locale);
        //QString languageName = QLocale::languageToString(locale.language());

        // remove the old translator
        qApp->removeTranslator(&translator);
        // load the new translator
        if(translator.load(QString(":/language/client_%1.qm").arg(rLanguage)))
        {
            qApp->installTranslator(&translator);
            retranslateUi(this);
            //log->append(trUtf8("Язык изменён на %1").arg(languageName));
        }
    }
}
void MainWindowImpl::setRussian()
{
    aRussian->setEnabled(false);
    aEnglish->setEnabled(true);
    aEnglish->setChecked(false);
    aSpanish->setEnabled(true);
    aSpanish->setChecked(false);
    loadLanguage(QString("ru_RU"));
}

void MainWindowImpl::setEnglish()
{
    aEnglish->setEnabled(false);
    aRussian->setEnabled(true);
    aRussian->setChecked(false);
    aSpanish->setEnabled(true);
    aSpanish->setChecked(false);
    loadLanguage(QString("en_US"));
}

void MainWindowImpl::setSpanish()
{
    aSpanish->setEnabled(false);
    aRussian->setChecked(false);
    aRussian->setEnabled(true);
    aEnglish->setChecked(false);
    aEnglish->setEnabled(true);
    loadLanguage(QString("es_ES"));
}

void MainWindowImpl::about()
{
    AboutDialogImpl infoDialog;
    infoDialog.exec();
}
void MainWindowImpl::setComplication()
{
    if (rButton_3->isChecked())
        relation = QPoint(3,3);
    else {
        if (rButton_4->isChecked())
            relation = QPoint(4,4);
        else
            relation = QPoint(5,5);
    }
}
void MainWindowImpl::detectGameType()
{
    multi = gameType2->isChecked();
}
void MainWindowImpl::getRandomImage()
{
    log->append(trUtf8("<i>Загрузка произвольного изображения......</i>"));
    if (imageSource == Internet) {
        getInternetImage();
        return;
    }
    int count;
    int newCur;
    if ((count = listImage->count()) > 0) {
        if (count == 1)
            newCur = 0;
        else
            while ((newCur = qrand()%count) == curRow);
        listImage->setCurrentRow(newCur);
    }
    else
        getFileList();
}
QString MainWindowImpl::getCache() {
    if (!QDir(trUtf8("cache")).exists())
        QDir().mkdir(trUtf8("cache"));

    return trUtf8("cache");
}

void MainWindowImpl::getFileList()
{
    log->append(trUtf8("<i>Получение списка изображений......</i>"));
    QString lf = localFolder->text();
    switch (imageSource) {
    case (Net):
        dataSize = 0;
        cacheUsed = false;
        messageType = 8;
        socket.abort();
        socket.connectToHost(host->text(), ip->text().toInt());
        break;
    case (Local):
        if (!lf.trimmed().isEmpty() && QDir().exists(lf)) {
           listImage->clear();
           QStringList dirList = QDir(lf).entryList(QDir::Files);
           listImage->addItems(dirList);
        }
        break;
    }
}
void MainWindowImpl::getInternetImage() {
    if (downloadedImage)
        downloadedImage->~FileDownloader();

    log->append(trUtf8("<i>Загрузка изображения из интернета......</i>"));
    QUrl imageUrl("http://lorempixel.com/400/400/");
    downloadedImage = new FileDownloader(imageUrl, this);

    connect(downloadedImage, SIGNAL(downloaded()), SLOT(loadImage()));
}
void MainWindowImpl::getImage(const int curRow)
{
    if (listImage && curRow != -1) {
        switch(imageSource) {
        case (Net):
            if (cacheUsed) {
                log->append(trUtf8("<i>Загрузка изображения из кэша......</i>"));
                QString file (trUtf8("%1/%2.jpg").arg(getCache()).arg(listImage->item(curRow)->text()));
                QPixmap image = QPixmap(file);
                if (image.isNull())
                    puzzleImage = QPixmap(QString::fromUtf8(":/images/images/example.jpg"));
                else {
                    puzzleImage = image;
                    if (puzzleImage.size() != puzzleWidget->size()) {
                        int size = qMin(puzzleImage.width(), puzzleImage.height());
                        puzzleImage = puzzleImage.copy((puzzleImage.width() - size)/2,
                                                   (puzzleImage.height() - size)/2, size, size)
                        .scaled(puzzleWidget->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                        puzzleImage.save(file);
                    }
                    log->append(trUtf8("<i>Изображение загружено из кэша!</i>"));
                }
                setupPuzzle();
                return;
            }
            log->append(trUtf8("<i>Загрузка изображения с сервера......</i>"));
            dataSize = 0;
            messageType = 4;
            this->curRow = curRow;
            socket.abort();
            socket.connectToHost(host->text(), ip->text().toInt());
            break;
        case (Local):
            QString lf = localFolder->text();
            if (!lf.trimmed().isEmpty() && QDir().exists(lf)) {
                log->append(trUtf8("<i>Загрузка изображения из папки......</i>"));
                QString file (trUtf8("%1/%2").arg(lf).arg(listImage->item(curRow)->text()));
                QPixmap image = QPixmap(file);
                if (image.isNull())
                    puzzleImage = QPixmap(QString::fromUtf8(":/images/images/example.jpg"));
                else {
                    puzzleImage = image;
                    if (puzzleImage.size() != puzzleWidget->size()) {
                        int size = qMin(puzzleImage.width(), puzzleImage.height());
                        puzzleImage = puzzleImage.copy((puzzleImage.width() - size)/2,
                                                   (puzzleImage.height() - size)/2, size, size)
                        .scaled(puzzleWidget->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
                    }
                    log->append(trUtf8("<i>Изображение загружено из папки!</i>"));
                }
                setupPuzzle();
            }
            break;
        }
    }
}
void MainWindowImpl::tcpConnected()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_4);
    out << messageType;
    if (messageType == 4) {
        QString file (trUtf8("%1/%2.jpg").arg(getCache()).arg(listImage->item(curRow)->text()));
        if (QFile(file).exists()) {
            puzzleImage = QPixmap(file);
            setupPuzzle();
            log->append(trUtf8("<i>Изображение загружено из кэша!</i>"));
            return;
        }
        else
            out << curRow + 1;
    }
    socket.write(block);
}
void MainWindowImpl::tcpDisconnected()
{
    socket.close();
}
void MainWindowImpl::tcpReady() {
    QDataStream stream(&socket);
    QString fileName;
    stream.setVersion(QDataStream::Qt_4_4);
    if (dataSize == 0) {
        if (socket.bytesAvailable() < 2*sizeof(quint32))
            return;
        stream >> messageType >> dataSize;
    }

    if (messageType == 4) {   // get image file
        if (dataSize > socket.bytesAvailable())
            return;
        stream >> fileName;
        if (socket.bytesAvailable() < dataSize)
            return;
        QByteArray block = socket.read(dataSize);
        QImage image = QImage::fromData(block);
        if (!image.isNull()) {
            puzzleImage = QPixmap::fromImage(image);
            setupPuzzle();
            log->append(trUtf8("<i>Изображение загружено!</i>"));
            QString file (trUtf8("%1/%2.jpg").arg(getCache()).arg(fileName));
            if (!QFile(file).exists())
                image.save(file);
        }
        else
            log->append(trUtf8("<i>Ошибка при получении изображения!</i>"));
    }
    else if (messageType == 8) { // get directory list
        //fileInfoList.clear();
        listImage->clear();
        int separator;
        if (socket.bytesAvailable() < dataSize)
            return;
        stream >> dataSize;
        while (dataSize--) {
            stream >> fileName >> separator;
            listImage->addItem(fileName);
        }
        listImage->update();
        log->append(trUtf8("<i>Список доступных файлов загружен.</i>"));
    }
    else // incorrect type
        log->append(trUtf8("<i>Неизвестный тип - %1</i>").arg(messageType));
}
void MainWindowImpl::tcpError(QAbstractSocket::SocketError error)
{
   if (error == QAbstractSocket::RemoteHostClosedError)
       return;

   if (imageSource != Net)
       return;

   log->append(trUtf8("Ошибка TCP: %1").arg(socket.errorString()));
   if (error == QAbstractSocket::ConnectionRefusedError)
   {
        log->append(trUtf8("Используется кэш."));
        listImage->clear();
        QDir dir (getCache());
        dir.setFilter(QDir::Files);
        QFileInfoList list = dir.entryInfoList();
        cacheUsed = true;
        for (int i = 0; i < list.size(); i++)
            listImage->addItem(list.at(i).baseName());
   }
}
void MainWindowImpl::chooseDirectory()
{
    QString dir = QFileDialog::getExistingDirectory(this, trUtf8("Выбор папки"),QString(),QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    localFolder->setText(dir);
    getFileList();
}
void MainWindowImpl::loadImage()
{
    QByteArray buf = downloadedImage->downloadedData();
    if (!buf.isNull() && !buf.isEmpty()) {
        puzzleImage.loadFromData(buf);
        if (puzzleImage.size() != puzzleWidget->size()) {
            int size = qMin(puzzleImage.width(), puzzleImage.height());
            puzzleImage = puzzleImage.copy((puzzleImage.width() - size)/2,
                                       (puzzleImage.height() - size)/2, size, size)
            .scaled(puzzleWidget->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        }
        log->append(trUtf8("<i>Изображение загружено из интернета!</i>"));
    }
    else
        puzzleImage = QPixmap(QString::fromUtf8(":/images/images/example.jpg"));

    setupPuzzle();
}
