#-------------------------------------------------
#
# Project created by QtCreator 2020-11-12T04:37:31
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PairDevice
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    hotplugthread.cpp \
    config.cpp

HEADERS += \
        mainwindow.h \
    hotplugthread.h \
    config.h

FORMS += \
        mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


unix {
#    !packagesExist(libusb-1.0):error("Could not find libusb-1.0 using pkg-config")
#    PKGCONFIG += libusb-1.0
#    CONFIG += link_pkgconfig

INCLUDEPATH += /home/astra/Documents/libusb/include/
LIBS += -L/home/astra/Documents/libusb/lib -lusb-1.0
}

RESOURCES += \
    resource.qrc
