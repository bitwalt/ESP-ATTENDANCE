#-------------------------------------------------
#
# Project created by QtCreator 2018-11-26T13:37:11
#
#-------------------------------------------------

QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ESP_grafica
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

LIBS += -lsqlite3

QMAKE_CXXFLAGS += -std=c++0x -pthread
LIBS += -pthread

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    dialog2.cpp \
    runtimewindow.cpp \
    qcustomplot.cpp \
    Analyzer.cpp \
    Database.cpp \
    Pack.cpp \
    Position.cpp \
    Server.cpp \
    form.cpp \
    dialog1.cpp \
    system.cpp \
    systhread.cpp \
    specific_situation.cpp \
    frequent_devices.cpp \
    md5.cpp \
    error_specific_situation.cpp \
    datastream.cpp

HEADERS += \
        mainwindow.h \
    dialog2.h \
    runtimewindow.h \
    qcustomplot.h \
    Analyzer.h \
    Database.h \
    Pack.h \
    Position.h \
    Server.h \
    form.h \
    dialog1.h \
    system.h \
    systhread.h \
    specific_situation.h \
    frequent_devices.h \
    md5.h \
    error_specific_situation.h \
    datastream.h

FORMS += \
        mainwindow.ui \
    dialog2.ui \
    runtimewindow.ui \
    dialog1.ui \
    specific_situation.ui \
    frequent_devices.ui \
    error_specific_situation.ui \
    datastream.ui

CONFIG += c++11

