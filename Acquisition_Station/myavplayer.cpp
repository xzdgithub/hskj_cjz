#include "myavplayer.h"
#include<QVBoxLayout>
#include<QMessageBox>

MyAVPlayer::MyAVPlayer(QWidget *parent) : QWidget(parent)
{


    m_unit = 1000;
    m_player = new AVPlayer(this);
    m_slider = new Slider(this);

    btn_Pause = new QPushButton(this);
    btn_Pause->setText("Play/Pause");
    btn_Pause->setStyleSheet("QPushButton"
                             "{background-color:rgba(91,155,213,100%);"
                             "color: white;"
                             "}"
                             "QPushButton:pressed{"
                             "background-color:rgb(85, 170, 255);"
                             "}");
    btn_screen = new QPushButton(this);
    btn_screen->setText("Screen picture");
    btn_screen->setStyleSheet("QPushButton"
                             "{background-color:rgba(91,155,213,100%);"
                             "color: white;"
                             "}"
                             "QPushButton:pressed{"
                             "background-color:rgb(85, 170, 255);"
                             "}");


    btn_getvoice = new QPushButton(this);
    btn_getvoice->setText("getM4a");
    btn_getvoice->setStyleSheet("QPushButton"
                             "{background-color:rgba(91,155,213,100%);"
                             "color: white;"
                             "}"
                             "QPushButton:pressed{"
                             "background-color:rgb(85, 170, 255);"
                             "}");

    mpCurrent = new QLabel(this);
    mpCurrent->setText(QString::fromLatin1("00:00:00"));
    //mpCurrent->setText(QTime(0, 0, 0).addMSecs(pos).toString(QString::fromLatin1("HH:mm:ss")));
    mpEnd = new QLabel(this);
    mpEnd->setText(QString::fromLatin1("00:00:00"));
    //mpEnd->setText(QTime(0, 0, 0).addMSecs(mpPlayer->mediaStopPosition()).toString(QString::fromLatin1("HH:mm:ss")));
    QVBoxLayout *vl = new QVBoxLayout();
    setLayout(vl);

    if (!renderer.widget()) {
        QMessageBox::warning(0, QString::fromLatin1("QtAV error"), tr("Can not create video renderer"));
        return;
    }
    //renderer.show();
    m_player->setRenderer(&renderer);
    vl->addWidget(renderer.widget());

    m_slider->setOrientation(Qt::Horizontal);
    connect(m_slider, SIGNAL(sliderMoved(int)), SLOT(seekBySlider(int)));
    connect(m_slider, SIGNAL(sliderPressed()), SLOT(seekBySlider()));
    connect(m_player, SIGNAL(positionChanged(qint64)), SLOT(updateSlider(qint64)));
    connect(m_player, SIGNAL(started()), SLOT(updateSlider()));
    connect(m_player, SIGNAL(notifyIntervalChanged()), SLOT(updateSliderUnit()));

    connect(m_player, SIGNAL(started()), this, SLOT(onStartPlay()));
    connect(m_player, SIGNAL(stopped()), this, SLOT(onStopPlay()));

    connect(btn_Pause, SIGNAL(clicked()), this, SLOT(playPause()));
    connect(btn_screen, SIGNAL(clicked()), this, SLOT(togetaFrame()));
    connect(m_player->videoCapture(), SIGNAL(saved(QString)), SLOT(onCaptureSaved(QString)));
    connect(btn_getvoice, SIGNAL(clicked()), this, SLOT(togetM4aVoice()));
    QHBoxLayout *hlay = new QHBoxLayout();

    //hlay->setContentsMargins(0,0,0,0);
    mpCurrent->setStyleSheet(QString("border-image:url(:/image/circle_lite.png);"));
    mpCurrent->setFixedSize(60,30);
    mpEnd->setStyleSheet(QString("border-image:url(:/image/circle_lite.png);"));
    mpEnd->setFixedSize(60,30);


    hlay->addWidget(mpCurrent);
    hlay->addWidget(m_slider);
    hlay->addWidget(mpEnd);
    hlay->addWidget(btn_Pause);
    hlay->addWidget(btn_screen);
    hlay->addWidget(btn_getvoice);

    vl->addItem(hlay);



    QHBoxLayout *hlay2 = new QHBoxLayout();

    btn_startcut = new QPushButton(this);
    btn_startcut->setFixedSize(100,30);
    btn_startcut->setText(tr("cut time start"));

    btn_startcut->setStyleSheet("QPushButton"
                             "{background-color:rgba(91,155,213,100%);"
                             "color: white;"
                             "}"
                             "QPushButton:pressed{"
                             "background-color:rgb(85, 170, 255);"
                             "}");

    connect(btn_startcut, SIGNAL(clicked()), this, SLOT(getstartcuttime()));


    btn_stopcut = new QPushButton(this);
    btn_stopcut->setFixedSize(100,30);
    btn_stopcut->setText(tr("cut time end"));

    btn_stopcut->setStyleSheet("QPushButton"
                             "{background-color:rgba(91,155,213,100%);"
                             "color: white;"
                             "}"
                             "QPushButton:pressed{"
                             "background-color:rgb(85, 170, 255);"
                             "}");

    connect(btn_stopcut, SIGNAL(clicked()), this, SLOT(getstopcuttime()));


    btn_cutvideo = new QPushButton(this);
    btn_cutvideo->setFixedSize(100,30);
    btn_cutvideo->setText(tr("start cut"));

    btn_cutvideo->setStyleSheet("QPushButton"
                             "{background-color:rgba(91,155,213,100%);"
                             "color: white;"
                             "}"
                             "QPushButton:pressed{"
                             "background-color:rgb(85, 170, 255);"
                             "}");


    connect(btn_cutvideo, SIGNAL(clicked()), this, SLOT(tocutvideo()));
    //connect(btn_cutvideo, SIGNAL(clicked()), this, SLOT(workaroundRendererSize()));

    label_starttime = new QLabel(this);
    label_starttime->setText(QString::fromLatin1("00:00:00"));
    label_starttime->setFixedSize(60,30);

    label_stoptime = new QLabel(this);
    label_stoptime->setText(QString::fromLatin1("00:00:00"));
    label_stoptime->setFixedSize(60,30);

    hlay2->addWidget(btn_startcut);
    hlay2->addWidget(label_starttime);
    hlay2->addWidget(btn_stopcut);
    hlay2->addWidget(label_stoptime);
    hlay2->addWidget(btn_cutvideo);


    vl->addItem(hlay2);
    //handleFullscreenChange();
    //workaroundRendererSize();
}
void MyAVPlayer::seekBySlider(int value)
{
    if (!m_player->isPlaying())
        return;
    m_player->seek(qint64(value*m_unit));
}

