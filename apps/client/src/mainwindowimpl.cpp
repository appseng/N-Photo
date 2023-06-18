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
    log->append(tr("<b>Добро пожаловать!</b><br>На данный момент <b>%1</b> день года, <b>%2:%3:%4</b>")
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
    connect(&downloader, SIGNAL(downloaded()), this, SLOT(loadImage()));
    // через сеть
    connect(&socket, SIGNAL(sendImage(int)), this, SLOT(getSocketImage(int)));
    connect(&socket, SIGNAL(sendError()), this, SLOT(socketError()));
    connect(&socket, SIGNAL(sendImageList(QList<QString>)), this, SLOT(socketList(QList<QString>)));
    connect(&socket, SIGNAL(incorrectType(MessageType)), this, SLOT(socketIncorrectType(MessageType)));

    //  из локальной папки
    connect(choseFolder, SIGNAL(clicked()), this, SLOT(chooseDirectory()));
    //

    guiTimer = new QTimer(this);
    connect(guiTimer, SIGNAL(timeout()), this, SLOT(updateTime()));

    timerOn();
    gameType = NPhoto;
    curRow = -1;

    rand.seed(QCursor::pos().x() * QCursor::pos().y());

    // default heuristic is ManhattanDistance
    heuristic = ManhattanDistance;
    aManhattanDistance->setChecked(true);
    setBusy(false);
    strategy = nullptr;
    thread = nullptr;

    solutionTimer = new QTimer(this);
    solutionTimer->setInterval(1200);
    connect(solutionTimer, SIGNAL(timeout()), this, SLOT(updateState()));

    getFileList();
}
MainWindowImpl::~MainWindowImpl()
{
    if (strategy != nullptr) {
        strategy->deleteLater();
    }
}
void MainWindowImpl::writeSettings()
{
    QSettings settings(tr("github.com/appseng/n-photo"), tr("n-photo"));

    settings.setValue(tr("geometry"), geometry());
    settings.setValue(tr("host"), host->text());
    settings.setValue(tr("ip"), ip->text());
    settings.setValue(tr("gameType"), gameType1->isChecked());
    settings.setValue(tr("logVisible"), aLog->isChecked());
    settings.setValue(tr("imageSource"),static_cast<int>(imageSource));
    settings.setValue(tr("localFolder"), localFolder->text());
    settings.setValue(tr("gameTypeSettings"),aGameSettings->isChecked());
    settings.setValue(tr("complicationSettings"),aComplication->isChecked());
    int rBut = 4;
    if (rButton_3->isChecked())
        rBut = 3;

    settings.setValue(tr("complication"), rBut);
    QString lang("en_US");
    if (aRussian->isChecked())
        lang = QString("ru_RU");
    else if (aSpanish->isChecked())
        lang = QString("es_ES");

    settings.setValue(tr("language"), lang);

    settings.setValue(tr("heuristic"), aManhattanDistance->isChecked());
}
void MainWindowImpl::readSettings()
{
    QSettings settings(tr("github.com/appseng/n-photo"),tr("n-photo"));

    QRect rect = settings.value(tr("geometry"), QRect(0,0,700,489)).toRect();
    move(rect.topLeft());

    resize(rect.size());

    localFolder->setText(settings.value(tr("localFolder")).toString());

    host->setText(settings.value(tr("host"), tr("localhost")).toString());
    ip->setText(settings.value(tr("ip"), tr("5500")).toString());

    bool gameType = settings.value(tr("gameType"), false).toBool();
    gameType1->setChecked(gameType);

    bool actLog = settings.value(tr("logVisible"), false).toBool();
    aLog->setChecked(actLog);
    int minHeight = qMin(rect.size().height(),(actLog)?600:489);
    setMinimumHeight(minHeight);
    resize(rect.size());
    log->setVisible(actLog);

    imageSource = static_cast<ImageSourceType>(settings.value(tr("imageSource"),Internet).toInt());
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
    bool gameSettingsVisible = settings.value(tr("gameTypeSettings"),true).toBool();
    gameTypeBox->setVisible(gameSettingsVisible);
    aGameSettings->setChecked(gameSettingsVisible);

    bool complicationSettingsVisible = settings.value(tr("complicationSettings"),true).toBool();
    complicationBox->setVisible(complicationSettingsVisible);
    aComplication->setChecked(complicationSettingsVisible);;

    int rBut = settings.value(tr("complication"), 4).toInt();
    if (rBut == 3) {
        rButton_3->setChecked(true);
    } else {
        rButton_4->setChecked(true);
    }
    setComplication();

    QString lang = settings.value(tr("language"),QString("en_US")).toString();
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

    bool heuristic = settings.value(tr("heuristic"), true).toBool();
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
    if (!timerText->isEnabled()) return;

    QTime spendTime = QTime::fromString(timerText->text(), TimeFormat).addSecs(1);
    setTimer(false, spendTime);
}
void MainWindowImpl::clickShuffle()
{
    if (puzzleWidget) {
        puzzleWidget->shuffle();
        shuffle->setEnabled(false);
        timerRestart();
    }
}
void MainWindowImpl::showPuzzleField()
{
    piecesList->setVisible(!gameType);
    puzzleWidget->changeType(gameType);
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
    if (isBusy())
        return;

    QString fileName = path;
    if (fileName.isNull())
        fileName = QFileDialog::getOpenFileName(this,
                                                tr("Открыть файл"),
                                                "", tr("Файлы изображений (*.png *.jpg *.bmp)"));

    if (!fileName.isEmpty()) {
        QPixmap newImage;
        if (!newImage.load(fileName)) {
            log->append(tr("Открытие файла : изображение не может быть загружено."));
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
    log->append(tr("Головоломка решена за %1 ходов и время %2")
                .arg(step->intValue())
                .arg(timerText->text()));
    if (byHuman) {
        QMessageBox::information(this, tr("Головоломка решена."),
                                 tr("Поздравляем! Вы решили головоломку успешно.\nНажмите OK для продолжения."),
                                 QMessageBox::Ok);
    }
    setupPuzzle();
}
void MainWindowImpl::timerRestart()
{
    if (!timerText->isEnabled()) return;

    timerReset();
    guiTimer->start(1000);
}

void MainWindowImpl::setupPuzzle()
{
    if (isBusy())
        return;

    if (puzzleImage.isNull())
        return;

    puzzleWidget->clear();
    setComplication();
    detectGameType();
    showPuzzleField();
    puzzleWidget->changeRelation(relation);

    if (gameType) {
        puzzleWidget->addPieces(puzzleImage);
        guiTimer->stop();
    }
    else {
        model->addPieces(puzzleImage, relation);
        timerRestart();
    }

    shuffle->setEnabled(gameType);
    step->setEnabled(gameType);
    moves = 0;
    step->display(0);
}
void MainWindowImpl::saveImage()
{
    if (isBusy())
        return;

    if (puzzleImage.isNull())
        return;

    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Сохранить изображение"),
                                                    "", tr("Изображения (*.png *.jpg *.bmp)"));
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
        guiTimer->stop();
        timerText->setText(tr("00:00:00"));
    } else
        timerText->setText(time.toString(TimeFormat));
}
void MainWindowImpl::timerReset()
{
    setTimer(true);
}

