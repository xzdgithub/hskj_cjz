#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowFlags(Qt::WindowCloseButtonHint);
    setFixedSize(this->width(),this->height());
    //qDebug()<<"MainWindow_username:"<<username;
    //getconfig = new Config();
    //ui->pushButton->setVisible(false);
    //ui->comboBox->setVisible(false);
    ui->label_time->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));

    updatetime = new QTimer(this);
    connect(updatetime, SIGNAL(timeout()), this, SLOT(updatelabeltime()));
    updatetime->start(1000);


    initActiveMq();

    AcStation_starttime = QDateTime::currentDateTime();
    initProcess();


    bool autodeletecopyfile = Config().Get("path","autodeletecopyfile").toBool();
    for (int i=0;i<30;i++) {
        dasbuddy[i] = new DASBuddy(autodeletecopyfile,this);


    }


    timer_uploadDiskSize = new QTimer(this);
    connect(timer_uploadDiskSize, SIGNAL(timeout()), this, SLOT(uploaddisksize()));
    timer_uploadDiskSize->start(600000);


    timer_updateformlabel = new QTimer(this);
    connect(timer_updateformlabel, SIGNAL(timeout()), this, SLOT(getnetworkanddiskmsg()));
    timer_updateformlabel->start(60000);

    storage = QStorageInfo::root();
    QString path = Config().Get("path","copypath").toString();
    pingip = Config().Get("server","serverIp").toString();

    pingip = pingip.mid(pingip.indexOf("//")+2);
    pingip = pingip.mid(0,pingip.indexOf(":"));
    qDebug()<<"pingip"<<pingip;
    storage.setPath(path);
    totaldisksize = QString::number(storage.bytesTotal()/1024.00/1024.00/1024.00,'f',2).toDouble();
    //storage.refresh();  //获得最新磁盘信息
    //freedisksize = QString::number(storage.bytesAvailable()/1024.00/1024.00/1024.00,'f',2).toDouble();

    //connectads = new ConnectADS();
    //    getusb = new GetUsbInfo(&t);

    //    for (int i=0;i<ADSDeviceslist.count();i++) {
    //        qDebug()<<ADSDeviceslist.at(i).path;
    //    }

    //    int port_num = getconfig->Get("portInfo","port_num").toInt();
    //    qDebug() << port_num;
    //    for (int i=0;i<port_num;i++) {
    //        QString port_no = "port" + QString::number(i);
    //        usbhublist.append(getconfig->Get("portInfo",port_no).toString());
    //        qDebug()<<"port"<<i<<":"<<usbhublist.at(i);
    //    }

    MyHostIPV4Address = getHostIPV4Address().toString();

    //ui->label_hostip->settext(MyHostIPV4Address);
    mysqlite = new MySqlLite(this);

    connect(mysqlite,SIGNAL(insertsuccess(QVariant)),this,SLOT(doinsertActiveMq(QVariant)));
    connect(this,SIGNAL(toupdateSql(QVariantMap)),mysqlite,SLOT(toupdateFromMq(QVariantMap)));
    connect(this,SIGNAL(todeleteSql(QVariantMap)),mysqlite,SLOT(todeleteFromMq(QVariantMap)));

    connect(mysqlite,SIGNAL(sendConfirmtoSerVer(int,QString)),this,SLOT(tosendConfirmtoSerVer(int,QString)));

    connect(this,SIGNAL(toinsert_AcStation_log(QVariantMap)),mysqlite,SLOT(insert_AcStation_Log(QVariantMap)));

    connect(mysqlite,SIGNAL(insertlogsuccess(int,QVariantMap)),this,SLOT(doMqinsertLog(int,QVariantMap)));

    connect(this,SIGNAL(toinsert_user_Log(QVariantMap)),mysqlite,SLOT(insert_User_Log(QVariantMap)));



    zfysetupthread = new QThread();
    zfysetting = new ZFYSetting(dasbuddy);
    zfysetting->moveToThread(zfysetupthread);
    zfysetupthread->start();


    form =new MainForm(dasbuddy,mysqlite,zfysetting,this);
    //form->setZFYSettingThread(zfysetting);
    form->setIPV4Label(MyHostIPV4Address);
    ui->stackedWidget->addWidget(form);

    ui->stackedWidget->setCurrentWidget(form);
    firstIndex = ui->stackedWidget->currentIndex();
    qDebug()<<"firstIndex:"<<firstIndex;
    qDebug()<<"currentWidget:"<<ui->stackedWidget->currentWidget();



    //connect(connectads,SIGNAL(passwordcorrect()),this,SLOT(changetoUdisk()));

    connect(form,SIGNAL(gosearch(int,QString,bool)),this,SLOT(gotoSearchForm(int,QString,bool)));
    connect(form,SIGNAL(gosetup(QList<int>)),this,SLOT(gotoSetupForm(QList<int>)));

    connect(this,SIGNAL(updateDiskSize(double,bool)),form,SLOT(updatedisksizelabel(double,bool)));

    connect(form,SIGNAL(gonewsetup()),this,SLOT(gotonewSetupForm()));

    connect(form,SIGNAL(toopenbroswer()),this,SLOT(openbroswer()));
    //connect(this,SIGNAL(sendConnectPortNumber(QList<int>)),form,SLOT(getZFYConnectPortNumber(QList<int>)));

    //    QObject::connect(&m_usb_info, &QUsbInfo::deviceInserted,
    //                     this, &MainWindow::onDevInserted);
    //    QObject::connect(&m_usb_info, &QUsbInfo::deviceRemoved,
    //                     this, &MainWindow::onDevRemoved);



    //getusb->initUsbDevices();
    //getusb->showAllUsbDevices();
    //checkHubConnect();







    //connectads->connectdevice();

    //    if(this->isActiveWindow()){
    //        qDebug()<<"active";


    //    }







    qDebug()<<"AcStation_starttime"<<AcStation_starttime;

    QString struuid = QUuid::createUuid().toString().remove("{").remove("}");
    QVariantMap map;
    map["TaskId"] = struuid;
    map["OperaType"] = 3;
    map["OperaContent"] = "Acquisition Station open time";
    map["Time"] = AcStation_starttime;


    emit toinsert_AcStation_log(map);
    uploaddisksize();
    getnetworkanddiskmsg();
}

