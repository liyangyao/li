#-------------------------------------------------
#
# Project created by QtCreator 2015-06-30T10:54:44
#
#-------------------------------------------------

QT       += testlib network

QT       -= gui

TARGET = tst_http_parser_test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_http_parser_test.cpp \
    httpparserex.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"


include(..\..\HttpParser.pri)
INCLUDEPATH += (..\..\..\)

HEADERS += \
    httpparserex.h

