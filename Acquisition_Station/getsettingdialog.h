#ifndef GETSETTINGDIALOG_H
#define GETSETTINGDIALOG_H

#include <QDialog>
#include<QDebug>
#include<QMouseEvent>
#include<QTimer>
namespace Ui {
class getSettingDialog;
}

class getSettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit getSettingDialog(QWidget *parent = nullptr);
    ~getSettingDialog();
private slots:
    void getfinish();

    void on_btn_OK_clicked();

    void getsettingfail();

private:
    Ui::getSettingDialog *ui;
    void closeEvent(QCloseEvent *event);
    void changetounfinsh();
    QTimer *starttimer;
};

#endif // GETSETTINGDIALOG_H
