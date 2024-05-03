#ifndef DASBUDDY_H
#define DASBUDDY_H

#include <QObject>
#include <QWidget>
#include<QLabel>
#include<QDebug>
#include<QPushButton>
#include<QPainter>
#include<QEvent>
#include<QString>
#include<QProgressBar>
#include<QWidget>
#include<QFrame>
#include<QColor>
#include "libusb-1.0/libusb.h"
#include<QThread>
#include<zfycontrol.h>
#include <QtNetwork>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include<QUrl>
#include<QTimer>
#include<QBitmap>
#include<QPainter>
#include "FileCopyer.h"
#include "DataVM.h"
struct command_block_wrapper {
    uint8_t dCBWSignature[4];
    uint32_t dCBWTag;
    uint32_t dCBWDataTransferLength;
    uint8_t bmCBWFlags;
    uint8_t bCBWLUN;
    uint8_t bCBWCBLength;
    uint8_t CBWCB[16];
};

class DASBuddy : public QWidget
{
    Q_OBJECT

public:
    explicit DASBuddy(bool deletefile,QWidget *parent = nullptr);
    ~DASBuddy();
    void setBackgroundColor(QColor color);
    void setUserName(QString name);
    void setUserUpload(QString upload);

    QString getUpload();
    void setphoto(QString path);

    QString getUserid() const;

    QString getDriverid() const;

    void toOpen(libusb_device *dev,int num);
    void toConnectZFY();

    void tosetUserId(QString userid);

    void tosetText(QString text);

    //void tosetListText(QStringList listtext);

    volatile bool getIsConnect() const;

    int num=-1;
    ZFYControl *zfy;
    QThread *controlthread = nullptr;

    void closeThread();
    void setIsConnect(volatile bool value);


    void setNetworkPic(const QString &szUrl);



    int getMybattery() const;
    void setMybattery(const int &value);

    void stopTimer();
    void setBatteryZero();
    void stopShine();

    void initTimer_shine();


    QString getWritingnow() const;

    void setWritingnow(const QString &value);

    void clearuploadsize();
    void startcopynoexits(QFileInfoList copylist);
    void starttocheckcopy(int num);
    void setPhotoText(QString text);
signals:
    void sendProgress(QVariant dataVar,qreal progress,int zfynum);
    void startCopy(QVariantMap varmap);
    void startToMount(int zfynum);
    void sendConnectPortNumber(int num,int battery);
    void getAllSettingStr(QVariantMap map);
    void sig_insertUserLog(int num);
    void insertZFY(int num);
    void sigtowirtesql(QVariantMap copyfilemap);
    void checkfileiscopy(QFileInfoList filelist,QString driverid,int num);
    void mountsuccess(int num);

    void zfycopyfinished();
public slots:
    void getAllSetting();
    void changewriting();
    void changewritingfail();
private slots:
    void finishedThreadSlot();
    void waittoCopy(int num);
    void updateProgress(QVariant dataVar,qreal progress,int zfynum);
    void postTogetNameAndimage();
    void requestFinished(QNetworkReply *reply);
    void updateBattery();
    QPixmap round(const QPixmap &img_in, int radius);
    void backgroundshine();

    void slotProgressBar(float value);
    void slotCopyFinished(bool result);
    void slotCopyonesuccess(QString,QString);
    void checkmount(int mount);

private:
    int Number;
    QLabel *UserName,*Battery;
    void initUi();
    QLabel *UserNameText,*UploadProcessText,*UploadText;

    //QProgressBar *processbar;
    //void initProcessBar();
    QFrame *frame_background,*frame_text;

    QLabel *label_photo;

    QLabel *BatteryText;
    QString converRGB2HexStr(QColor _rgb);
    libusb_device_handle *handle = NULL;

    QString userid,driverid;
    int mybattery;

    QString passwordtext = "@Ver#800000000";
    QString toGetBattery = "@Battery";
    QString toGetUserID =  "@GetUserID";
    QString toGetDriveID =  "@GetDriveID";
    QString toUdisktext =  "@Ner#800000000";


    QString moviesize,photosize,moviecycTime,moviecyc,dateWater;
    QString gsensor,moviecodec,prerec,delayrec,gPS,motiondet,ircut,recmode;

    QString toGetMoviesize =  "@GetMoviesize";
    QString toGetPhotosize =  "@GetPhotosize";
    QString toGetMoviecycTime =  "@GetMoviecycTime";
    QString toGetMoviecyc =  "@GetMoviecyc";
    QString toGetDateWater =  "@GetDateWater";
    QString toGetGsensor =  "@GetGsensor";
    QString toGetMoviecodec =  "@GetMoviecodec";
    QString toGetPrerec =  "@GetPrerec";
    QString toGetDelayrec =  "@GetDelayrec";
    QString toGetGPS =  "@GetGPS";
    QString toGetMotiondet =  "@GetMotiondet";
    QString toGetIrcut =  "@GetIrcut";
    QString toGetRecmode =  "@GetRecmode";


    volatile bool isConnect;

    int communicate(libusb_device_handle * devHandle,QString text);
    int transferBulkData(libusb_device_handle * usbHandle,command_block_wrapper * cbw,int data_length,unsigned char * buffer );


    QString writingnow;

    //dirpath = "/home/itventi/copyfile/"+QDateTime::currentDateTime().toString("yyyy-MM-dd") + "/" + QString::number(user_Id) +"/";


    QNetworkRequest request;
    QNetworkAccessManager* naManager;
    QTimer *timer_battery = nullptr;//battery power +1 everyminmute;
    QTimer *timer_shine = nullptr;
    void initTimer_battery();

    bool deletefile;






    qint64 copySize = 0;
    //qint64 sizeall = 0;
    QString mkMutiDir(const QString path);
    void getAllFile(QString path, QFileInfoList &file_all, QFileInfoList &file_gps, qint64 &sizeall);

    DataVM *myDataVm;

    QFileInfoList file_gps;


    QVariantMap gpsMap;

    QDateTime uploadDate;
    QString dirpath;
    QString virtualpath;
    bool isFileExist(QString filepath);
};

#endif // DASBUDDY_H
