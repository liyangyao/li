QT       += testlib

QT       -= gui

TARGET = li_test
CONFIG   += console
CONFIG   -= app_bundle
DESTDIR = $$PWD/bin

TEMPLATE = app


SOURCES += main.cpp

HEADERS += \
    tst_singleton.h

INCLUDEPATH += $$PWD/../..

