#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QDateTime>
#include<mainform.h>
#include<howtosearch.h>
#include<searchbydate.h>
#include<searchbytype.h>
#include<searchbystaffname.h>
#include<videoplayerform.h>
//#include "qusbdevice.h"
//#include "qusbinfo.h"
#include "libusb-1.0/libusb.h"
#include<getusbinfo.h>
#include<QMessageBox>
#include<config.h>
#include<mymqproducer.h>
#include<mymqconsumer.h>
#include<mysqllite.h>
#include <QtNetwork>
#include<QJsonArray>
#include<QJsonDocument>
#include<QJsonObject>
#include<setupform.h>
#include<dasbuddy.h>
#include<QUuid>
#include<QMouseEvent>
#include<loginform.h>
#include<QTimer>
#include<newsetupform.h>
#include<zfysetting.h>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
signals:
    //void govideo(QString path,int search_type);
    void govideo(QVariantMap videomap);
    void connecthubtrue();
    void connecthubfalse();
    void sendConnectPortNumber(QList<int> list);
    void toupdateSql(QVariantMap map);
    void todeleteSql(QVariantMap map);
    void toinsert_AcStation_log(QVariantMap map);
    void toinsert_user_Log(QVariantMap map);
    void updateDiskSize(double disksize,bool pingresult);
private slots:
    void gotoSearchForm(int type,QString userid,bool videojur);
    void gotoSearch_By_Files_Type(int type,QString userid,bool videojur);
    void gotoSearch_By_Date(int type,QString userid,bool videojur);
    void gotoSearch_By_Staff_Name(int type,QString userid,bool videojur);
    void goBackMainForm();
    void goBackSearch(int search_type);
    //void gotoVideoForm(QString path,int search_type);
    void gotoVideoForm(QVariantMap videomap);
    //void onDevInserted(QUsbDevice::Id id);
    //void onDevRemoved(QUsbDevice::Id id);
    void changetoUdisk();


    void doinsertActiveMq(QVariant dataVar);
    void getConsumerMessage(QString str);

    void toexitApp();
    void gotoSetupForm(QList<int> connectnumber);
    void gotonewSetupForm();
    void readBashStandardOutputInfo();
    void readBashStandardErrorInfo();
    void doMqinsertLog(int type,QVariantMap map);
    void checkjurstr(int type,QString login_UserName,QStringList jurstr);
    void tosendConfirmtoSerVer(int status,QString taskid);
    void uploaddisksize();
    void getnetworkanddiskmsg();
    void goBackMainFormaddwidget();
    void updatelabeltime();

    void openbroswer();
    void goBackSearchForm();
private:
    Ui::MainWindow *ui;
    int firstIndex;
    MainForm *form;
    VideoPlayerForm *videoform;
    HowToSearch *howtosearch;
    SearchByType *searchbytype;
    SearchByDate *searchbydate;
    SearchByStaffName *searchbystaffname;
    SetUpForm *setupform;
    //QUsbInfo m_usb_info;
    QThread t;
    GetUsbInfo *getusb;
    QList<STUUSBDevices> ADSDeviceslist;
    //QStringList usbhublist = {"3.1","3.2.1","3.2.2","3.3.1","3.3.2","3.4","3.2.4","3.2.3","3.3.4","3.3.3","4.1","4.2.1","4.2.2","4.3.1","4.3.2","4.4","4.2.4","4.2.3","4.3.4","4.3.3"};
    void checkHubConnect();
    QStringList usbhublist;
    //ConnectADS *connectads;
    QList<int> ZFYConnectNumber;

    Config *getconfig;

    MyMqProducer myproducer;

    MyMqConsumer myconsumer;

    MySqlLite *mysqlite;
    QString MyHostIPV4Address;
    void closeEvent(QCloseEvent *event);
private:
    void initActiveMq();

    QHostAddress getHostIPV4Address();
    DASBuddy *dasbuddy[30];


    QProcess *m_proces_bash;
    void initProcess();
    QDateTime systemstarttime;
    QDateTime AcStation_starttime;
    LoginForm *loginForm;
    QTimer *timer_uploadDiskSize;
    QTimer *timer_updateformlabel;
    QTimer *updatetime;
    QStorageInfo storage;
    double freedisksize;
    double totaldisksize;
    bool pingOk(QString sIp);
    QString pingip;
    newSetUpForm *newsetupform;

    QThread *zfysetupthread;
    ZFYSetting *zfysetting;
};

#endif // MAINWINDOW_H
