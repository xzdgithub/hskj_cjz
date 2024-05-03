#include "videoplayerform.h"
#include "ui_videoplayerform.h"
VideoPlayerForm::VideoPlayerForm(MySqlLite *sqltie,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VideoPlayerForm)
{
    ui->setupUi(this);

    //connect(this,SIGNAL(gosearch()),ui->widget,SLOT(playvideo()));

    //ui->widget->playvideo("test.mp4");
    //userid
    this->mysql = sqltie;
    connect(this,SIGNAL(sigtoWriteCutSql(QVariantMap)),mysql,SLOT(WriteCutSql(QVariantMap)));
    connect(this,SIGNAL(sigtoaddsqllog(QVariantMap)),mysql,SLOT(insert_User_Log(QVariantMap)));
    connect(this->ui->widget,SIGNAL(cutfinished(QVariantMap)),this,SLOT(toWriteCutSql(QVariantMap)));
    connect(this->ui->widget,SIGNAL(addsqllog(QVariantMap)),this,SLOT(toaddsqllog(QVariantMap)));

}

VideoPlayerForm::~VideoPlayerForm()
{
    qDebug()<<"~VideoPlayerForm";
    delete ui;

}
void VideoPlayerForm::playvideo(QVariantMap videomap){
    //QString videopath = videomap.value("videopath").toString();
    int search_type = videomap.value("searchtype").toInt();

    ui->widget->playvideo(videomap);
    this->search_type = search_type;
}

void VideoPlayerForm::on_pushButton_goback_clicked()
{
    emit goback(search_type);
    //this->deleteLater();
}
void VideoPlayerForm::toWriteCutSql(QVariantMap cutmap){

    qDebug()<<"video cut!!!!!!!!!!!!!!!!!!!!!";

    emit sigtoWriteCutSql(cutmap);
}
void VideoPlayerForm::toaddsqllog(QVariantMap logmap){
    emit sigtoaddsqllog(logmap);
}
