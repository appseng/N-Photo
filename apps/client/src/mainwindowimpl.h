#ifndef MAINWINDOWIMPL_H
#define MAINWINDOWIMPL_H

#include <QMainWindow>
#include <QPixmap>
#include <QTime>
#include <QAbstractSocket>
#include <QTcpSocket>
#include <QListView>
#include <QTranslator>
#include <QRandomGenerator>

// UI: widget
#include "puzzlewidget.h"
#include "piecesmodel.h"

// UI: dialog
#include "aboutdialogimpl.h"

// generated UI for this class
#include "ui_mainwindow.h"

// for finding solution
#include "state.h"
#include "solvethread.h"

// for downloading images from the Internet
#include "downloader.h"
// for networking
#include "clientsocket.h"

class MainWindowImpl : public QMainWindow, public Ui::MainWindow
{
        Q_OBJECT
public:
        MainWindowImpl(QWidget * = nullptr, Qt::WindowFlags = Qt::WindowFlags());
        ~MainWindowImpl();
private slots:
        // menu "File"
        void openImage(const QString & = QString());
        void saveImage();
        // menu "Settings"
        void setLogVisible();
        void setSettingsAll();
        void setServerSettings();
        void setLocalFolderSettings();
        void setInternetSettings();
        void setGameSettings();
        void setComplicationSettings();
        void setCompleted(bool);
        // menu "Timer"
        void timerOn();
        void timerOff();
        void timerReset();
        void updateTime();
        // menu "Language"
        void setRussian();
        void setEnglish();
        void setSpanish();
        // menu "Solution"
        void solvePuzzle();
        void setMisplacedTiles();
        void setManhattanDistance();
        void moveMissingRectangleLeft();
        void moveMissingRectangleUp();
        void moveMissingRectangleDown();
        void moveMissingRectangleRight();
        // solution
        void displayState(StateParam*);
        void onPuzzleSolved(StepParam*);
        void updateState();
        void startTimer(QStack<State*>*);
        // menu "Help"
        void about();
        void aboutQt();
        // buttons
        void clickShuffle();
        void clickRefresh();
        void incStep();
        void getRandomImage();
        // getting images
        void getFileList();
        void getImage(const int);
        // // from a server
        void getSocketImage(const QByteArray);
        void socketError();
        void socketList(QList<QString>);
        void socketIncorrectType(MessageType);
        // // from a local directory
        void chooseDirectory();
        // // from the Internet
        void loadImage();
private:
        void readSettings();
        void writeSettings();

        // language
        void loadLanguage(const QString&);
        QTranslator translator;
        QString currentLanguage;

        // network
        void ConnectToServer(MessageType);
        ClientSocket socket;
        Downloader downloader;

        // puzzle widget
        void setupWidgets();
        void setupPuzzle();
        void showPuzzleField();
        QPixmap puzzleImage;
        QListView *piecesList;
        PuzzleWidget *puzzleWidget;
        PiecesModel *model;
        int curRow;

        // complication
        void setComplication();
        QPoint relation;
        // game type
        void detectGameType();
        GameType gameType;

        // timer
        void setTimer(const bool, const QTime = QTime());
        void timerRestart();
        QTimer *guiTimer;

        // Source of images
        void getInternetImage();
        ImageSourceType imageSource;

        // memory caching
        QList<QPixmap> imagesCache;

        // solution
        void setBusy(bool);
        bool isBusy();
        bool busy;
        int moves;
        PuzzleStrategy *strategy;
        SolveThread *thread;
        QTimer *solutionTimer;
        Heuristic heuristic;
        QVector<char> nodes;
        QStack<State*> *path;
        
        QRandomGenerator rand;

protected:
        void closeEvent(QCloseEvent *);
};
#endif
