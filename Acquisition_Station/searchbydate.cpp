#include "searchbydate.h"
#include "ui_searchbydate.h"
#include<QTextCharFormat>
#include<QDebug>
SearchByDate::SearchByDate(int type,QString userid,MySqlLite *sqltie,bool videojur,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SearchByDate)
{
    ui->setupUi(this);
    this->type = type;
    this->userid = userid;
    this->videojur =videojur;
    //path = "/mnt/usb/zfy1";

    ui->calendarWidget->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    ui->calendarWidget->setWeekdayTextFormat(Qt::Sunday,ui->calendarWidget->weekdayTextFormat(Qt::Monday));//设置周六日为新的字体格式
    ui->calendarWidget->setWeekdayTextFormat(Qt::Saturday,ui->calendarWidget->weekdayTextFormat(Qt::Monday));


    ui->calendarWidget->setHorizontalHeaderFormat(QCalendarWidget::ShortDayNames);
    QTextCharFormat format ;
    format.setForeground(QColor("white"));
    QFont font;
    font.setPixelSize(20);
    font.setBold(true);
    format.setFont(font);
    ui->calendarWidget->setHeaderTextFormat(format);



    setDateColor(QDate::currentDate(), QColor("red"));

    mysql = sqltie;

    connect(this,SIGNAL(get_tb_files_withdate(int,QString,QDate)),mysql,SLOT(get_tb_files_withdate(int,QString,QDate)));
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
    //ui->calendarWidget->();
    emit get_tb_files_withdate(type,userid,QDate::currentDate());

}

SearchByDate::~SearchByDate()
{
    delete ui;
}
void SearchByDate::setDateColor(const QDate &date, const QColor &color)
{
    QTextCharFormat format = ui->calendarWidget->dateTextFormat(date);
    //format.setForeground(color);
    format.setBackground(color);
    ui->calendarWidget->setDateTextFormat(date, format);
}




void SearchByDate::on_calendarWidget_clicked(const QDate &date)
{


    clearTable();

    emit get_tb_files_withdate(type,userid,date);
}

void SearchByDate::clearTable(){

    ui->tableWidget->setRowCount(0);
}

void SearchByDate::on_pushButton_goback_clicked()
{
    emit goback();
}

void SearchByDate::show_tb_files(QList<FileInfo> file)
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
        check->setTextAlignment(Qt::AlignCenter);
        check->setCheckState(Qt::Unchecked);

        ui->tableWidget->setItem(rowcount,5,check);
        //ui->tableWidget->item(i,0)->setTextAlignment(Qt::AlignCenter);
    }

}

void SearchByDate::on_tableWidget_cellDoubleClicked(int row, int column)
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

void SearchByDate::on_btn_Export_clicked()
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
