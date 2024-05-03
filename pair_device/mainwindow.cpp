#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    hotplugthread = new HotPlugThread();
    hotplugthread->start();
    connect(hotplugthread,SIGNAL(insertZFY(QString)),this,SLOT(slotinsert(QString)));
    connect(hotplugthread,SIGNAL(deleteZFY(QString)),this,SLOT(slotdelete(QString)));
    ui->label->setWordWrap(true);
    ui->comboBox->setItemData(0,QVariant("AS"),Qt::UserRole-1);
    setDisabled();
    initProcess();
    myconfig = new Config();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotinsert(QString insertnum)
{
    qDebug()<<"slot insert:"<<insertnum;
    this->insertNum = insertnum;
    setEnabled();
}
void MainWindow::slotdelete(QString deletenum)
{
    qDebug()<<"slot delete:"<<deletenum;
    this->insertNum = "AA";
    setDisabled();
}

void MainWindow::setEnabled(){

    ui->label->setText(tr("device connect,select one number of the box and click the button to change"));
    ui->btn_confirm->setEnabled(true);
    ui->comboBox->setEnabled(true);
}

void MainWindow::setDisabled(){
    ui->label->setText(tr("the device disconnect,please connect the device first"));
    ui->btn_confirm->setEnabled(false);
    ui->comboBox->setCurrentIndex(0);
    ui->comboBox->setEnabled(false);
}


void MainWindow::on_btn_confirm_clicked()
{
    //QDir::currentPath();

    if(insertNum !="AA"){
        qDebug()<<"set_insertNum"<<insertNum;
        if(ui->comboBox->currentIndex()>=1){
            bool setconfig = setConfigfile(insertNum);
            if(setconfig){
                bool result = setUdevFile(insertNum);
                if(!result){
                    QMessageBox::warning(this, tr("Info"),"change fail,check sudo permission", QMessageBox::Ok );
                }else {
                    QMessageBox::warning(this, tr("Info"),"change success!", QMessageBox::Ok );
                }
            }

        }else {
            qDebug()<<"select one!!";
            QMessageBox::warning(this, tr("Info"),tr("please select one!!"), QMessageBox::Ok );
        }

    }

}


bool MainWindow::setConfigfile(QString insertNum){
    QStringList portlist;
    QList<int> usedlist;
    QString port_old = "port" + QString::number(ui->comboBox->currentIndex()-1);
    QString port_num = insertNum.mid(insertNum.indexOf("-")+1);
    qDebug()<<"port_num"<<port_num;
    int portnum = myconfig->Get("portInfo","port_num").toInt();
    for (int i=0;i<portnum;i++) {
        //qDebug()<<i<<":"<<myconfig->Get("portInfo","port"+QString::number(i)).toString();
        portlist.append(myconfig->Get("portInfo","port"+QString::number(i)).toString());

        if(port_num == myconfig->Get("portInfo","port"+QString::number(i)).toString()){
            //this port was used in other form

            usedlist.append(i);




        }
    }

    if(!usedlist.isEmpty()){
        QMessageBox::StandardButton rb = QMessageBox::warning(this, tr("Info"),
                                                              tr("this port was used in other form,click yes button to "
                                                                                   "change it to select form,click cancel button"
                                                                                   " to cancel"), QMessageBox::Cancel | QMessageBox::Yes);
        if(rb == QMessageBox::Yes)
        {
            for (int i = 0;i<usedlist.count();i++) {
                Config().Set("portInfo","port" + QString::number(usedlist.at(i)),"0.000.0");
                qDebug()<<" exit port num:"<<QString::number(usedlist.at(i));
            }

        }
        if(rb == QMessageBox::Cancel)
        {

            return false;
        }

    }

    Config().Set("portInfo",port_old,port_num);
    qDebug()<<"port_old:"<<port_old<<"port_num:"<<port_num;
    return true;
}

