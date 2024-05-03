#ifndef INSERTZFYDIALOG_H
#define INSERTZFYDIALOG_H

#include <QDialog>
#include<QDebug>
#include<QMouseEvent>
#include<QTimer>
namespace Ui {
class InsertZFYDialog;
}

class InsertZFYDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InsertZFYDialog(int zfynum,QWidget *parent = nullptr);
    ~InsertZFYDialog();
signals:
    void sigreconnect(int zfynum);
private slots:
    void on_btn_Ok_clicked();
    void connectfinish();
    void connectfail();
private:
    Ui::InsertZFYDialog *ui;
    void closeEvent(QCloseEvent *event);
    void changetounfinsh();
    QTimer *starttimer;
    int connecttime;
    int zfynum;
};

#endif // INSERTZFYDIALOG_H
