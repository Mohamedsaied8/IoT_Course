QT       += core

QT       -= gui

TARGET = threadTest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
#QMAKE_CXXFLAGS += -std=c++ -pthread
LIBS += -pthread

LIBS += C:\Qt\Qt5.2.0\Tools\mingw48_32\i686-w64-mingw32\lib\libwsock32.a

SOURCES += main.cpp