MainWindow::~MainWindow()
{
    qDebug()<<"~MainWindow exit!";
    delete ui;
}

void MainWindow::gotoSearchForm(int type,QString userid,bool videojur){


    howtosearch = new HowToSearch(type,userid,videojur,this);


    connect(howtosearch,SIGNAL(gotosearch_by_files_type(int,QString,bool)),this,SLOT(gotoSearch_By_Files_Type(int,QString,bool)));
    connect(howtosearch,SIGNAL(gotosearch_by_date(int,QString,bool)),this,SLOT(gotoSearch_By_Date(int,QString,bool)));
    connect(howtosearch,SIGNAL(gotosearch_by_staff_name(int,QString,bool)),this,SLOT(gotoSearch_By_Staff_Name(int,QString,bool)));

    connect(howtosearch,SIGNAL(goback()),this,SLOT(goBackMainForm()));

    ui->stackedWidget->addWidget(howtosearch);

    ui->stackedWidget->setCurrentWidget(howtosearch);
}

void MainWindow::gotoSetupForm(QList<int> connectnumber)
{
    setupform = new SetUpForm(dasbuddy,connectnumber,zfysetting,this);


    connect(setupform,SIGNAL(goback()),this,SLOT(goBackMainFormaddwidget()));

    connect(setupform,SIGNAL(changesuccess(int)),form,SLOT(reloadconfig(int)));

    ui->stackedWidget->addWidget(setupform);

    ui->stackedWidget->setCurrentWidget(setupform);

    for (int i=0;i<30;i++) {


        connect(dasbuddy[i],SIGNAL(getAllSettingStr(QVariantMap)),setupform,SLOT(recieveAllSetting(QVariantMap)));
    }

}

