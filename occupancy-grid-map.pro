#-------------------------------------------------
#
# Project created by QtCreator 2018-01-06T13:38:32
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ocuppancy-grid-test
TEMPLATE = app

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/include
INCLUDEPATH += $$PWD/gui

SOURCES += main.cpp\
        gui/mainwindow.cpp \
        gui/graphicsview.cpp \
        gui/griditem.cpp \
        src/gridmap.cpp \
        gui/dialog.cpp \
        gui/fielditem.cpp

HEADERS  += gui/mainwindow.h \
    gui/graphicsview.h \
    gui/griditem.h \
    include/gridmap.h \
    include/util.hpp \
    gui/dialog.h \
    gui/fielditem.h

FORMS    += gui/mainwindow.ui \
    gui/dialog.ui

RESOURCES += \
    gui/gui.qrc
