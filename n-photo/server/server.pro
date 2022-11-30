# -------------------------------------------------
# Project created by QtCreator 2009-03-18T19:49:29
# -------------------------------------------------
HEADERS = src/dialog.h \
    src/server.h \
    src/databasework.h \
    src/clientsocket.h
SOURCES = src/dialog.cpp \
    src/main.cpp \
    src/server.cpp \
    src/databasework.cpp \
    src/clientsocket.cpp
QT += network sql widgets
TARGET = server
CONFIG += qt warn_on release
OBJECTS_DIR = build
DESTDIR = bin
MOC_DIR = build
UI_DIR = build