void MyAVPlayer::seekBySlider()
{
    seekBySlider(m_slider->value());
}

void MyAVPlayer::playPause()
{
    if (!m_player->isPlaying()) {
        m_player->play();
        return;
    }

    m_player->pause(!m_player->isPaused());
}

void MyAVPlayer::updateSlider(qint64 value)
{
    m_slider->setRange(0, int(m_player->duration()/m_unit));
    m_slider->setValue(int(value/m_unit));
    mpCurrent->setText(QTime(0, 0, 0).addMSecs(value).toString(QString::fromLatin1("HH:mm:ss")));
}

void MyAVPlayer::updateSlider()
{
    updateSlider(m_player->position());
}

void MyAVPlayer::updateSliderUnit()
{
    m_unit = m_player->notifyInterval();
    updateSlider();
}
void MyAVPlayer::playvideo(QVariantMap videomap){
    QString videopath = videomap.value("videopath").toString();
    qDebug()<<"video path:"<<videopath;
    playingpath = videopath;
    //shortPath = videomap.value("shortpath").toString();
    userid = videomap.value("userid").toString();
    driverid = videomap.value("driverid").toString();
    videojur = videomap.value("videojur").toBool();
    oldfilename = videomap.value("oldfilename").toString();
    qDebug()<<"myvideo videojur:"<<videojur;

    m_player->play(videopath);
}
void MyAVPlayer::onStartPlay()
{
    //    m_slider->setMinimum(m_player->mediaStartPosition());
    //    m_slider->setMaximum(m_player->mediaStopPosition());
    //    m_slider->setValue(0);
    m_slider->setEnabled(m_player->isSeekable());
    mpEnd->setText(QTime(0, 0, 0).addMSecs(m_player->mediaStopPosition()).toString(QString::fromLatin1("HH:mm:ss")));

}
void MyAVPlayer::onStopPlay(){

    m_slider->setValue(0);
    m_slider->setDisabled(true);
    mpCurrent->setText(QString::fromLatin1("00:00:00"));
    mpEnd->setText(QString::fromLatin1("00:00:00"));





}

