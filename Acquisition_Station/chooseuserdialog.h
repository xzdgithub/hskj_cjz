#ifndef CHOOSEUSERDIALOG_H
#define CHOOSEUSERDIALOG_H

#include <QDialog>
#include<qdebug.h>
namespace Ui {
class ChooseUserDialog;
}

class ChooseUserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChooseUserDialog(QWidget *parent = nullptr);
    ~ChooseUserDialog();
signals:
    void towriteUser(QString polno);
private slots:


    void on_btn_chooseuser_clicked();

    void addUsers(QStringList userlist);
private:
    Ui::ChooseUserDialog *ui;
};

#endif // CHOOSEUSERDIALOG_H
