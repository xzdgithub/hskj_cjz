#ifndef SEARCHBYSTAFFNAME_H
#define SEARCHBYSTAFFNAME_H

#include <QWidget>
#include<mysqllite.h>
#include<QFileDialog>
#include<QDesktopServices>
#include<QUrl>
#include<QMessageBox>
namespace Ui {
class SearchByStaffName;
}

class SearchByStaffName : public QWidget
{
    Q_OBJECT

public:
    explicit SearchByStaffName(int type,QString userid,MySqlLite *sqltie,bool videojur,QWidget *parent = nullptr);
    ~SearchByStaffName();

signals:
    void goback();
    void get_tb_users();
    void get_tb_files_with_userName(QString username);
    //void gotovideo(QString path,int search_type);
    void gotovideo(QVariantMap videomap);
private slots:
    void on_pushButton_goback_clicked();
    void show_tb_users(QList<UserInfo> userlist);
    void on_comboBox_currentIndexChanged(const QString &arg1);

    void show_tb_files(QList<FileInfo>);

    void on_tableWidget_cellDoubleClicked(int row, int column);

    void on_btn_Export_clicked();

private:
    Ui::SearchByStaffName *ui;
    MySqlLite *mysql;
    void clearTable();
    QList<FileInfo> getfile;
    int type = -1;
    QString userid;
    bool videojur;
};

#endif // SEARCHBYSTAFFNAME_H