void MyAVPlayer::getstartcuttime()
{
    label_starttime->setText(QTime(0, 0, 0).addMSecs(m_player->position()).toString(QString::fromLatin1("HH:mm:ss")));

}

void MyAVPlayer::getstopcuttime()
{
    label_stoptime->setText(QTime(0, 0, 0).addMSecs(m_player->position()).toString(QString::fromLatin1("HH:mm:ss")));
}

void MyAVPlayer::tocutvideo()
{

    if(!videojur){
        QMessageBox::warning(NULL, tr("INFO"), tr("you don not have permission to cut video!"), QMessageBox::Ok );
    }else {
        initProcess();


        //QString output_path = QFileDialog::getSaveFileName(this, "save","../", "video(*.MP4)");

        //ffmpeg -ss 10 -t 15 -accurate_seek -i test.mp4 -codec copy cut.mp4
        // /usr/local/ffmpeg/bin/ffmpeg

        //QString bash_text = "/usr/local/ffmpeg/bin/ffmpeg";

        //label_starttime //label_stoptime
        //QTime(0, 0, 0).addMSecs(label_starttime->text().to);

        QString dirpath1 = QString("%1").arg(Config().Get("path","copypath").toString());
        virtualpath = QString("%1").arg(Config().Get("path","virtualpath").toString());
        QDateTime uploadDate = QDateTime::currentDateTime();
        QString uploadDatestr = uploadDate.toString("yyyy-MM-dd");
        shortPath = dirpath1 + virtualpath + "/"+uploadDatestr +"/"+userid +"/";
        qDebug()<<"shortpath :"<<shortPath;

        newVideoUuid = QUuid::createUuid().toString().remove("{").remove("}");
        QString output_path = shortPath + newVideoUuid +".MP4";
        qDebug()<<"output_path:"<<output_path;


        QString mkmutidir = mkMutiDir(shortPath);
        qDebug()<<"mkmutidir"<<mkmutidir;


        QTime starttime = QTime::fromString(label_starttime->text(),"HH:mm:ss");
        QTime endtime = QTime::fromString(label_stoptime->text(),"HH:mm:ss");
        qDebug()<<"starttime-endtime:"<<starttime.secsTo(endtime);

        QString bash_text = QString("%1 %2 %3 %4 %5 %6 %7 %8 %9")
                .arg("./ffmpeg")
                .arg("-ss").arg(label_starttime->text())
                .arg("-t").arg(qAbs(starttime.secsTo(endtime)))
                .arg("-accurate_seek -i")
                .arg(playingpath)
                .arg("-codec copy")
                .arg(output_path);

        qDebug()<<"bash_text:"<<bash_text;
        m_proces_bash->write(bash_text.toLocal8Bit() + '\n');

        //emit videotoinsertsql();



        //m_proces_bash->close();
    }

}

