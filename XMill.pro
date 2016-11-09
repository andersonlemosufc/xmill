#-------------------------------------------------
#
# Project created by QtCreator 2016-11-04T13:02:45
#
#-------------------------------------------------

QT       += core gui xml xmlpatterns

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = XMill
TEMPLATE = app

CONFIG += c++14

SOURCES += main.cpp\
        mainwindow.cpp \
    parser/streamparser.cpp \
    compress/xmill.cpp \
    compress/container.cpp \
    util/util.cpp \
    compress/zlib/easyzlib.c \
    decompress/xdemill.cpp \
    decompress/decontainer.cpp

HEADERS  += mainwindow.h \
    parser/streamparser.h \
    xmill.h \
    tree/patricia.h \
    compress/xmill.h \
    compress/container.h \
    util/util.h \
    compress/zlib/easyzlib.h \
    decompress/xdemill.h \
    decompress/decontainer.h

FORMS    += mainwindow.ui