void MainWindow::gotonewSetupForm()
{
    newsetupform = new newSetUpForm(dasbuddy,this);
    connect(newsetupform,SIGNAL(goback()),this,SLOT(goBackMainForm()));
    ui->stackedWidget->addWidget(newsetupform);

    ui->stackedWidget->setCurrentWidget(newsetupform);
}


void MainWindow::gotoSearch_By_Files_Type(int type,QString userid,bool videojur){

    if(searchbytype != nullptr){

    }
    searchbytype = new SearchByType(type,userid,mysqlite,videojur,this);

    connect(searchbytype,SIGNAL(goback()),this,SLOT(goBackSearchForm()));

    connect(searchbytype,SIGNAL(gotovideo(QVariantMap)),this,SLOT(gotoVideoForm(QVariantMap)));

    ui->stackedWidget->addWidget(searchbytype);
    ui->stackedWidget->setCurrentWidget(searchbytype);
}

void MainWindow::gotoSearch_By_Date(int type,QString userid,bool videojur){
    searchbydate = new SearchByDate(type,userid,mysqlite,videojur,this);
    connect(searchbydate,SIGNAL(goback()),this,SLOT(goBackSearchForm()));
    connect(searchbydate,SIGNAL(gotovideo(QVariantMap)),this,SLOT(gotoVideoForm(QVariantMap)));
    ui->stackedWidget->addWidget(searchbydate);
    ui->stackedWidget->setCurrentWidget(searchbydate);
}

void MainWindow::gotoSearch_By_Staff_Name(int type,QString userid,bool videojur){
    searchbystaffname = new SearchByStaffName(type,userid,mysqlite,videojur,this);

    connect(searchbystaffname,SIGNAL(goback()),this,SLOT(goBackSearchForm()));
    connect(searchbystaffname,SIGNAL(gotovideo(QVariantMap)),this,SLOT(gotoVideoForm(QVariantMap)));
    ui->stackedWidget->addWidget(searchbystaffname);
    ui->stackedWidget->setCurrentWidget(searchbystaffname);
}
void MainWindow::goBackMainForm(){


    ui->stackedWidget->setCurrentIndex(firstIndex);
    //kill other form
    //ui->stackedWidget->removeWidget()

    for(int i = ui->stackedWidget->count(); i >= 2; i--)

    {
        qDebug()<<"form i"<<i;
        QWidget* widget = ui->stackedWidget->widget(i-1);
        ui->stackedWidget->removeWidget(widget);
        widget->deleteLater();

    }
}

void MainWindow::goBackSearchForm(){


    ui->stackedWidget->setCurrentIndex(1);
    //kill other form
    //ui->stackedWidget->removeWidget()

    for(int i = ui->stackedWidget->count(); i >= 3; i--)

    {
        qDebug()<<"form i"<<i;
        QWidget* widget = ui->stackedWidget->widget(i-1);
        ui->stackedWidget->removeWidget(widget);
        widget->deleteLater();

    }
}
void MainWindow::goBackMainFormaddwidget(){

    ui->stackedWidget->setCurrentIndex(firstIndex);
    form->setlistWidget();
    //kill other form
    //ui->stackedWidget->removeWidget()

    for(int i = ui->stackedWidget->count(); i >= 2; i--)

    {
        qDebug()<<"form i"<<i;
        QWidget* widget = ui->stackedWidget->widget(i-1);
        ui->stackedWidget->removeWidget(widget);
        widget->deleteLater();

    }


}

void MainWindow::updatelabeltime()
{
    ui->label_time->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
}

