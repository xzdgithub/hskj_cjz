#ifndef ZFYCONTROL_H
#define ZFYCONTROL_H

#include <QObject>
#include<QFile>
#include<QDebug>
#include<QDir>
#include<QFileInfoList>
#include <QUuid>
#include<QThread>
#include<QDateTime>
#include<QProcess>
#include<config.h>
struct File_copy_Info{
    QString file_uuid;
    QString file_name;
    QString file_est;
    QString file_path;
    QString user_id;
    QString driver_id;
    QString file_gps;
    QDateTime file_uploadtime;
    QDateTime file_creattime;
    qint64 file_size;
    int file_type;

};
Q_DECLARE_METATYPE(File_copy_Info);
class ZFYControl : public QObject
{
    Q_OBJECT
public:
    explicit ZFYControl(bool autodeletecopyfile,QObject *parent = nullptr);
    ZFYControl(int zfynum);
    ~ZFYControl();

    QFileInfoList copyFile(QStringList *uuid,QString dirpath);
    QString getBattery();
    void closeThread();

    void setPath(const QString &value);

    void setZfynum(int value);


    void clearSize();
signals:
    void copyonesuccess(QVariant dataVar,qreal progress,int zfynum);
    void mount_finish(int num);
    void start_mount(int num);
    void mount_fail();
public slots:
    void copyFile2(QVariantMap map);
    void readBashStandardOutputInfo();
    void readBashStandardErrorInfo();
    void mountToUdisk(int num);
private:
    //QString path ="/mnt/usb/zfy1";
    QString path;
    int user_Id = 123456;
    QString mkMutiDir(const QString path);

    QFileInfoList getAllFile();

    qint64 copySize = 0;
    qint64 sizeall = 0;

    int zfynum = -1;

    volatile bool isStop;
    bool myProcess(QString cmd, QStringList param, QString &output);

    QProcess *m_proces_bash;
    void initMount();
    bool autodeletecopyfile;
    bool isDirExist(QString dirpath);
    bool isFileExist(QString filepath);
};

#endif // ZFYCONTROL_H
