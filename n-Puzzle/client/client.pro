TEMPLATE = app
QT += widgets core network
CONFIG += qt warn_on release
OBJECTS_DIR = build
TARGET = n-puzzle
DESTDIR = bin
MOC_DIR = build
UI_DIR = build
FORMS = ui/mainwindow.ui ui/aboutDialog.ui
HEADERS = src/mainwindowimpl.h src/puzzlewidget.h src/piecesmodel.h src/aboutdialogimpl.h \
    src/filedownloader.h
SOURCES = src/mainwindowimpl.cpp \
 src/main.cpp \
 src/puzzlewidget.cpp \
 src/piecesmodel.cpp \
 src/aboutdialogimpl.cpp \
    src/filedownloader.cpp
RESOURCES += puzzle.qrc
TRANSLATIONS = client_ru_RU.ts client_en_US.ts client_es_ES.ts
RC_FILE += res.rc
CODECFORTR  = utf8

OTHER_FILES += \
    LICENCE.BSD \
    LICENCE.ICONS