void MainWindowImpl::loadLanguage(const QString& language)
{
    if (currentLanguage != language)
    {
        currentLanguage = language;
        QLocale locale = QLocale(currentLanguage);
        QLocale::setDefault(locale);

        qApp->removeTranslator(&translator);
        if (translator.load(QString(":/language/client_%1.qm").arg(language)))
        {
            qApp->installTranslator(&translator);
            retranslateUi(this);
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
    if (!gameType)
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

           log->append(tr("Поиск решения начался..."));
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
        const QVector<char>* nodes = path->pop()->getNodes();
        StateParam *param = new StateParam(this, nodes, path->count() == 0);
        displayState(param);
    } else {
        solutionTimer->stop();
        setBusy(false);

        path->clear();
        path->detach();
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
    else
        relation = QPoint(4,4);
}
void MainWindowImpl::detectGameType()
{
    gameType = GameType(gameType2->isChecked());
}
void MainWindowImpl::getRandomImage()
{
    if (isBusy())
        return;

    log->append(tr("<i>Загрузка произвольного изображения......</i>"));
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
            while ((newCur = rand.bounded(count)) == curRow);
        listImage->setCurrentRow(newCur);
    } else
        getFileList();
}
void MainWindowImpl::ConnectToServer(MessageType type)
{
    socket.clientConnect(type, host->text(), ip->text().toUShort(), listImage->currentRow());
}
void MainWindowImpl::getFileList()
{
    if (isBusy())
        return;

    log->append(tr("<i>Получение списка изображений......</i>"));

    if (imageSource == Net) {
        ConnectToServer(List);
    }
    else if (imageSource == Local) {
        QString lf = localFolder->text();
        if (!lf.trimmed().isEmpty() && QDir().exists(lf)) {
            listImage->clear();
            QStringList dirList = QDir(lf).entryList(QDir::Files);
            listImage->addItems(dirList);
        }
    }
    else if (imageSource == Internet) {
        listImage->clear();
        imagesCache.clear();

        getInternetImage();
    }
}
void MainWindowImpl::getInternetImage() {
    log->append(tr("<i>Загрузка изображения из интернета......</i>"));

    downloader.sendRequest();
}
void MainWindowImpl::getImage(const int curRow)
{
    if (isBusy())
        return;

    if (listImage && curRow != -1) {
        if (imageSource == Net) {
            this->curRow = curRow;

            if (imagesCache[curRow] != QPixmap()){
                puzzleImage = imagesCache[curRow];
                setupPuzzle();
                return;
            }
            log->append(tr("<i>Загрузка изображения с сервера......</i>"));

            ConnectToServer(File);
        }
        else if (imageSource == Local) {
            QString lf = localFolder->text();
            if (!lf.trimmed().isEmpty() && QDir().exists(lf)) {
                log->append(tr("<i>Загрузка изображения из папки......</i>"));
                QString file (tr("%1/%2").arg(lf).arg(listImage->currentItem()->text()));
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
                    log->append(tr("<i>Изображение загружено из папки!</i>"));
                }
                setupPuzzle();
            }
        }
        else if (imageSource == Internet) {
            puzzleImage = imagesCache[listImage->currentRow()];
            setupPuzzle();
        }
    }
}

