#include "zfysetting.h"

ZFYSetting::ZFYSetting(DASBuddy **buddy,QObject *parent) : QObject(parent)
{
    qDebug()<<"ZFYSetting_MainThread:"<<QThread::currentThreadId();
    for (int i=0;i<30;i++) {
        dasbuddy[i] = buddy[i];
    }

}

void ZFYSetting::togetZFYSettings(int num)
{
    qDebug()<<"ZFYSetting_currentThreadId:"<<QThread::currentThreadId();
    qDebug()<<"togetZFYSettings num:"<<num;
    dasbuddy[num]->getAllSetting();

}

void ZFYSetting::tosetZFY(int num, QString settingtext)
{
    dasbuddy[num]->tosetText(settingtext);
}
void ZFYSetting::tosetlistZFY(int num, QStringList settinglisttext)
{
    //dasbuddy[num]->tosetListText(settinglisttext);

    if(!settinglisttext.isEmpty()){
        qDebug()<<"settinglisttext.count()"<<settinglisttext.count();
        for (int i = 0;i<settinglisttext.count();i++) {

            dasbuddy[num]->tosetText(settinglisttext.at(i));
            QThread::msleep(200);

            if(i==settinglisttext.count()-1){

                qDebug()<<"writefinish!!";
                emit setListsuccess();
            }

            qDebug()<<"write i:"<<i;
        }
    }
}

void ZFYSetting::tosetlistZFYlist(QList<int> connectlist, QStringList settinglisttext)
{

    if(!connectlist.isEmpty()){
        if(!settinglisttext.isEmpty()){
            for (int i=0;i<connectlist.count();i++) {

                dasbuddy[connectlist.at(i)]->clearuploadsize();
                QThread::msleep(300);
                for (int j = 0;j<settinglisttext.count();j++) {
                    dasbuddy[connectlist.at(i)]->tosetText(settinglisttext.at(j));
                    QThread::msleep(200);
                    if(j==settinglisttext.count()-1){

                        qDebug()<<"write one device finish!!";
                        emit setoneListsuccess(connectlist.at(i));
                    }
                }

                if(i==connectlist.count()-1){

                    qDebug()<<"write all finish!!";
                    emit setallListsuccess();
                }
            }
        }
    }
}

void ZFYSetting::toreconnect(int num)
{
    qDebug()<<"zfysetting to reconnect!!!";
    dasbuddy[num]->toConnectZFY();
}
