#ifndef SEARCHBYTYPE_H
#define SEARCHBYTYPE_H

#include <QDialog>
#include <qdir.h>
#include<QDebug>
#include<mysqllite.h>
#include<QDesktopServices>
#include<QUrl>
#include<QFileDialog>
#include<QMessageBox>
namespace Ui {
class SearchByType;
}

class SearchByType : public QDialog
{
    Q_OBJECT

public:
    explicit SearchByType(int type,QString userid,MySqlLite *sqltie,bool videojur,QWidget *parent = nullptr);
    ~SearchByType();
signals:
    void goback();
    //void gotovideo(QString path,int search_type);
    void gotovideo(QVariantMap videomap);
    //0 代表all 1代表video 2代表photo 3代表log
    void get_tb_files_type(int type_jur,QString userid,int type);
private slots:
    void on_pushButton_video_clicked();

    void on_pushButton_goback_clicked();

    void on_pushButton_searchall_clicked();

    void on_tableWidget_cellClicked(int row, int column);

    void on_tableWidget_cellDoubleClicked(int row, int column);

    void show_tb_files(QList<FileInfo>);

    void on_pushButton_photo_clicked();

    void on_pushButton_Log_clicked();

    void on_btn_Export_clicked();

    void on_btn_Audio_clicked();

private:
    Ui::SearchByType *ui;

    QFileInfoList GetFileList(QString path,QStringList filters);
    void clearTable();

    MySqlLite *mysql;
    QList<FileInfo> getfile;
    int type_jur = -1;
    QString userid;
    bool videojur;


};

#endif // SEARCHBYTYPE_H