void MyAVPlayer::togetM4aVoice(){
    if(!videojur){
        QMessageBox::warning(NULL, tr("INFO"), tr("you don not have permission to get audio!"), QMessageBox::Ok );
    }else {

        initProcess();


        //QString output_path = QFileDialog::getSaveFileName(this, "save","../", "voice(*.m4a)");


        QString dirpath1 = QString("%1").arg(Config().Get("path","copypath").toString());
        virtualpath = QString("%1").arg(Config().Get("path","virtualpath").toString());
        QDateTime uploadDate = QDateTime::currentDateTime();
        QString uploadDatestr = uploadDate.toString("yyyy-MM-dd");
        shortPath = dirpath1 + virtualpath + "/"+uploadDatestr +"/"+userid +"/";
        qDebug()<<"shortpath :"<<shortPath;

        newVideoUuid = QUuid::createUuid().toString().remove("{").remove("}");
        QString output_path = shortPath + newVideoUuid +".m4a";
        qDebug()<<"output_path:"<<output_path;

        QString mkmutidir = mkMutiDir(shortPath);
        qDebug()<<"mkmutidir"<<mkmutidir;

        //ffmpeg -i input.flv -vn -codec copy out.m4a
        QString bash_text = QString("%1 %2 %3 %4 %5 %6")
                .arg("./ffmpeg")
                .arg("-i").arg(playingpath)
                .arg("-vn")
                .arg("-codec copy")
                .arg(output_path);

        qDebug()<<"bash_text:"<<bash_text;
        m_proces_bash->write(bash_text.toLocal8Bit() + '\n');
    }


}
void MyAVPlayer::togetaFrame(){


    if(!videojur){
        QMessageBox::warning(NULL, tr("INFO"), tr("you don not have permission to screen the photo!"), QMessageBox::Ok );
    }else {

        //QString output_path = QFileDialog::getSaveFileName(this, "save","../", "picture(*.png)");

        QString dirpath1 = QString("%1").arg(Config().Get("path","copypath").toString());
        virtualpath = QString("%1").arg(Config().Get("path","virtualpath").toString());
        QDateTime uploadDate = QDateTime::currentDateTime();
        QString uploadDatestr = uploadDate.toString("yyyy-MM-dd");
        shortPath = dirpath1 + virtualpath + "/"+uploadDatestr +"/"+userid ;
        qDebug()<<"shortpath :"<<shortPath;

        newVideoUuid = QUuid::createUuid().toString().remove("{").remove("}");


        QString mkmutidir = mkMutiDir(shortPath);
        qDebug()<<"mkmutidir"<<mkmutidir;

        //QFileInfo file(output_path);
        //qDebug()<<"filepath"<<file.fileName();
        //qDebug()<<"output_path"<<file.path();
        m_player->videoCapture()->setCaptureDir(shortPath);
        //m_player->videoCapture()->setQuality(100);
        //m_player->videoCapture()->setAutoSave(false);
        //m_player->videoCapture()->setOriginalFormat(true);
        //m_player->videoCapture()->setSaveFormat("PNG");
        m_player->videoCapture()->setCaptureName(newVideoUuid);
        //m_player->videoCapture()->setObjectName(file.completeBaseName());
        m_player->videoCapture()->capture();
    }



}

void MyAVPlayer::onCaptureSaved(const QString &path)
{

    QMessageBox::warning(NULL, tr("INFO"), tr("save a photo success!"), QMessageBox::Ok );
    qDebug()<<(tr("saved to!!!!!!!!!!!!!!!!!!!: ") + path);

    //QThread::msleep(500);
    QFileInfo file(path);
    qDebug()<<"file.size!!!!!!!!!!!!!!!!!!!: "<<file.size();

    QVariantMap map;
    map["id"] = file.completeBaseName();
    map["file_name"] = file.fileName();
    map["user_id"] = userid;
    map["driver_id"] = driverid;
    map["create_time"] = file.created();
    map["upload_time"] = QDateTime::currentDateTime();
    map["file_size"] = file.size();
    map["file_type"] = 1;
    map["file_path"] = shortPath+"/";
    map["file_est"] = "png";
    map["coordinates"] = QString("");
    map["virtualpath"] = virtualpath;

    emit cutfinished(map);

    QVariantMap logmap;
    QString struuid = QUuid::createUuid().toString().remove("{").remove("}");
    logmap["TaskId"] = struuid;
    logmap["OperaType"] = 4;
    logmap["OperaContent"] = QString("%1%2%3").arg("oldfilename:").arg(oldfilename).arg(" to get video photo");
    logmap["Time"] = QDateTime::currentDateTime();
    logmap["DevNo"] = driverid;
    logmap["PolNo"] = userid;
    emit addsqllog(logmap);

}


void MyAVPlayer::initProcess()
{
    m_proces_bash = new QProcess;
    m_proces_bash->start("bash");
    m_proces_bash->waitForStarted();
    qDebug()<<"initProcess";
    connect(m_proces_bash,SIGNAL(readyReadStandardOutput()),this,SLOT(readBashStandardOutputInfo()));
    connect(m_proces_bash,SIGNAL(readyReadStandardError()),this,SLOT(readBashStandardErrorInfo()));

    //m_proces_bash->close()
}


void MyAVPlayer::readBashStandardOutputInfo()
{
    qDebug()<<"readBashStandardOutputInfo";
    QByteArray cmdout = m_proces_bash->readAllStandardOutput();
    if(!cmdout.isEmpty()){
        //ui->textEdit_bashmsg->append(QString::fromLocal8Bit(cmdout));
        qDebug()<<"readBashStandardOutputInfo"<<QString::fromLocal8Bit(cmdout);



    }
}

