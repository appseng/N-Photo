TEMPLATE = app
QT += core network widgets
CONFIG += qt c++17
OBJECTS_DIR = build
TARGET = n-photo
DESTDIR = bin
MOC_DIR = build
UI_DIR = build
FORMS = ui/mainwindow.ui \
    ui/aboutDialog.ui
HEADERS = src/mainwindowimpl.h \
    src/puzzlewidget.h \
    src/piecesmodel.h \
    src/aboutdialogimpl.h \
    src/state.h \
    src/puzzlestrategy.h \
    src/enums.h \
    src/solvethread.h \
    src/params.h \
    src/downloader.h \
    src/clientsocket.h
SOURCES = src/mainwindowimpl.cpp \
    src/main.cpp \
    src/puzzlewidget.cpp \
    src/piecesmodel.cpp \
    src/aboutdialogimpl.cpp \
    src/state.cpp \
    src/puzzlestrategy.cpp \
    src/solvethread.cpp \
    src/params.cpp \
    src/downloader.cpp \
    src/clientsocket.cpp
RESOURCES += puzzle.qrc
TRANSLATIONS = client_ru_RU.ts \
    client_en_US.ts \
    client_es_ES.ts
RC_FILE += res.rc
CODECFORTR  = utf8

# remove possible other optimization flags
#QMAKE_CXXFLAGS_RELEASE -= -O3
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE -= -O0
