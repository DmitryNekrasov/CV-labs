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
    poi.cpp \
    qimagegenerator.cpp \
    descriptors.cpp \
    pyramid.cpp

HEADERS += \
    matrix.h \
    myimage.h \
    defs.h \
    core.h \
    simple.h \
    array.h \
    poi.h \
    qimagegenerator.h \
    descriptors.h \
    algo.h \
    pyramid.h

LIBS += -L/usr/local/lib \
        -lgsl \
        -lgslcblas

INCLUDEPATH += /usr/local/include

DEPENDPATH += /usr/local/include
