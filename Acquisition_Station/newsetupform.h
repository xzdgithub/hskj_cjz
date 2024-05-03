#ifndef NEWSETUPFORM_H
#define NEWSETUPFORM_H

#include <QWidget>
#include<dasbuddy.h>
#include<QListWidgetItem>
namespace Ui {
class newSetUpForm;
}

class newSetUpForm : public QWidget
{
    Q_OBJECT

public:
    explicit newSetUpForm(DASBuddy **buddy,QWidget *parent = nullptr);
    ~newSetUpForm();
signals:
    void goback();
private slots:
    void on_listWidget_itemClicked(QListWidgetItem *item);

    void on_btn_goback_clicked();

private:
    Ui::newSetUpForm *ui;
    DASBuddy *dasbuddy[30];
};

#endif // NEWSETUPFORM_H
