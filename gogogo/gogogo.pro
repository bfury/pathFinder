
QT       += core gui \
            widgets
CONFIG += console thread

TARGET = gogogo
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS \
            QT_NO_WARNING_OUTPUT

SOURCES += \
    GUI.cpp \
        main.cpp \
    pathNet.cpp \
    point.cpp \
    zoomNet.cpp

HEADERS += \
    GUI.h \
    pathNet.h \
    point.h \
    road.h \
    zoomNet.h

FORMS += \
    GUI.ui

RESOURCES += \
    FileForRead.qrc
