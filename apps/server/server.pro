# -------------------------------------------------
# Project created by QtCreator 2009-03-18T19:49:29
# -------------------------------------------------
HEADERS = \
    src/server.h \
    src/databasework.h \
    src/connectionsocket.h \
    src/enums.h
SOURCES = \
    src/main.cpp \
    src/server.cpp \
    src/databasework.cpp \
    src/connectionsocket.cpp
QT += network sql widgets core
TARGET = server
CONFIG += warn_on release console
CONFIG -= app_bundle
OBJECTS_DIR = build
DESTDIR = bin
MOC_DIR = build
UI_DIR = build
