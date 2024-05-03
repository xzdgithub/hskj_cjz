#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<hotplugthread.h>
#include<config.h>
#include<QMessageBox>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void slotinsert(QString insertnum);
    void on_btn_confirm_clicked();

    void slotdelete(QString deletenum);
    void on_pushButton_clicked();

    void readBashStandardOutputInfo();
    void readBashStandardErrorInfo();
    void on_btn_howtouse_clicked();

private:
    Ui::MainWindow *ui;
    HotPlugThread *hotplugthread;
    void setEnabled();
    void setDisabled();
    QString insertNum = "AA";
    bool setConfigfile(QString insertNum);
    bool setUdevFile(QString insertNum);
    void restartUdevService();
    void initProcess();
    QProcess *m_proces_bash;
    Config *myconfig;
};

#endif // MAINWINDOW_H
