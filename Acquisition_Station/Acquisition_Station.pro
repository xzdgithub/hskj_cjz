#-------------------------------------------------
#
# Project created by QtCreator 2020-09-17T06:30:22
#
#-------------------------------------------------

QT       += core gui
QT += av avwidgets
QT += sql
QT += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Acquisition_Station
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

#include(QtUsb/src/usb/files.pri)
SOURCES += \
        main.cpp \
        mainwindow.cpp \
    myavplayer.cpp \
    Slider.cpp \
    customtabstyle.cpp \
    dasbuddy.cpp \
    loginform.cpp \
    searchbytype.cpp \
    searchbydate.cpp \
    howtosearch.cpp \
    mainform.cpp \
    searchbystaffname.cpp \
    videoplayerform.cpp \
    getusbinfo.cpp \
    zfycontrol.cpp \
    mysqllite.cpp \
    config.cpp \
    hotplugthread.cpp \
    mymqproducer.cpp \
    mymqconsumer.cpp \
    setupform.cpp \
    zfysetting.cpp \
    getsettingdialog.cpp \
    insertzfydialog.cpp \
    chooseuserdialog.cpp \
    newsetupform.cpp \
    writesettingdialog.cpp \
    DataVM.cpp \
    FileCopyer.cpp \
    uploadingnum.cpp

HEADERS += \
        mainwindow.h \
    myavplayer.h \
    Slider.h \
    customtabstyle.h \
    dasbuddy.h \
    loginform.h \
    searchbytype.h \
    searchbydate.h \
    howtosearch.h \
    mainform.h \
    searchbystaffname.h \
    getusbinfo.h \
    videoplayerform.h \
    zfycontrol.h \
    mysqllite.h \
    config.h \
    hotplugthread.h \
    mymqproducer.h \
    mymqconsumer.h \
    setupform.h \
    zfysetting.h \
    getsettingdialog.h \
    insertzfydialog.h \
    chooseuserdialog.h \
    newsetupform.h \
    writesettingdialog.h \
    DataVM.h \
    FileCopyer.h \
    uploadingnum.h

FORMS += \
        mainwindow.ui \
    loginform.ui \
    searchbytype.ui \
    searchbydate.ui \
    howtosearch.ui \
    mainform.ui \
    searchbystaffname.ui \
    videoplayerform.ui \
    setupform.ui \
    getsettingdialog.ui \
    insertzfydialog.ui \
    chooseuserdialog.ui \
    newsetupform.ui \
    writesettingdialog.ui



#language
TRANSLATIONS = lang_cn.ts\
               lang_en.ts\
               lang_ru.ts
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resourse.qrc

DISTFILES +=


unix {
    !packagesExist(libusb-1.0):error("Could not find libusb-1.0 using pkg-config")
    PKGCONFIG += libusb-1.0

    CONFIG += link_pkgconfig

LIBS += -L/usr/local/ActiveMQ-CPP/lib -lactivemq-cpp

DEPENDPATH += /usr/local/ActiveMQ-CPP/lib

INCLUDEPATH += /usr/local/ActiveMQ-CPP/include/activemq-cpp-3.9.5
}