bool MainWindow::setUdevFile(QString insertNum){

    QStringList mylist;
    QList<int> usedport;
    int indexnum = ui->comboBox->currentIndex();
    int udevindex = -1;
    //check file first
    QFile file("/etc/udev/rules.d/00-usb-permissions.rules");
    //QFile file("./123.rules");
    if(!file.exists()){
        if(file.open(QIODevice::WriteOnly)){
            file.write("SUBSYSTEM==\"usb\" , MODE=\"0660\", GROUP=\"plugdev\"\n");
            file.close();

        }else {
            return false;
        }

    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug()<<"read udev file error";

        return false;
    }

    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        qDebug()<<line;
        mylist.append(line);
    }
    file.close();


    //    qDebug()<<mylist.at(0);
    //    mylist[0] = "ABCD";
    //    qDebug()<<mylist.at(0);
    //check combox select num is exist

    if (!mylist.isEmpty()) {

        for (int i=0;i<mylist.count();i++) {

            //ACTION=="add",KERNELS=="3-4.3.3:1.0",SUBSYSTEMS=="usb",DRIVERS=="usb-storage",SYMLINK+="zfy1"
            //ckeck zfy*

            if(mylist.at(i).contains("zfy"+QString::number(indexnum))){
                udevindex = i;
                break;
            }else {
                udevindex = -1;
            }
        }


        for (int i=0;i<mylist.count();i++) {

            //check the port num is use or not

            if(mylist.at(i).contains(insertNum+":1.0")){
                usedport.append(i);
            }
        }
    }



    if(!usedport.isEmpty()){
        for (int i=0;i<usedport.count();i++) {
                mylist[usedport.at(i)].clear();
        }
    }

    //ACTION=="add",KERNELS=="3-4.3.3:1.0",SUBSYSTEMS=="usb",DRIVERS=="usb-storage",SYMLINK+="zfy1"

    //QString writenew = "ACTION==\"add\",KERNELS==\"3-4.3.3:1.0\",SUBSYSTEMS==\"usb\",DRIVERS==\"usb-storage\",SYMLINK+=\"zfy1\"";
    QString writenew = QString("%1%2%3%4%5")
            .arg("ACTION==\"add\",KERNELS==\"").arg(insertNum+":1.0")
            .arg("\",SUBSYSTEMS==\"usb\",DRIVERS==\"usb-storage\",SYMLINK+=\"")
            .arg("zfy"+QString::number(indexnum)).arg("\"\n");

    //qDebug()<<"writenew:"<<writenew;

    //change it or new it
    bool isOK = file.open(QIODevice::WriteOnly);
    if(!isOK){
        return false;
    }
    if(udevindex == -1){
        //add new date
        if (isOK == true){
            //write new
            qDebug()<<"add new line"<<writenew;
            mylist.append(writenew);

            for (int i=0;i<mylist.count();i++) {
                file.write(mylist.at(i).toLatin1());
            }

        }

    }else {
        //change old data
        if (isOK == true){

            qDebug()<<"change old_line"<<udevindex<<" to"<<writenew;
            mylist[udevindex] = writenew;

            for (int i=0;i<mylist.count();i++) {
                file.write(mylist.at(i).toLatin1());
            }
        }

    }
    file.close();
    return true;
}

void MainWindow::on_pushButton_clicked()
{
    //setUdevFile("3-3.4.3");
    restartUdevService();
}

void MainWindow::restartUdevService(){

    qDebug()<<"restart udev service!";


    QString bash_text = "/etc/init.d/udev restart";
    m_proces_bash->write(bash_text.toLocal8Bit() + '\n');
}
void MainWindow::initProcess()
{
    m_proces_bash = new QProcess;
    m_proces_bash->start("bash");
    m_proces_bash->waitForStarted();
    qDebug()<<"initProcess";
    connect(m_proces_bash,SIGNAL(readyReadStandardOutput()),this,SLOT(readBashStandardOutputInfo()));
    connect(m_proces_bash,SIGNAL(readyReadStandardError()),this,SLOT(readBashStandardErrorInfo()));
}
void MainWindow::readBashStandardOutputInfo()
{
    qDebug()<<"readBashStandardOutputInfo";
    QByteArray cmdout = m_proces_bash->readAllStandardOutput();
    if(!cmdout.isEmpty()){
        //ui->textEdit_bashmsg->append(QString::fromLocal8Bit(cmdout));
        qDebug()<<"readBashStandardOutputInfo"<<QString::fromLocal8Bit(cmdout);

        if(QString::fromLocal8Bit(cmdout).contains("Restarting udev (via systemctl): udev.service")){
            //emit changesuccess(ui->comboBox_mylist->currentText().toInt()-1);
            QMessageBox::warning(this, tr("Info"), tr("restart udevservice success"), QMessageBox::Ok );

        }
    }
}

void MainWindow::readBashStandardErrorInfo()
{
    qDebug()<<"readBashStandardErrorInfo";
    QByteArray cmdout = m_proces_bash->readAllStandardError();
    if(!cmdout.isEmpty()){
        qDebug()<<"readBashStandardErrorInfo"<<QString::fromLocal8Bit(cmdout);
    }
}
void MainWindow::on_btn_howtouse_clicked()
{
    //Notice:don't connect two divices at the same time,
    //you should set up one USB port and then delete this device and insert other USB prots to set
    //after setting,you should click the button to restart the service.
    QString notice = tr("don't connect two divices at the same time,"
                        "you should insert one USB port and select one number of the combobox ,click the confirm button to set it"
                        " then delete this device and insert it to other USB prots to set other form"
                        "after set all port,you should click the button to restart the service.");
    QMessageBox::warning(this, tr("Info"), notice, QMessageBox::Ok );

}