void MainWindow::openbroswer()
{
    QString url = Config().Get("server","serverManagerIp").toString();
    //QString writetest = "chromium --chrome-frame http://www.baidu.com --no-sandbox";
    QString writetest = "chromium --chrome-frame " + url+ " --no-sandbox";
    m_proces_bash->write(writetest.toLocal8Bit() + '\n');
}
void MainWindow::goBackSearch(int search_type)
{
    ui->stackedWidget->setCurrentIndex(search_type);
    for(int i = ui->stackedWidget->count(); i >= 4; i--)

    {
        qDebug()<<"form i"<<i;
        QWidget* widget = ui->stackedWidget->widget(i-1);
        ui->stackedWidget->removeWidget(widget);
        widget->deleteLater();

    }
}
void MainWindow::gotoVideoForm(QVariantMap videomap){

    videoform = new VideoPlayerForm(mysqlite,this);


    connect(this,SIGNAL(govideo(QVariantMap)),videoform,SLOT(playvideo(QVariantMap)));
    connect(videoform,SIGNAL(goback(int)),this,SLOT(goBackSearch(int)));

    ui->stackedWidget->addWidget(videoform);

    ui->stackedWidget->setCurrentWidget(videoform);
    emit govideo(videomap);
}

//void MainWindow::onDevInserted(QUsbDevice::Id id)
//{
//    qInfo("Device inserted: %04x:%04x", id.vid, id.pid);

//    if(id.vid == 1539 &&  id.pid == 34321){
//        qDebug()<<"执法仪已插入";

//        //checkHubConnect();

//         // libusb_device * usb_device;
////        libusb_open(id.usb_device,&deviceHandler);

//        //form->toConnectZFY(id.usb_device);
//        connectads->toConnectZFYwithDevice(id.usb_device);
//        if(!ADSDeviceslist.isEmpty()){
//            for (int i=0;i<ADSDeviceslist.count();i++) {
//                qDebug()<<ADSDeviceslist.at(i).path;
//            }
//        }


//    }else{
//        qDebug()<<"vid :"<<id.vid<<"pid :"<<id.pid;
//    }
//}

//void MainWindow::onDevRemoved(QUsbDevice::Id id)
//{
//    qInfo("Device removed: %04x:%04x", id.vid, id.pid);
//    if(id.vid == 1539 &&  id.pid == 34321){
//        qDebug()<<"执法仪已移除";
//        //check_libusb_get_devices_list();
//    }else{
//        qDebug()<<"vid :"<<id.vid<<"pid :"<<id.pid;
//    }
//}





void MainWindow::checkHubConnect(){
    ADSDeviceslist = getusb->getADSDevices();
    //ADSDeviceslist.at(0).path


    if(!ADSDeviceslist.isEmpty()){

        ZFYConnectNumber.clear();
        //监测port口，将对应位置的连接状态设置为true,拔出后需要重新设置为false
        for(int i=0;i<getusb->getADSDevices().count();i++){
            qDebug()<<getusb->getADSDevices().at(i).path;

            ZFYConnectNumber.append(usbhublist.indexOf(getusb->getADSDevices().at(i).path));

        }

        emit sendConnectPortNumber(ZFYConnectNumber);
//        if(ADSDeviceslist.at(0).path == usbhublist.at(ui->comboBox->currentText().toInt()-1)){
//            qDebug()<<"连接正确";
//            emit connecthubtrue();
//            //        QApplication::setQuitOnLastWindowClosed(false);
//            //        QMessageBox message(QMessageBox::Warning,"Information",

//            //                            "连接正确",QMessageBox::Ok,NULL);
//            //        if (message.exec()==QMessageBox::Ok)
//            //        {
//            //            qDebug()<<"clicked ok\n";
//            //        }

//            //            qDebug()<<"getIsConnect"<<connectads->getIsConnect();
//            //            if(!connectads->getIsConnect()){
//            //                //qDebug()<<"getIsConnect"<<connectads->getIsConnect();
//            //                connectads->connectdevice();
//            //            }

//        }else{

//            qDebug()<<"请将执法仪插入对应位置";
//            emit connecthubfalse();
//            //        QApplication::setQuitOnLastWindowClosed(false);
//            //        QMessageBox message(QMessageBox::Warning,"Information",

//            //                            "请将执法仪插入对应位置",QMessageBox::Ok,NULL);
//            //        if (message.exec()==QMessageBox::Ok)
//            //        {
//            //            qDebug()<<"clicked ok\n";
//            //        }
//        }
    }

}

