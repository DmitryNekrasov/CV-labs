QT += core
QT += gui

CONFIG += c++14

TARGET = CV-labs
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    matrix.cpp \
    myimage.cpp \
    core.cpp \
    simple.cpp \
    array.cpp \
    poi.cpp

HEADERS += \
    matrix.h \
    myimage.h \
    defs.h \
    core.h \
    simple.h \
    array.h \
    poi.h
