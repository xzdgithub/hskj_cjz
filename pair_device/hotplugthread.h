#ifndef HOTPLUGTHREAD_H
#define HOTPLUGTHREAD_H

#include <QObject>
#include<QThread>
#include "libusb-1.0/libusb.h"
#include <QDebug>
class HotPlugThread: public QThread
{
    Q_OBJECT
public:
    HotPlugThread();

signals:
    void deleteZFY(QString deleteNum);
    void insertZFY(QString insertNum);
protected:
    void run();
private:
    int rc;

private slots:

};

#endif // HOTPLUGTHREAD_H
