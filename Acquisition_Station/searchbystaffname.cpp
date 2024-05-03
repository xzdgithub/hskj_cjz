#include "searchbystaffname.h"
#include "ui_searchbystaffname.h"

SearchByStaffName::SearchByStaffName(int type,QString userid,MySqlLite *sqltie,bool videojur,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SearchByStaffName)
{
    ui->setupUi(this);

    mysql = sqltie;
    this->type = type;
    this->userid = userid;
    this->videojur =videojur;
    connect(this,SIGNAL(get_tb_users()),mysql,SLOT(get_tb_users()));

    connect(this,SIGNAL(get_tb_files_with_userName(QString)),mysql,SLOT(get_tb_files_withuserName(QString)));

    connect(mysql,SIGNAL(gettb_user_success(QList<UserInfo>)),this,SLOT(show_tb_users(QList<UserInfo>)));

    connect(mysql,SIGNAL(gettb_files_success(QList<FileInfo>)),this,SLOT(show_tb_files(QList<FileInfo>)));

    ui->tableWidget->setAlternatingRowColors(true); // 隔行变色
    ui->tableWidget->setPalette(QPalette(QColor(234,239,247))); // 设置隔行变色的颜色  gray灰色

    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setColumnWidth(0, 330);
    ui->tableWidget->setColumnWidth(1, 160);
    ui->tableWidget->setColumnWidth(2, 160);
    ui->tableWidget->setColumnWidth(3, 100);
    ui->tableWidget->setColumnWidth(4, 100);
    ui->tableWidget->setColumnWidth(5, 75);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    emit get_tb_users();
}

SearchByStaffName::~SearchByStaffName()
{
    delete ui;
}

void SearchByStaffName::on_pushButton_goback_clicked()
{
    emit goback();
}

void SearchByStaffName::show_tb_users(QList<UserInfo> userlist)
{
    //qDebug()<<"user_name:"<<userlist.at(0).user_name;
    if(!userlist.isEmpty()){
        for (int i=0;i<userlist.count();i++) {
            ui->comboBox->addItem(userlist.at(i).user_name);
        }

    }

}


//select * from tb_files ,tb_user  where tb_files.user_id=tb_user.id AND tb_user.name = 'Wang'

void SearchByStaffName::on_comboBox_currentIndexChanged(const QString &arg1)
{
    //qDebug()<<"arg1"<<arg1;
    clearTable();
    emit get_tb_files_with_userName(arg1);
}

void SearchByStaffName::show_tb_files(QList<FileInfo> file)
{
    int rowcount = ui->tableWidget->rowCount();
    getfile = file;
    for(int i=file.count()-1;i>=0;i--){
        ui->tableWidget->insertRow(rowcount);
        ui->tableWidget->setItem(rowcount,0,new QTableWidgetItem(file.at(i).file_name));
        //QDateTime time = file_list[i].lastModified();
        ui->tableWidget->setItem(rowcount,1,new QTableWidgetItem(file.at(i).file_date));
        ui->tableWidget->setItem(rowcount,2,new QTableWidgetItem(file.at(i).file_upload_time));
        ui->tableWidget->setItem(rowcount,3,new QTableWidgetItem(file.at(i).file_userid));
        ui->tableWidget->setItem(rowcount,4,new QTableWidgetItem(file.at(i).file_driverid));
        QTableWidgetItem *check = new QTableWidgetItem;
        check->setCheckState(Qt::Unchecked);
        ui->tableWidget->setItem(rowcount,5,check);
    }


}
void SearchByStaffName::clearTable(){

    ui->tableWidget->setRowCount(0);
}

void SearchByStaffName::on_tableWidget_cellDoubleClicked(int row, int column)
{
    if(column == 0){
        qDebug()<<"double clicked"<<"getfile.at(row).filepath()";

        if(!getfile.isEmpty()){
            if(getfile.at(row).file_est =="MP4" || getfile.at(row).file_est =="m4a"){
                qDebug()<<"double click video";
                QVariantMap videomap;
                videomap["searchtype"] = 2;
                videomap["userid"] = getfile.at(row).file_userid;
                videomap["driverid"] = getfile.at(row).file_driverid;
                videomap["videopath"] = getfile.at(row).filepath;
                videomap["shortpath"] = getfile.at(row).file_shortpath;
                videomap["oldfilename"] = getfile.at(row).file_name;
                videomap["videojur"] = videojur;
                //emit gotovideo(getfile.at(row).filepath,2);
                emit gotovideo(videomap);
            }else {
                qDebug()<<"getfile.at(row).filepath"<<getfile.at(row).filepath;
                QDesktopServices::openUrl(QUrl(getfile.at(row).filepath));

            }

        }

    }
}

void SearchByStaffName::on_btn_Export_clicked()
{

    //qDebug()<<check->checkState();
    QString output_path = QFileDialog::getExistingDirectory(this,tr("select path"),"../../");
    qDebug()<<"output_path"<<output_path;
    qDebug()<<"ui->tableWidget->rowCount()"<<ui->tableWidget->rowCount();
    if(output_path.length() >0){
        bool export_result = false;
        for (int i=0;i<ui->tableWidget->rowCount();i++) {

            if(ui->tableWidget->item(i,5)->checkState() == Qt::Checked){
                qDebug()<<"true i"<<getfile.at(i).filepath;

                QString out_file = output_path + QDir::separator() + getfile.at(i).file_name;
                qDebug()<<"out_file"<<out_file;
                QFile::setPermissions(out_file, QFile::WriteOwner);
                export_result = QFile::copy(getfile.at(i).filepath, out_file);
                qDebug()<<"export_result:"<<export_result;

            }

            if(i == ui->tableWidget->rowCount()-1){
                if(export_result){

                    QMessageBox::warning(NULL, "INFO", tr("export finished!"), QMessageBox::Ok );
                }else {
                    QMessageBox::warning(NULL, "INFO", tr("export fail!"), QMessageBox::Ok );
                }
            }

        }



    }
}