void MainWindow::initActiveMq()
{
    connect(&myconsumer,SIGNAL(comsumerreceiver(QString)),this,SLOT(getConsumerMessage(QString)));
    connect(&myproducer,SIGNAL(producer_connectfail()),this,SLOT(toexitApp()));
    activemq::library::ActiveMQCPP::initializeLibrary();
    std::string brokerURI;
    std::string destURI;
    bool useTopics;
    bool clientAck;
    //brokerURI ="tcp://127.0.0.1:61616?transport.useAsyncSend=true&maxReconnectDelay=10000";
    brokerURI ="tcp://127.0.0.1:61616";
    useTopics = false;
    unsigned int numMessages = 99999999;
    clientAck = false;

    std::string consumerdestURI = Config().Get("server","ServerQueue").toString().toStdString();
    qDebug()<<"consumerdestURI"<<QString::fromStdString(consumerdestURI);
    std::string consumerbrokerURI = Config().Get("server","serverMqIp").toString().toStdString();
    qDebug()<<"consumerbrokerURI"<<QString::fromStdString(consumerbrokerURI);
    destURI = "ServerQueue";

    bool connectresult = myproducer.start(brokerURI, numMessages, destURI, useTopics ,clientAck);
    qDebug()<<"myproducer_connectresult"<<connectresult;
    bool myconsumerresult = myconsumer.start(consumerbrokerURI, consumerdestURI, useTopics, clientAck);
    qDebug()<<"myconsumer_connectresult"<<myconsumerresult;


}
void MainWindow::changetoUdisk(){


    //QThread::msleep(500);
    qDebug()<<"changedtoUdisk";
    //connectads->changetoUdisk();
}



