#ifndef MYAVPLAYER_H
#define MYAVPLAYER_H

#include <QWidget>
#include <QtAV>
#include <QtAVWidgets>
#include<Slider.h>
#include<QLabel>
#include<QPushButton>
#include<QProcess>
#include<QMessageBox>
#include<QFileDialog>
#include<QtMath>
#include<QThread>
#include<QUuid>
#include<config.h>
using namespace QtAV;
class MyAVPlayer : public QWidget
{
    Q_OBJECT
public:
    explicit MyAVPlayer(QWidget *parent = nullptr);
    void playvideo(QVariantMap videomap);
signals:
    void videotoinsertsql();
    void cutfinished(QVariantMap map);
    void addsqllog(QVariantMap map);
public slots:
    void seekBySlider(int value);
    void seekBySlider();
    void playPause();
    void updateSlider(qint64 value);
    void updateSlider();
    void updateSliderUnit();
    void onStartPlay();
    void onStopPlay();

    void getstartcuttime();
    void getstopcuttime();

    void tocutvideo();

    void readBashStandardOutputInfo();
    void readBashStandardErrorInfo();
private slots:
    void togetaFrame();
    void togetM4aVoice();
    void onCaptureSaved(const QString &path);
    void workaroundRendererSize();
private:
    GLWidgetRenderer2 renderer;
    AVPlayer *m_player;
    Slider *m_slider;
    int m_unit;
    QLabel *mpCurrent,*mpEnd,*label_starttime,*label_stoptime;
    QPushButton *btn_Pause,*btn_startcut,*btn_stopcut,
    *btn_cutvideo,*btn_screen,*btn_getvoice;

    QProcess *m_proces_bash;
    void initProcess();

    QString playingpath;
    QString shortPath;
    QString newVideoUuid;
    QString userid;
    QString driverid;
    bool videojur;
    QString oldfilename;
    QString virtualpath;
    QString mkMutiDir(const QString path);
};

#endif // MYAVPLAYER_H
