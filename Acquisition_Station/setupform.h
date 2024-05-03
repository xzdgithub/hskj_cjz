#ifndef SETUPFORM_H
#define SETUPFORM_H

#include <QWidget>
#include<QDebug>
#include<QFile>
#include<config.h>
#include<QProcess>
#include<QMessageBox>
#include<dasbuddy.h>
#include<QComboBox>
#include<QThread>
#include<getsettingdialog.h>
#include<QListWidgetItem>
#include<writesettingdialog.h>
#include<zfysetting.h>
#include<QRegExpValidator>
namespace Ui {
class SetUpForm;
}

class SetUpForm : public QWidget
{
    Q_OBJECT

public:
    explicit SetUpForm(DASBuddy **buddy,QList<int> connectnumber,ZFYSetting *zfysetting,QWidget *parent = nullptr);
    ~SetUpForm();
signals:
    void goback();

    void changesuccess(int old_num);

    void togetSetting(int num);

    void towriteSetting(int num,QString writetext);

    void towriteSettingList(int num,QStringList writetextlist);

    void towriteListofSettingList(QList<int> connectlist,QStringList writetextlist);
    void getsettingfinish();

    void writesettingfinish();

private slots:
    void on_pushButton_goBack_clicked();

    void on_pushButton_change_clicked();

    void readBashStandardOutputInfo();
    void readBashStandardErrorInfo();


    void on_comboBox_mylist_currentIndexChanged(int index);


    void recieveAllSetting(QVariantMap map);


    void on_comboBox_Moviesize_currentIndexChanged(int index);

    void on_comboBox_Photosize_currentIndexChanged(int index);

    void on_comboBox_MoviecycTime_currentIndexChanged(int index);

    void on_comboBox_Moviecyc_currentIndexChanged(int index);

    void on_comboBox_DateWater_currentIndexChanged(int index);

    void on_comboBox_Gsensor_currentIndexChanged(int index);

    void on_comboBox_Moviecodec_currentIndexChanged(int index);

    void on_comboBox_Prerec_currentIndexChanged(int index);

    void on_comboBox_Delayrec_currentIndexChanged(int index);

    void on_comboBox_GPS_currentIndexChanged(int index);

    void on_comboBox_Motiondet_currentIndexChanged(int index);

    void on_comboBox_Ircut_currentIndexChanged(int index);

    void on_comboBox_Recmode_currentIndexChanged(int index);

    //void on_btn_setDate_clicked();

    void deleteSettingDialog();
    void deletewriteSettingDialog();

    void on_comboBox_language_currentIndexChanged(int index);



    void on_listWidget_itemClicked(QListWidgetItem *item);

    void on_btn_write_clicked();


    void on_btn_writeAll_clicked();

private:
    Ui::SetUpForm *ui;
    void changeUdevFile(int number_old,int number_change);
    void changeConfig(int number_old, int number_change);
    void restartUdevService();
    QProcess *m_proces_bash;
    void initProcess();
    DASBuddy *dasbuddy[30];

    void set_combobox_text(QComboBox *box,int num);
    bool isUserSelect;
    //QThread *setupthread;
    ZFYSetting *zfysetting;
    getSettingDialog *setting_dialog;
    writeSettingDialog *writesetting_dialog;
    void setcomboxdisable();
    void setcomboxenable();
    void setLanguage(QString language);
    void getLanguage();
    bool isAutoGetLanguage;
    int settingnum = -1;
};

#endif // SETUPFORM_H
