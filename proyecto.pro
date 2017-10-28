#-------------------------------------------------
#
# Project created by QtCreator 2011-04-20T16:53:10
#
#-------------------------------------------------

QT       += core gui
QT       += opengl

TARGET = ../proyecto
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    GLWidget.cpp \
    Cv3DMesh.cpp \
    mesh.cpp

HEADERS  += mainwindow.h \
    functions.h \
    GLWidget.h \
    Cv3DMesh.h \
    mesh.h

INCLUDEPATH += C:\OpenCV2.1\include \
               C:\OpenCV2.1\include\opencv

LIBS += C:\OpenCV2.1\lib\*.lib

FORMS    += mainwindow.ui
