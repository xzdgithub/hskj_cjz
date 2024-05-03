#ifndef HOTPLUGTHREAD_H
#define HOTPLUGTHREAD_H

#include <QObject>
#include<QThread>
#include "libusb-1.0/libusb.h"
#include <QDebug>
#include<config.h>
#include<dasbuddy.h>
class HotPlugThread : public QThread
{
    Q_OBJECT
public:
    HotPlugThread(DASBuddy **buddy);
    void reloadConfig();
    void clearForm(int num);
signals:
    void deleteZFY(int num);
    void insertZFY(int num);
protected:
    void run();
private slots:
    void toreconnect(int zfynum);
private:
    int rc;
    void communicate();

    Config *getconfig;

    //int hotplug_callback(libusb_context *ctx, libusb_device *dev, libusb_hotplug_event event, void *user_data);

    int zfyreconnectnum = -1;


};

#endif // HOTPLUGTHREAD_H