void MainWindowImpl::chooseDirectory()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Выбор папки"),localFolder->text(),QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dir.length() == 0)
        dir = localFolder->text();

    localFolder->setText(dir);
    getFileList();
}
void MainWindowImpl::loadImage()
{
    QByteArray buf = downloader.getData();
    if (!buf.isNull() && !buf.isEmpty()) {
        puzzleImage.loadFromData(buf);
        if (puzzleImage.size() != puzzleWidget->size()) {
            int size = qMin(puzzleImage.width(), puzzleImage.height());
            puzzleImage = puzzleImage.copy((puzzleImage.width() - size)/2,
                                       (puzzleImage.height() - size)/2, size, size)
            .scaled(puzzleWidget->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        }        
        imagesCache.append(puzzleImage);
        listImage->addItem(tr("Картинка").append(" #%1").arg(imagesCache.count()));

        log->append(tr("<i>Изображение загружено из интернета!</i>"));
    }
    else
        puzzleImage = QPixmap(QString::fromUtf8(":/images/images/example.jpg"));

    setupPuzzle();
}
void MainWindowImpl::displayState(StateParam *param)
{
    const QVector<char>* nodes = param->getState();
    bool isFinal = param->isFinalState();
    if (isFinal)
        setBusy(false);

    if (nodes != nullptr)
        puzzleWidget->setPieces(nodes);
}
void MainWindowImpl::onPuzzleSolved(StepParam* param) {
    int steps = param->getSteps();
    int states = param->getStates();
    log->append(tr("Решение найдено: %1 шагов, %2 состояний").arg(steps).arg(states));
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
bool MainWindowImpl::isBusy()
{
    if (busy)
        log->append(tr("Программа занята, подождите."));

    return busy;
}
void MainWindowImpl::getSocketImage(int dataSize)
{
    QImage image = QImage::fromData(socket.read(dataSize));
    if (!image.isNull()) {
        puzzleImage = QPixmap::fromImage(image);
        setupPuzzle();
        log->append(tr("<i>Изображение загружено!</i>"));

        imagesCache.replace(this->curRow, puzzleImage);
    }
    else
        log->append(tr("<i>Ошибка при получении изображения!</i>"));

}
void MainWindowImpl::socketError()
{
    if (imageSource != Net)
        return;

    log->append(tr("Ошибка TCP: %1").arg(socket.errorString()));
}
void MainWindowImpl::socketList(QList<QString> names)
{
    listImage->clear();
    imagesCache.clear();

    for (int i = 0; i < names.length(); i++) {
        listImage->addItem(names[i]);
        imagesCache.append(QPixmap());
    }
    listImage->update();
    log->append(tr("<i>Список доступных файлов загружен.</i>"));
}
void MainWindowImpl::socketIncorrectType(MessageType messageType)
{
    log->append(tr("<i>Неизвестный тип - %1</i>").arg(messageType));
}
