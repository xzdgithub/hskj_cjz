#ifndef VIDEOPLAYERFORM_H
#define VIDEOPLAYERFORM_H

#include <QWidget>
#include<QVariantMap>
#include<mysqllite.h>
namespace Ui {
class VideoPlayerForm;
}

class VideoPlayerForm : public QWidget
{
    Q_OBJECT

public:
    explicit VideoPlayerForm(MySqlLite *sqltie,QWidget *parent = nullptr);
    ~VideoPlayerForm();
signals:
    void goback(int search_type);
    void sigtoWriteCutSql(QVariantMap cutmap);
    void sigtoaddsqllog(QVariantMap logmap);
private slots:
    void playvideo(QVariantMap videomap);
    void on_pushButton_goback_clicked();

    void toWriteCutSql(QVariantMap cutmap);
    void toaddsqllog(QVariantMap logmap);
private:
    Ui::VideoPlayerForm *ui;
    //2:search by file type 3:search by date 4:search by staffname
    int search_type = -1;
    QString userid;
    MySqlLite *mysql;
};
#endif // VIDEOPLAYERFORM_H
