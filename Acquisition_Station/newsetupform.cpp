#include "newsetupform.h"
#include "ui_newsetupform.h"

newSetUpForm::newSetUpForm(DASBuddy **buddy,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::newSetUpForm)
{
    ui->setupUi(this);
    for (int i=0;i<30;i++) {
        dasbuddy[i] = buddy[i];

        QListWidgetItem *newItem = new QListWidgetItem();

        ui->listWidget->insertItem(i,newItem);

        ui->listWidget->setItemWidget(newItem, dasbuddy[i]);
    }

}

newSetUpForm::~newSetUpForm()
{
    delete ui;
}

void newSetUpForm::on_listWidget_itemClicked(QListWidgetItem *item)
{
    qDebug()<<item->listWidget()->currentRow();
}

void newSetUpForm::on_btn_goback_clicked()
{
    emit goback();
}
