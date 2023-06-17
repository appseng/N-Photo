#ifndef MAINWINDOWIMPL_H
#define MAINWINDOWIMPL_H

#include <QMainWindow>
#include <QPixmap>
#include <QTime>
#include <QAbstractSocket>
#include <QTcpSocket>
#include <QListView>
#include <QTranslator>

// UI: widget
#include "puzzlewidget.h"
#include "piecesmodel.h"

// UI: dialog
#include "aboutdialogimpl.h"

// generated UI for this class
#include "ui_mainwindow.h"

// for downloading images from the Internet
#include "downloader.h"

// for finding solution
#include "state.h"
#include "solvethread.h"

// for networking
#include "clientsocket.h"

class MainWindowImpl : public QMainWindow, public Ui::MainWindow
{
        Q_OBJECT
public:
        MainWindowImpl(QWidget * = nullptr, Qt::WindowFlags = nullptr);
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

        void displayState(Param*);
        void onPuzzleSolved(Param*);
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
        void getSocketImage(int);
        void socketError();
        void socketList(QList<QString>);
        void socketIncorrect(MessageType);
        void tcpConnect(MessageType);
        // // from a local directory
        void chooseDirectory();
        // // from the Internet
        void loadImage();
private:
        void readSettings();
        void writeSettings();

        void loadLanguage(const QString&);
        QTranslator translator;

        void setComplication();
        void detectGameType();
        void setupWidgets();
        void setupPuzzle();
        void setTimer(const bool, const QTime = QTime());
        void showPuzzleField();

        void getInternetImage();

        void setBusy(bool);
        QPixmap puzzleImage;
        QListView *piecesList;
        PuzzleWidget *puzzleWidget;
        PiecesModel *model;
        QTimer *ptimer;
        bool multi;
        QPoint relation;
        int moves;
        int curRow;

        QList<QPixmap> images;

        PuzzleStrategy *strategy;
        SolveThread *thread;
        QTimer *solutionTimer;
        Heuristic heuristic;
        QVector<char> nodes;
        QStack<State*> *path;
        bool busy;

        Downloader downloader;
        int imageIndex;
        // language
        QString currLang;
        // Source of images
        ImageSourceType imageSource;
        // network
        ClientSocket socket;
        int dataSize;
        unsigned int messageType;
protected:
        void closeEvent(QCloseEvent *);
};
#endif
