#ifndef SEARCHBYDATE_H
#define SEARCHBYDATE_H

#include <QDialog>
#include<QFileInfoList>
#include<qdir.h>
#include<mysqllite.h>
#include<QFileDialog>
#include<QDesktopServices>
#include<QUrl>
#include<QMessageBox>
namespace Ui {
class SearchByDate;
}

class SearchByDate : public QDialog
{
    Q_OBJECT

public:
    explicit SearchByDate(int type,QString userid,MySqlLite *sqltie,bool videojur,QWidget *parent = nullptr);
    ~SearchByDate();
signals:
    void goback();
    void get_tb_files_withdate(int type,QString userid,QDate date);
    //void gotovideo(QString path,int search_type);
    void gotovideo(QVariantMap videomap);
private slots:
    void on_calendarWidget_clicked(const QDate &date);

    void on_pushButton_goback_clicked();

    void show_tb_files(QList<FileInfo> file);

    void on_tableWidget_cellDoubleClicked(int row, int column);

    void on_btn_Export_clicked();

private:
    Ui::SearchByDate *ui;
    void setDateColor(const QDate &date, const QColor &color);
    void clearTable();

    MySqlLite *mysql;
    QList<FileInfo> getfile;
    int type = -1;
    QString userid;
    bool videojur;
};

#endif // SEARCHBYDATE_H
