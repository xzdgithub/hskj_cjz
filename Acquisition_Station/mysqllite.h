#ifndef MYSQLLITE_H
#define MYSQLLITE_H
#include <qdebug.h>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include<QSqlTableModel>
#include<QDateTime>
#include <QObject>
#include<zfycontrol.h>
#include<config.h>
struct FileInfo{
    QString file_name;
    QString file_date;
    QString file_upload_time;
    QString file_driverid;
    QString file_remark;
    QString file_userid;
    QString filepath;
    QString file_shortpath;
    QString file_est;
};

struct FileAllInfo{
    QString file_uuid;
    QString file_name;
    QString user_id;
    QString driver_id;
    QString file_caseNo;
    QString fileshowpath;
    QString file_gps;
    QDateTime create_time;
    QDateTime upload_time;
    qint64 file_size;
    int file_type;
    int save_max_day;
    int is_important;

};
Q_DECLARE_METATYPE(FileAllInfo);
struct UserInfo{
    int user_id;
    QString user_name;
    QString user_photo_url;
};
class MySqlLite :public QObject
{
    Q_OBJECT
public:
    explicit MySqlLite(QObject *parent = nullptr);
    QSqlDatabase    sqliteDatabase;
    void openmysql();
    void checkconnection();
    QStringList gettableNameList();
    void readTable();
    void insertData();
    void updateData();
    void deleteData();
    void closeSqlLite();
    bool exec(QString text);
    bool insertb_files(QString uuid,QString file_name,QString user_id,QString driver_id,QDateTime create_time,QDateTime uploadtime,qint64 file_size,int file_type,QString file_path,QString file_est,QString file_gps,QString virtualpath);
    void insertb_user();


    void gettb_files_bydate(int type,QString userid,QDate date);
    bool isfileexit(QString filename, QString driverid);
public slots:
    void get_tb_files_type(int type_jur,QString userid,int type);
    void get_tb_files_withdate(int type,QString userid,QDate date);
    void get_tb_users();

    void get_tb_files_withuserName(QString username);
    void doinsertfiles(QVariantMap copyfilemap);

    void WriteCutSql(QVariantMap cutmap);
    void toupdateFromMq(QVariantMap map);
    void todeleteFromMq(QVariantMap map);

    void insert_AcStation_Log(QVariantMap map);

    void insert_User_Log(QVariantMap map);
signals:
    void gettb_files_success(QList<FileInfo>);
    void gettb_user_success(QList<UserInfo>);
    void insertsuccess(QVariant dataVar);
    void insertsuccesstest();
    void insertlogsuccess(int type,QVariantMap map);
    void sendConfirmtoSerVer(int status,QString Taskid);
    void sig_filenocopy(QFileInfoList filenocopy,int num);
private slots:
    void slotcheckfile(QFileInfoList filelist,QString driverid,int num);

private:

    void gettb_files_all(int type_jur,QString userid);
    void gettb_files_video(int type_jur,QString userid);
    void gettb_files_photo(int type_jur,QString userid);
    void gettb_files_log(int type_jur,QString userid);
    void gettb_files_audio(int type_jur,QString userid);

    //QString fileName = "/home/itventi/QT_project/Acquisition_Station/zhifayi.db";
    QString fileName ;
    QStringList realTableName;
    QSqlQuery *sql_query;



};

#endif // MYSQLLITE_H
