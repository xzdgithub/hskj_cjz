#include "chooseuserdialog.h"
#include "ui_chooseuserdialog.h"

ChooseUserDialog::ChooseUserDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChooseUserDialog)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Choose User"));
    setFixedSize(this->width(),this->height());
    setWindowFlags(Qt::WindowCloseButtonHint);
    ui->comboBox_user->setItemData(0,QVariant("Choose User"),Qt::UserRole-1);
}

ChooseUserDialog::~ChooseUserDialog()
{
    delete ui;
}

void ChooseUserDialog::addUsers(QStringList userlist){
    ui->comboBox_user->addItems(userlist);
}

void ChooseUserDialog::on_btn_chooseuser_clicked()
{

    if(ui->comboBox_user->currentIndex() ==0){
        qDebug()<<"please choose one user!";
    }else {
        QString polno = ui->comboBox_user->currentText();
        polno = polno.mid(0,polno.indexOf(":"));
        qDebug()<<"polno:"<<polno;


        emit towriteUser(polno);
    }

}
