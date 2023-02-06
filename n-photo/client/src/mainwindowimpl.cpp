#include <QListView>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QPixmap>
#include <QPoint>
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
    setupWidgets();

    readSettings();

    QTime now = QTime::currentTime();
    log->append(trUtf8("<b>Добро пожаловать!</b><br>На данный момент <b>%1</b> день года, <b>%2:%3:%4</b>")
                 .arg(QDate::currentDate().dayOfYear())
                 .arg(now.hour()).arg(now.minute()).arg(now.second()));
    //step->setVisible(false);
    // действия меню
    // file
    connect(aExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(aOpen, SIGNAL(triggered()), this, SLOT(openImage()));
    connect(aSave, SIGNAL(triggered()), this, SLOT(saveImage()));
    // timer
    connect(aTimerOn, SIGNAL(triggered()), this, SLOT(timerOn()));
    connect(aTimerOff, SIGNAL(triggered()), this, SLOT(timerOff()));
    connect(aTimerReset, SIGNAL(triggered()), this, SLOT(timerReset()));
    // settings
    connect(aServerSource, SIGNAL(triggered()), this, SLOT(setServerSettings()));
    connect(aLocalSource, SIGNAL(triggered()), this, SLOT(setLocalFolderSettings()));
    connect(aInternetSource, SIGNAL(triggered()), this, SLOT(setInternetSettings()));
    connect(aGameSettings, SIGNAL(triggered()), this, SLOT(setGameSettings()));
    connect(aComplication, SIGNAL(triggered()), this, SLOT(setComplicationSettings()));
    connect(aAll, SIGNAL(triggered()), this, SLOT(setSettingsAll()));
    connect(aLog, SIGNAL(triggered()), this, SLOT(setLogVisible()));
    // lanuguage
    connect(aRussian, SIGNAL(triggered()), this, SLOT(setRussian()));
    connect(aEnglish, SIGNAL(triggered()), this, SLOT(setEnglish()));
    connect(aSpanish, SIGNAL(triggered()), this, SLOT(setSpanish()));
    // help
    connect(aAboutProgram, SIGNAL(triggered()), this, SLOT(about()));
    connect(aAboutQt, SIGNAL(triggered()), this, SLOT(aboutQt()));
    // solution
    connect(aSolvePuzzle, SIGNAL(triggered()), this, SLOT(solvePuzzle()));
    connect(aMisplacedTiles, SIGNAL(triggered()), this, SLOT(setMisplacedTiles()));
    connect(aManhattanDistance, SIGNAL(triggered()), this, SLOT(setManhattanDistance()));
    // solution=>moves
    connect(aLeft, SIGNAL(triggered()), this, SLOT(moveMissingRectangleLeft()));
    connect(aUp, SIGNAL(triggered()), this, SLOT(moveMissingRectangleUp()));
    connect(aDown, SIGNAL(triggered()), this, SLOT(moveMissingRectangleDown()));
    connect(aRight, SIGNAL(triggered()), this, SLOT(moveMissingRectangleRight()));
    /////////
    // кнопки
    connect(shuffle, SIGNAL(clicked()), this, SLOT (clickShuffle()));
    connect(refresh, SIGNAL (clicked()), this, SLOT (clickRefresh()));
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
    //

    ptimer = new QTimer(this);
    connect(ptimer, SIGNAL(timeout()), this, SLOT(updateTime()));
    setTimer(true);
    timerOn();
    multi = true;
    curRow = -1;
    downloadedImage = nullptr;
    imageIndex = -1;

    qsrand(uint(QCursor::pos().x() * QCursor::pos().y()));

    getFileList();

    // default heuristic is ManhattanDistance
    heuristic = ManhattanDistance;
    aManhattanDistance->setChecked(true);
    setBusy(false);
    strategy = nullptr;
    thread = nullptr;

    solutionTimer = new QTimer(this);
    solutionTimer->setInterval(1200);
    connect(solutionTimer, SIGNAL(timeout()), this, SLOT(updateState()));

    dataSize = 0;
}
MainWindowImpl::~MainWindowImpl()
{
    if (strategy != nullptr) {
        strategy->deleteLater();
    }
}
void MainWindowImpl::writeSettings()
{
    QSettings settings(trUtf8("github.com/appseng/n-photo"), trUtf8("n-photo"));

    settings.setValue(trUtf8("geometry"), geometry());
    settings.setValue(trUtf8("host"), host->text());
    settings.setValue(trUtf8("ip"), ip->text());
    settings.setValue(trUtf8("gameType"), gameType1->isChecked());
    settings.setValue(trUtf8("logVisible"), aLog->isChecked());
    settings.setValue(trUtf8("imageSource"),static_cast<int>(imageSource));
    settings.setValue(trUtf8("localFolder"), localFolder->text());
    settings.setValue(trUtf8("gameTypeSettings"),aGameSettings->isChecked());
    settings.setValue(trUtf8("complicationSettings"),aComplication->isChecked());
    int rBut = 4;
    if (rButton_3->isChecked())
        rBut = 3;

    settings.setValue(trUtf8("complication"), rBut);
    QString lang("en_US");
    if (aRussian->isChecked())
        lang = QString("ru_RU");
    else if (aSpanish->isChecked())
        lang = QString("es_ES");

    settings.setValue(trUtf8("language"), lang);

    settings.setValue(trUtf8("heuristic"), aManhattanDistance->isChecked());
}
void MainWindowImpl::readSettings()
{
    QSettings settings(trUtf8("github.com/appseng/n-photo"),trUtf8("n-photo"));

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
    if (rBut == 3) {
        rButton_3->setChecked(true);
    } else {
        rButton_4->setChecked(true);
    }
    setComplication();

    QString lang = settings.value(trUtf8("language"),QString("en_US")).toString();
    if (lang == QString("ru_RU")) {
        aRussian->setEnabled(false);
        aRussian->setChecked(true);
    } else if (lang == QString("es_ES")) {
        aSpanish->setEnabled(false);
        aSpanish->setChecked(true);
    } else {
        lang = QString("en_US");
        aEnglish->setEnabled(false);
        aEnglish->setChecked(true);
    }
    loadLanguage(lang);

    bool heuristic = settings.value(trUtf8("heuristic"), true).toBool();
    if (heuristic) {
        aManhattanDistance->setChecked(true);
    } else {
        aMisplacedTiles->setChecked(true);
    }
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
    connect(puzzleWidget, SIGNAL(puzzleCompleted(bool)),
            this, SLOT(setCompleted(bool)), Qt::QueuedConnection);
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
void MainWindowImpl::clickRefresh()
{
    setupPuzzle();
}
void MainWindowImpl::updateTime()
{
    QTime curTime = QTime::fromString(timerText->text(), TimeFormat).addSecs(1);
    setTimer(false, curTime);
}
void MainWindowImpl::clickShuffle()
{
    if (puzzleWidget) {
        puzzleWidget->shuffle();
        shuffle->setEnabled(false);
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
    getFileList();
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
    } else {
        height = ss.height() - log->height();
        setMinimumHeight(minimumHeight() - log->height());
    }
    resize(ss.width(), height);
}
void MainWindowImpl::openImage(const QString &path)
{
    if (busy)
        return;

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
void MainWindowImpl::setCompleted(bool byHuman)
{
    log->append(trUtf8("Головоломка решена за %1 ходов и время %2")
                .arg(step->intValue())
                .arg(timerText->text()));
    if (byHuman) {
        QMessageBox::information(this, trUtf8("Головоломка решена."),
                                 trUtf8("Поздравляем! Вы решили головоломку успешно.\nНажмите OK для продолжения."),
                                 QMessageBox::Ok);
    }
    setupPuzzle();
}
void MainWindowImpl::setupPuzzle()
{
    if (busy)
        return;

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
    } else {
        model->addPieces(puzzleImage, relation);
        timerReset();
        if (timerText->isEnabled())
            ptimer->start(1000);
    }
    shuffle->setEnabled(multi);
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
    } else
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
void MainWindowImpl::solvePuzzle()
{
    if (!multi)
        return;

    if (busy == false) {
        nodes.clear();
        for(int j = 0; j < relation.y(); j++) {
            for (int i = 0; i < relation.x(); i++){
                QPoint point(i,j);
                int index = puzzleWidget->getTargetIndex(point);
                if (index != -1)
                    index++;

                nodes.append(char(index));
            }
        }
           setBusy(true);

           if (thread != nullptr) {
               thread->deleteLater();
           }
           thread = new SolveThread(this, &nodes, heuristic);
           thread->start();

           log->append(trUtf8("Поиск решения начался..."));
    }
}
void MainWindowImpl::startTimer(QStack<State*>* path)
{
    this->path = path;
    solutionTimer->start();
}

void MainWindowImpl::updateState()
{
    if (path->count() > 0) {
        const QVector<char>* nodes = path->pop()->getState();
        Param *param = new Param(this, nodes, path->count() == 0);
        displayState(param);
    } else {
        solutionTimer->stop();
        setBusy(false);
    }
}

void MainWindowImpl::setMisplacedTiles()
{
    aMisplacedTiles->setChecked(true);
    aManhattanDistance->setChecked(false);
    heuristic = MisplacedTiles;
}
void MainWindowImpl::setManhattanDistance()
{
    aMisplacedTiles->setChecked(false);
    aManhattanDistance->setChecked(true);
    heuristic = ManhattanDistance;

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
    if (busy)
        return;

    log->append(trUtf8("<i>Загрузка произвольного изображения......</i>"));
    if (imageSource == Internet) {
        getInternetImage();
        return;
    }
    int count = listImage->count();
    int newCur;
    if (count > 0) {
        if (count == 1)
            newCur = 0;
        else
            while ((newCur = qrand()%count) == curRow);
        listImage->setCurrentRow(newCur);
    } else
        getFileList();
}
void MainWindowImpl::tcpConnect(MessageType type)
{
    messageType = type;
    dataSize = 0;
    socket.abort();
    socket.connectToHost(host->text(), ip->text().toUShort());
}
void MainWindowImpl::getFileList()
{
    if (busy)
        return;

    log->append(trUtf8("<i>Получение списка изображений......</i>"));

    switch (imageSource) {
        case (Net):
            tcpConnect(List);
            break;
        case (Local): {
            QString lf = localFolder->text();
            if (!lf.trimmed().isEmpty() && QDir().exists(lf)) {
               listImage->clear();
               QStringList dirList = QDir(lf).entryList(QDir::Files);
               listImage->addItems(dirList);
            }
            break;
        }
        case (Internet):
            getInternetImage();
            break;
    }
}
void MainWindowImpl::getInternetImage() {
    if (downloadedImage != nullptr)
        downloadedImage->deleteLater();

    log->append(trUtf8("<i>Загрузка изображения из интернета......</i>"));

    QUrl imageUrl(trUtf8("https://i.pravatar.cc/400"));
    downloadedImage = new FileDownloader(imageUrl, this);

    connect(downloadedImage, SIGNAL(downloaded()), SLOT(loadImage()));
}
void MainWindowImpl::getImage(const int curRow)
{
    if (busy)
        return;

    if (listImage && curRow != -1) {
        switch(imageSource) {
            case (Net):
                log->append(trUtf8("<i>Загрузка изображения с сервера......</i>"));
                this->curRow = curRow;

                tcpConnect(File);
                break;
            case (Local): {
                QString lf = localFolder->text();
                if (!lf.trimmed().isEmpty() && QDir().exists(lf)) {
                    log->append(trUtf8("<i>Загрузка изображения из папки......</i>"));
                    QString file (trUtf8("%1/%2").arg(lf).arg(listImage->currentItem()->text()));
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
            default:
                break;
        }
    }
}
void MainWindowImpl::tcpConnected()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::ReadWrite);
    out << messageType;

    if (messageType == File) {
        out << listImage->currentRow();
    }
    else
        out << int(0);

    socket.write(block);
}
void MainWindowImpl::tcpDisconnected()
{
    socket.close();
}
void MainWindowImpl::tcpReady()
{
    if (socket.bytesAvailable() < 2*sizeof(int)) return;

    QDataStream stream(&socket);
    if (dataSize == 0)
        stream >> messageType >> dataSize;

    if (messageType == File) {   // get image file
        if (socket.bytesAvailable() < dataSize)
            return;

        QImage image = QImage::fromData(socket.read(dataSize));
        if (!image.isNull()) {
            puzzleImage = QPixmap::fromImage(image);
            setupPuzzle();
            log->append(trUtf8("<i>Изображение загружено!</i>"));
        }
        else
            log->append(trUtf8("<i>Ошибка при получении изображения!</i>"));

        dataSize = 0;
    }
    else if (messageType == List) { // get directory list
        listImage->clear();

        QString name;
        int separator;
        while (dataSize > 0) {
            stream >> name >> separator;
            listImage->addItem(name);
            dataSize--;
        }
        listImage->update();
        log->append(trUtf8("<i>Список доступных файлов загружен.</i>"));
        dataSize = 0;
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
}
void MainWindowImpl::chooseDirectory()
{
    QString dir = QFileDialog::getExistingDirectory(this, trUtf8("Выбор папки"),localFolder->text(),QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dir.length() == 0)
        dir = localFolder->text();

    localFolder->setText(dir);
    getFileList();
}
void MainWindowImpl::loadImage()
{
    QByteArray buf = downloadedImage->getDownloadedData();
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
void MainWindowImpl::displayState(Param *param)
{
    const QVector<char>* nodes = param->getState();
    bool isFinal = param->isFinalState();
    if (isFinal)
        setBusy(false);

    if (nodes != nullptr)
        puzzleWidget->setPieces(nodes);
}
void MainWindowImpl::onPuzzleSolved(Param* param) {
    int steps = param->getSteps();
    int states = param->getStates();
    log->append(trUtf8("Решение найдено: %1 шагов, %2 состояний").arg(steps).arg(states));
}
void MainWindowImpl::moveMissingRectangleLeft()
{
    puzzleWidget->moveMissingRectangle(Left);
}
void MainWindowImpl::moveMissingRectangleUp()
{
    puzzleWidget->moveMissingRectangle(Up);
}
void MainWindowImpl::moveMissingRectangleDown()
{
    puzzleWidget->moveMissingRectangle(Down);
}
void MainWindowImpl::moveMissingRectangleRight()
{
    puzzleWidget->moveMissingRectangle(Right);
}
void MainWindowImpl::setBusy(bool busy)
{
    this->busy = busy;
    puzzleWidget->setBusy(busy);
}
