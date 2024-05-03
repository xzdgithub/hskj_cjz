#ifndef ZFYSETTING_H
#define ZFYSETTING_H

#include <QObject>
#include<dasbuddy.h>
#include<QDebug>
#include<QThread>

class ZFYSetting : public QObject
{
    Q_OBJECT
public:
    explicit ZFYSetting(DASBuddy **buddy,QObject *parent = nullptr);

signals:
    void setListsuccess();
    void setoneListsuccess(int num);
    void setallListsuccess();
public slots:
    void togetZFYSettings(int num);

    void tosetZFY(int num,QString settingtext);
private slots:
    void tosetlistZFY(int num, QStringList settinglisttext);
    void tosetlistZFYlist(QList<int> connectlist, QStringList settinglisttext);
    void toreconnect(int num);
private:
    DASBuddy *dasbuddy[30];
};

#endif // ZFYSETTING_H
