#ifndef MAINFORM_H
#define MAINFORM_H

#include <QWidget>
#include<dasbuddy.h>
#include<QFile>
#include<zfycontrol.h>
#include<mysqllite.h>
#include<QFileInfoList>
#include<QDateTime>
#include<hotplugthread.h>
#include<QMessageBox>
#include<loginform.h>
#include<QDesktopServices>
#include<QUrl>
#include<insertzfydialog.h>
#include<QNetworkRequest>
#include <QtNetwork>
#include<chooseuserdialog.h>
#include<QListWidgetItem>
#include<zfysetting.h>
#include<uploadingnum.h>
namespace Ui {
class MainForm;
}


class MainForm : public QWidget
{
    Q_OBJECT

public:
    explicit MainForm(DASBuddy **buddy,MySqlLite *sqltie,ZFYSetting *zfysetting,QWidget *parent = nullptr);
    ~MainForm();
    void setlistWidget();

    void setIPV4Label(QString ipv4Address);
signals:
    //type = 0 means can see all fiels type = 1 means can only see own files
    void gosearch(int type,QString username,bool videojur);
    void gosetup(QList<int> connetnumber);
    void startCopy(QString dirpath,int zfynum);
    void insertfiles_sqlite(QVariantMap map);
    void takedevice(int num);
    void toinsert_user_Log(QVariantMap map);
    void connectfinish();
    void getUsers(QStringList userlist);
    void toshineBackground();
    void gonewsetup();

    void sigsqltocheckfile(QFileInfoList filelist,QString driverid,int num);

    void sigtobuddycheckcopy(int num);
    void toopenbroswer();
private slots:
    void on_pushButton_searchfiles_clicked();
    void getZFYConnectPortNumber(int num,int battery);
    void todeleteZFY(int num);

    void toinsertUserLogAndPost(int num);

    void updateProgress(QVariant dataVar,qreal progress,int zfynum);
    void finishedThreadSlot();

    void on_pushButton_setup_clicked();

    void reloadconfig(int old_num);
    void on_btn_takecamera_clicked();

    void checkjurstr(int type,QString login_UserName,QStringList jurstr);
    void on_btn_manager_clicked();

    void toinsertZFYDialog(int num);
    void deleteZFYDialog();

    void requestFinished(QNetworkReply *reply);
    void on_btn_config_clicked();
    void updatedisksizelabel(double disksize,bool pingresult);
    void adminwriteuser(QString polno);
    void on_pushButton_setup2_clicked();

    void slottoinsertsql(QVariantMap copyfilemap);

    void slottocheckfile(QFileInfoList filelist,QString driverid,int num);
    void slotstartcopy(QFileInfoList filelist,int num);

    void slottocppy(int num);

    void slottosubuploadingnum();

private:
    Ui::MainForm *ui;
    DASBuddy *dasbuddy[30];
    QString getBattery(QString path);
    QString path = "/mnt/usb/zfy1/LOG/BATTERY.TXT";
    ZFYControl *zfy1 ;
    ZFYControl *zfy2 ;
    QStringList uuid;
    MySqlLite *mysql;
    QString dirpath;

    //libusb_device_handler * deviceHandler;

    int user_Id = 123456;
    //QThread *controlthread;
    //QThread *controlthread2;
    void closeThread();

    HotPlugThread *thread1;
    LoginForm *loginForm;
    InsertZFYDialog *insertZFYDialog = nullptr;
    ChooseUserDialog *chooseUserDialog;

    QNetworkRequest request;
    QNetworkAccessManager* naManager;

    int maxindex = -1;


    ZFYSetting *zfysetting;

    UploadingNum *uploadingnum;
};

#endif // MAINFORM_H
