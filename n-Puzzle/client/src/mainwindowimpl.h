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

#ifndef MAINWINDOWIMPL_H
#define MAINWINDOWIMPL_H
//
#include <QMainWindow>
#include <QPixmap>
#include <QTime>
#include <QAbstractSocket>
#include <QTcpSocket>
#include <QListView>
#include <QTranslator>
#include "filedownloader.h"
#include "puzzlewidget.h"
#include "piecesmodel.h"
#include "aboutdialogimpl.h"
#include "ui_mainwindow.h"

enum  ImageSourceType {Net=1, Internet, Local};

class MainWindowImpl : public QMainWindow, public Ui::MainWindow
{
        Q_OBJECT
public:
        MainWindowImpl(QWidget * = 0, Qt::WindowFlags = 0);

private slots:
        // меню "Файл"
        void openImage(const QString & = QString());
        void saveImage();
        // меню "Настройки"
        void setLogVisible();
        void setSettingsAll();
        void setServerSettings();
        void setLocalFolderSettings();
        void setInternetSettings();
        void setGameSettings();
        void setComplicationSettings();
        void setCompleted();
        // меню "Таймер"
        void timerOn();
        void timerOff();
        void timerReset();
        void updateTime();
        // меню "Язык"
        void setRussian();
        void setEnglish();
        void setSpanish();
        // меню "Помощь"
        void about();
        void aboutQt();
        // кнопки
        void clickScramble();
        void clickSolve();
        void incStep();
        void getRandomImage();
        // Получение изображения
        void getFileList();
        void getImage(const int);
        // // с сервера
        void tcpReady();
        void tcpError(QAbstractSocket::SocketError);
        void tcpConnected();
        void tcpDisconnected();
        // // из локальной папки
        void chooseDirectory();
        // // из интернета
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
        QString getCache();

        QPixmap puzzleImage;
        QListView *piecesList;
        PuzzleWidget *puzzleWidget;
        PiecesModel *model;
        QTimer *ptimer;
        bool multi;
        QPoint relation;
        int moves;
        int curRow;
        bool cacheUsed;
        FileDownloader *downloadedImage;
        // язык
        QString currLang;
        // Источник картинок
        ImageSourceType imageSource;
        //bool isRemoteSource;
        //bool isInternetSource;
        // сеть
        QTcpSocket socket;
        quint32 dataSize;
        quint32 messageType;
        //QStringList fileInfoList;

protected:
        void closeEvent(QCloseEvent *);
};
#endif
