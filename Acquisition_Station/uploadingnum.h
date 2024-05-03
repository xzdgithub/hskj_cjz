#ifndef UPLOADINGNUM_H
#define UPLOADINGNUM_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include<QMutexLocker>
#include<QDebug>
#include<config.h>
class UploadingNum: public QThread
{
    Q_OBJECT
public:
    UploadingNum();
    bool addUploadingnum();
    void subUploadingnum();
    void setiswaitcopy(int num,bool value);
signals:
    void sigtocopy(int num);
protected:
    void run();
private:
    int uploadingnum;

    // 加锁
    QMutex mutex;
    bool iswaitcopy[30];
    int uploadingsametime;

};

#endif // UPLOADINGNUM_H
