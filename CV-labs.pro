QT += core
QT += gui

CONFIG += c++14

TARGET = CV-labs
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    matrix.cpp \
    myimage.cpp

HEADERS += \
    matrix.h \
    myimage.h \
    defs.h