void MainWindow::doinsertActiveMq(QVariant dataVar)
{
    qDebug()<<"insert copyfile ActiveMq start!!";
    FileAllInfo file_info = dataVar.value<FileAllInfo>();
    QVariantMap map;
    //QVariantMap mapOut;
    QVariantList varlist;
    QVariantMap map2;
    map["ClientCode"] = MyHostIPV4Address;
    map["OperaType"] = 1;

    map2["TaskId"] = file_info.file_uuid;
    map2["FileName"] = file_info.file_name;
    map2["CreateTime"] = file_info.create_time;
    map2["UploadTime"] = file_info.upload_time;
    map2["CaseNo"] = file_info.file_caseNo;
    map2["FileLength"] = file_info.file_size;
    map2["IsImportant"] = file_info.is_important;
    map2["PolNo"] = file_info.user_id;
    map2["CheckCode"] = file_info.file_uuid;
    map2["SaveMaxDay"] = file_info.save_max_day;
    map2["DevNo"] = file_info.driver_id;
    map2["FileType"] = file_info.file_type;
    map2["FileShowPath"] = file_info.fileshowpath;
    map2["Coordinates"] = file_info.file_gps;
    varlist.append(map2);
    map["OperaModels"] = varlist;
    map["Sendtime"] = QDateTime::currentDateTime();

    //mapOut["AddDocInfo"] = map;
    QString data = QJsonDocument::fromVariant(map).toJson(QJsonDocument::Indented);//带有格式
    myproducer.sendTxtMsg("Client2Server:"+data.toStdString());
    map.clear();
    map2.clear();
    varlist.clear();


}
void MainWindow::doMqinsertLog(int type,QVariantMap map){
    qDebug()<<"insert ActiveMq Log start!!";
    QVariantMap maptop;
    maptop["ClientCode"] = MyHostIPV4Address;
    maptop["OperaType"] = type;
    maptop["Sendtime"] = QDateTime::currentDateTime();
    QVariantList varlist;
    varlist.append(map);
    maptop["OperaModels"] = varlist;
    QString data = QJsonDocument::fromVariant(maptop).toJson(QJsonDocument::Indented);//带有格式
    myproducer.sendTxtMsg("Client2Server:"+data.toStdString());
    maptop.clear();
    map.clear();
    varlist.clear();

}
void MainWindow::getConsumerMessage(QString str)
{
    qDebug()<<"message str"<<str;
    str = str.mid(str.indexOf(":")+1);
    qDebug()<<"message str"<<str;
    //QVariantMap map ;
    //str.to
    //map.value()
    //Client2Server[""]
    QJsonParseError err_rpt;

    QJsonDocument  root_Doc = QJsonDocument::fromJson(str.toLocal8Bit(), &err_rpt);//字符串格式化为JSON
    if(err_rpt.error != QJsonParseError::NoError)
    {
        qDebug() << "JSON格式错误";
        //return -1;
    }else{
        //        qDebug() << "JSON格式正确：\n" << root_Doc;
        QJsonObject root_Obj = root_Doc.object();
        int result_OperaType = root_Obj.value("OperaType").toInt();
        QString result_Datetime = root_Obj.value("Datetime").toString();
        QJsonValue result_OperaModels = root_Obj.value("OperaModels");
        if(result_OperaType  == 1){
            //update file
            if(result_OperaModels.isArray())
            {
                QVariantMap map;
                QJsonObject result_Obj = result_OperaModels.toArray().at(0).toObject();

                QString result_CheckCode = result_Obj.value("CheckCode").toString();
                qDebug() << result_CheckCode;

                QString result_CreateTime = result_Obj.value("CreateTime").toString();
                qDebug() << result_CreateTime;

                QString result_DevNo = result_Obj.value("DevNo").toString();
                qDebug() << result_DevNo;

                QString result_TaskId = result_Obj.value("TaskId").toString();
                qDebug() << result_TaskId;


                map["CheckCode"] = result_CheckCode;
                map["CreateTime"] = result_CreateTime;
                map["DevNo"] = result_DevNo;
                map["TaskId"] = result_TaskId;

                emit toupdateSql(map);
            }
        }
        if(result_OperaType == 2){
            //delete file
            if(result_OperaModels.isArray())
            {
                QVariantMap map;
                QJsonObject result_Obj = result_OperaModels.toArray().at(0).toObject();
                QString result_CheckCode = result_Obj.value("CheckCode").toString();
                qDebug() << result_CheckCode;

                QString result_TaskId = result_Obj.value("TaskId").toString();
                qDebug() << result_TaskId;

                map["CheckCode"] = result_CheckCode;
                map["TaskId"] = result_TaskId;


                emit todeleteSql(map);
            }

        }


    }

}

void MainWindow::toexitApp()
{
    qDebug()<<"connect local activemq fail,to exit!!!";

    QMessageBox::StandardButton rb = QMessageBox::warning(NULL, tr("Warning"), tr("Can not Connect the local ActiveMq!click button to exit"), QMessageBox::Ok );
    if(rb == QMessageBox::Ok)
    {
        exit(0);
    }
}


QHostAddress MainWindow::getHostIPV4Address()
{
    foreach(const QHostAddress& hostAddress,QNetworkInterface::allAddresses())
        if ( hostAddress != QHostAddress::LocalHost && hostAddress.toIPv4Address() )
            return hostAddress;

    return QHostAddress::LocalHost;
}

