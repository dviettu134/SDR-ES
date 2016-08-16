# -------------------------------------------------
# Project created by QtCreator 2016-06-23T17:47:21
# -------------------------------------------------
QT += core \
    gui
greaterThan(QT_MAJOR_VERSION, 4):QT += widgets
TARGET = SDR-ES
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    console.cpp \
    hled.cpp \
    filedata.cpp \
    function_Check.cpp
HEADERS += mainwindow.h \
    console.h \
    hled.h \
    filedata.h \
    define.h \
    function_Check.h
FORMS += mainwindow.ui
include(qextserialport/qextserialport.pri)