void MyAVPlayer::readBashStandardErrorInfo()
{
    qDebug()<<"readBashStandardErrorInfo";
    QByteArray cmdout = m_proces_bash->readAllStandardError();
    if(!cmdout.isEmpty()){
        //ui->textEdit_bashmsg->append(QString::fromLocal8Bit(cmdout));
        qDebug()<<"readBashStandardErrorInfo"<<QString::fromLocal8Bit(cmdout);
        if(QString::fromLocal8Bit(cmdout).contains("already exists. Overwrite? [y/N]")){

            QString bash_text ="y";
            m_proces_bash->write(bash_text.toLocal8Bit() + '\n');
        }

        if(QString::fromLocal8Bit(cmdout).contains("frame=") &&QString::fromLocal8Bit(cmdout).contains("muxing overhead:")){

            qDebug()<<"cut finish!"<<newVideoUuid;

            QMessageBox::warning(NULL, tr("INFO"), tr("cut the video success!"), QMessageBox::Ok );

            QFileInfo file(shortPath + newVideoUuid +".MP4");
            qDebug()<<"cutfilesize:"<<file.size();
            QVariantMap map;
            map["id"] = newVideoUuid;
            map["file_name"] = newVideoUuid +".MP4";
            map["user_id"] = userid;
            map["driver_id"] = driverid;
            map["create_time"] = file.created();
            map["upload_time"] = QDateTime::currentDateTime();
            map["file_size"] = file.size();
            map["file_type"] = 2;
            map["file_path"] = shortPath;
            map["file_est"] = "MP4";
            map["coordinates"] = QString("");
            map["virtualpath"] = virtualpath;
            emit cutfinished(map);


            QVariantMap logmap;
            QString struuid = QUuid::createUuid().toString().remove("{").remove("}");
            logmap["TaskId"] = struuid;
            logmap["OperaType"] = 4;
            logmap["OperaContent"] = QString("%1%2%3").arg("oldfilename:").arg(oldfilename).arg(" to cut video");
            logmap["Time"] = QDateTime::currentDateTime();
            logmap["DevNo"] = QString("");
            logmap["PolNo"] = userid;
            emit addsqllog(logmap);



        }
        if(!QString::fromLocal8Bit(cmdout).contains("frame=") &&QString::fromLocal8Bit(cmdout).contains("muxing overhead:")){

            qDebug()<<"get sound finish!";

            QMessageBox::warning(NULL, tr("INFO"), tr("get the audio success!"), QMessageBox::Ok );

            QFileInfo file(shortPath + newVideoUuid +".m4a");
            qDebug()<<"cutfilesize:"<<file.size();
            QVariantMap map;
            map["id"] = newVideoUuid;
            map["file_name"] = newVideoUuid +".m4a";
            map["user_id"] = userid;
            map["driver_id"] = driverid;
            map["create_time"] = file.created();
            map["upload_time"] = QDateTime::currentDateTime();
            map["file_size"] = file.size();
            map["file_type"] = 3;
            map["file_path"] = shortPath;
            map["file_est"] = "m4a";
            map["coordinates"] = QString("");
            map["virtualpath"] = virtualpath;
            emit cutfinished(map);

            QVariantMap logmap;
            QString struuid = QUuid::createUuid().toString().remove("{").remove("}");
            logmap["TaskId"] = struuid;
            logmap["OperaType"] = 4;
            logmap["OperaContent"] = QString("%1%2%3").arg("oldfilename:").arg(oldfilename).arg(" to get video audio");
            logmap["Time"] = QDateTime::currentDateTime();
            logmap["DevNo"] = QString("");
            logmap["PolNo"] = userid;
            emit addsqllog(logmap);
        }

    }
}

void MyAVPlayer::workaroundRendererSize()
{
    qDebug()<<"resize";
    if (!&renderer)
        return;
    QSize s = rect().size();
    //resize(QSize(s.width()-1, s.height()-1));
    //resize(s); //window resize to fullscreen size will create another fullScreenChange event
    renderer.widget()->resize(QSize(s.width()+1, s.height()+1));
    renderer.widget()->resize(s);
}

QString MyAVPlayer::mkMutiDir(const QString path){
    QDir dir(path);
    if ( dir.exists(path)){
        return path;
    }
    QString parentDir = mkMutiDir(path.mid(0,path.lastIndexOf('/')));
    QString dirname = path.mid(path.lastIndexOf('/') + 1);
    QDir parentPath(parentDir);
    if ( !dirname.isEmpty() )
        parentPath.mkpath(dirname);
    return parentDir + "/" + dirname;
}