void MainWindow::initProcess()
{
    m_proces_bash = new QProcess;
    m_proces_bash->start("bash");
    m_proces_bash->waitForStarted();
    qDebug()<<"initProcess";
    connect(m_proces_bash,SIGNAL(readyReadStandardOutput()),this,SLOT(readBashStandardOutputInfo()));
    connect(m_proces_bash,SIGNAL(readyReadStandardError()),this,SLOT(readBashStandardErrorInfo()));

    //m_proces_bash->close()

    QString text_start = "who -b";
    //QString text_start = "last -x|grep shutdown | head -1";
    m_proces_bash->write(text_start.toLocal8Bit() + '\n');
}


void MainWindow::readBashStandardOutputInfo()
{
    qDebug()<<"readBashStandardOutputInfo";
    QByteArray cmdout = m_proces_bash->readAllStandardOutput();
    if(!cmdout.isEmpty()){
        if(cmdout.contains("system boot")){

        }
        //ui->textEdit_bashmsg->append(QString::fromLocal8Bit(cmdout));
        qDebug()<<"readBashStandardOutputInfo"<<QString::fromLocal8Bit(cmdout);

        QRegExp reg1("([0-9]{4}-[0-9]{2}-[0-9]{2}\\s[0-9]{2}:[0-9]{2})");
        reg1.indexIn(QString::fromLocal8Bit(cmdout));
        qDebug()<<"reg1.cap"<<reg1.cap(1);

        if(reg1.cap(1).length() >0){
            qDebug()<<"reg1.cap(1).length()"<<reg1.cap(1).length();
            QString strBuffer = reg1.cap(1) + ":00";
            systemstarttime = QDateTime::fromString(strBuffer, "yyyy-MM-dd hh:mm:ss");


            qDebug()<<"systemstarttime"<<systemstarttime;

            QString struuid = QUuid::createUuid().toString().remove("{").remove("}");
            QVariantMap map;
            map["TaskId"] = struuid;
            map["OperaType"] = 1;
            map["OperaContent"] = "system start time";
            map["Time"] = systemstarttime;

            emit toinsert_AcStation_log(map);

            //to get shutdown time
            QString text_shutdown = "last -x|grep shutdown | head -1";
            m_proces_bash->write(text_shutdown.toLocal8Bit() + '\n');
        }else{
            QRegExp myshutdown("(Mon|Tue|Wed|Thu|Fri|Sat|Sun)\\s(Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec)\\s{1,2}(\\d{1,2})\\s(\\d{2}):(\\d{2})");
            myshutdown.indexIn(QString::fromLocal8Bit(cmdout));
            qDebug()<<"myex.capturedTexts()"<<myshutdown.capturedTexts();
            qDebug()<<"myex.cap(0)"<<myshutdown.cap(0);
            QString date1 = QString("%1 %2 %3 %4:%5")
                    .arg(myshutdown.cap(1)).arg(myshutdown.cap(2))
                    .arg(myshutdown.cap(3).toInt(),2,10,QLatin1Char('0'))
                    .arg(myshutdown.cap(4)).arg(myshutdown.cap(5));

            //QString date1 = myshutdown.cap(0);

            QString year = QDate::currentDate().toString("yyyy");
            QString datelast = date1 + ":00 "+year;

            QLocale locale = QLocale::English;
            QDateTime myshutdowntime = locale.toDateTime(datelast,"ddd MMM dd hh:mm:ss yyyy");
            qDebug()<<"myshutdowntime"<<myshutdowntime;

            QString struuid = QUuid::createUuid().toString().remove("{").remove("}");
            QVariantMap map;
            map["TaskId"] = struuid;
            map["OperaType"] = 2;
            map["OperaContent"] = "system shutdown time";
            map["Time"] = myshutdowntime;

            emit toinsert_AcStation_log(map);



        }











    }
}

