#include "howtosearch.h"
#include "ui_howtosearch.h"

HowToSearch::HowToSearch(int type,QString userid,bool videojur,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HowToSearch)
{
    ui->setupUi(this);
    this->type = type;
    this->userid = userid;
    this->videojur = videojur;
    if(type == 1){

        ui->pushButton_search_by_staff_name->setVisible(false);
    }
}

HowToSearch::~HowToSearch()
{
    delete ui;
}


void HowToSearch::on_pushButton_search_by_files_type_clicked()
{
    emit gotosearch_by_files_type(type,userid,videojur);
}

void HowToSearch::on_pushButton_search_by_date_clicked()
{
    emit gotosearch_by_date(type,userid,videojur);
}

void HowToSearch::on_pushButton_search_by_staff_name_clicked()
{
    emit gotosearch_by_staff_name(type,userid,videojur);
}

void HowToSearch::on_btn_back_clicked()
{
    emit goback();
}