void MainWindow::readBashStandardErrorInfo()
{
    qDebug()<<"readBashStandardErrorInfo";
    QByteArray cmdout = m_proces_bash->readAllStandardError();
    if(!cmdout.isEmpty()){
        //ui->textEdit_bashmsg->append(QString::fromLocal8Bit(cmdout));
        qDebug()<<"readBashStandardErrorInfo"<<QString::fromLocal8Bit(cmdout);

    }
}
void MainWindow::closeEvent(QCloseEvent *event)
{
    qDebug()<<"close event";


    loginForm = new LoginForm(0);
    //loginForm->setWindowFlag(Qt::)
    loginForm->setWindowModality(Qt::ApplicationModal);
    loginForm->show();

    connect(loginForm,SIGNAL(loginsuccess(int,QString,QStringList)),this,SLOT(checkjurstr(int,QString,QStringList)));

    event->ignore();
}
void MainWindow::checkjurstr(int type,QString login_UserName,QStringList jurstr){
    loginForm->hide();
    loginForm->deleteLater();
    if(jurstr.contains("49")){

        QString struuid = QUuid::createUuid().toString().remove("{").remove("}");
        QVariantMap map;
        map["TaskId"] = struuid;
        map["OperaType"] = 4;
        map["OperaContent"] = "Acquisition Station close time";
        map["Time"] = QDateTime::currentDateTime();


        emit toinsert_AcStation_log(map);


        QString struuid2 = QUuid::createUuid().toString().remove("{").remove("}");
        QVariantMap map2;
        map2["TaskId"] = struuid;
        map2["OperaType"] = 6;
        map2["OperaContent"] = "Acquisition Station close time on userlog";
        map2["Time"] = QDateTime::currentDateTime();
        map2["PolNo"] = login_UserName;
        map2["DevNo"] = QString("");
        emit toinsert_user_Log(map2);
        exit(0);
    }else {

        QMessageBox::warning(NULL, "INFO", "login success,but your account do not have permission to exit this application!", QMessageBox::Ok );
    }

}

void MainWindow::tosendConfirmtoSerVer(int status, QString taskid)
{
    qDebug()<<"send confirm taskid back to server ActiveMq !!";
    QVariantMap map;

    map["Receipt"] = taskid;
    map["Status"] = status;

    QString data = QJsonDocument::fromVariant(map).toJson(QJsonDocument::Indented);//带有格式
    myproducer.sendTxtMsg("ConfirmMsg:"+data.toStdString());
    map.clear();
}

void MainWindow::uploaddisksize()
{

    storage.refresh();
    //storage.by
    freedisksize = QString::number(storage.bytesAvailable()/1024.00/1024.00/1024.00,'f',2).toDouble();


    QVariantMap map;
    QString struuid = QUuid::createUuid().toString().remove("{").remove("}");
    map["TaskId"] = struuid;
    map["OperaType"] = 1;
    map["DiskFreeSize"] = freedisksize;
    doMqinsertLog(4,map);
}

void MainWindow::getnetworkanddiskmsg()
{

    storage.refresh();
    //storage.by
    freedisksize = QString::number(storage.bytesAvailable()/1024.00/1024.00/1024.00,'f',2).toDouble();
    bool pingresult = pingOk(pingip);

    emit updateDiskSize(freedisksize/totaldisksize*100.00,pingresult);
}
bool MainWindow::pingOk(QString sIp)
{
    //判断输入IP是否为空
    if (sIp.isEmpty()) return false;

    //构建ping命令
    QString sCmd = QString("ping -s 1 -c 1 %1").arg(sIp);
    QProcess proc;
    proc.start(sCmd);
    proc.waitForReadyRead(500);
    proc.waitForFinished(500);

    //读取ping命令返回的所有信息
    QString sRet = proc.readAll();

    //如果网络连通,ping命令返回的信息,包含 "TTL"
    bool bPing = (sRet.indexOf("TTL", 0, Qt::CaseInsensitive) >= 0);
    qDebug() << sCmd << sRet << bPing;
    proc.finished(0);
    proc.terminate();

    return bPing;
}
