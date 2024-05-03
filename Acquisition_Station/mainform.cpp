#include "mainform.h"
#include "ui_mainform.h"
#include<QColor>
MainForm::MainForm(DASBuddy **buddy,MySqlLite *sqltie,ZFYSetting *zfysetting,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainForm)
{
    ui->setupUi(this);

    this->zfysetting = zfysetting;

    ui->pushButton_setup2->setVisible(false);

    //qDebug()<<"MainForm QThread::currentThreadId()=="<<QThread::currentThreadId();
    for (int i=0;i<30;i++) {
        //dasbuddy[i] = new DASBuddy(this);
        dasbuddy[i] = buddy[i];
        //connect(dasbuddy[i],SIGNAL(changesuccess(QString,int)),this,SLOT(starttoMount(QString,int)));

        dasbuddy[i]->setPhotoText(QString::number(i+1));



        connect(dasbuddy[i],SIGNAL(sendProgress(QVariant,qreal,int)),this,SLOT(updateProgress(QVariant,qreal,int)));


        connect(dasbuddy[i],SIGNAL(sendConnectPortNumber(int,int)),this,SLOT(getZFYConnectPortNumber(int,int)));

        connect(dasbuddy[i],SIGNAL(sig_insertUserLog(int)),this,SLOT(toinsertUserLogAndPost(int)));


        connect(dasbuddy[i],SIGNAL(sigtowirtesql(QVariantMap)),this,SLOT(slottoinsertsql(QVariantMap)));


        connect(dasbuddy[i],SIGNAL(checkfileiscopy(QFileInfoList,QString,int)),this,SLOT(slottocheckfile(QFileInfoList,QString,int)));


        connect(dasbuddy[i],SIGNAL(zfycopyfinished()),this,SLOT(slottosubuploadingnum()));

        //connect(dasbuddy[i],SIGNAL(insertZFY(int)),this,SLOT(toinsertZFYDialog(int)));


        //connect(this,SIGNAL(toshineBackground()),dasbuddy[i],SLOT(initTimer_shine()));

        //dasbuddy[i]->setNumberText(QString("%1").arg(i+1, 2, 10, QLatin1Char('0')));
        //dasbuddy->setDeviceId("0");
        QListWidgetItem *newItem = new QListWidgetItem();

        ui->listWidget->insertItem(i,newItem);

        ui->listWidget->setItemWidget(newItem, dasbuddy[i]);
    }


    thread1 = new HotPlugThread(dasbuddy);

    thread1->start();

    //connect(this,SIGNAL(deleteZFY(int)),thread1,SLOT(todeleteZFY(int)));
    connect(thread1,SIGNAL(deleteZFY(int)),this,SLOT(todeleteZFY(int)));

    connect(thread1,SIGNAL(insertZFY(int)),this,SLOT(toinsertZFYDialog(int)));



    //insertZFY
    //connect(thread1,SIGNAL(insertZFY()),this,SLOT(toinsertZFYDialog()));
    //    zfy1 = new ZFYControl(0);

    //    controlthread = new QThread();
    //    zfy1->moveToThread(controlthread);
    //    controlthread->start();
    //    qDebug()<<"mythread QThread::currentThreadId()=="<<QThread::currentThreadId();


    //    zfy2 = new ZFYControl(1);
    //    zfy2->setPath("/home/itventi/testFile");
    //    controlthread2 = new QThread();
    //    zfy2->moveToThread(controlthread2);
    //    controlthread2->start();

    mysql = sqltie;
    //    mysql->openmysql();
    //QStringList mylist = mysql->gettableNameList();

    //dirpath = "/home/itventi/copyfile/"+QDateTime::currentDateTime().toString("yyyy-MM-dd") + "/" + QString::number(user_Id) +"/";


    //    connect(zfy1,SIGNAL(copyonesuccess(QVariant,qreal,int)),this,SLOT(updateProgress(QVariant,qreal,int)));

    //    connect(this,SIGNAL(startCopy(QString,int)),zfy1,SLOT(copyFile2(QString,int)));

    //    connect(controlthread,SIGNAL(finished()),zfy1,SLOT(deleteLater()));

    //    connect(controlthread,SIGNAL(finished()),this,SLOT(finishedThreadSlot()));

    //    connect(zfy2,SIGNAL(copyonesuccess(QVariant,qreal,int)),this,SLOT(updateProgress(QVariant,qreal,int)));

    //    connect(this,SIGNAL(startCopy(QString,int)),zfy2,SLOT(copyFile2(QString,int)));



    //connect(this,SIGNAL(insertfiles_sqlite(QVariant)),mysql,SLOT(doinsertfiles(QVariant)));

    connect(this,SIGNAL(toinsert_user_Log(QVariantMap)),mysql,SLOT(insert_User_Log(QVariantMap)));


    connect(this,SIGNAL(insertfiles_sqlite(QVariantMap)),mysql,SLOT(doinsertfiles(QVariantMap)));


    connect(this,SIGNAL(sigsqltocheckfile(QFileInfoList,QString,int)),mysql,SLOT(slotcheckfile(QFileInfoList,QString,int)));

    connect(mysql,SIGNAL(sig_filenocopy(QFileInfoList,int)),this,SLOT(slotstartcopy(QFileInfoList,int)));

    naManager = new QNetworkAccessManager(this);
    connect(naManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));





    uploadingnum = new UploadingNum();
    uploadingnum->start();
    connect(uploadingnum,SIGNAL(sigtocopy(int)),this,SLOT(slottocppy(int)));
}


MainForm::~MainForm()
{
    delete ui;
}

void MainForm::setlistWidget()
{


    //ui->listWidget->update();
    //ui->listWidget->clear();
    for (int i=0;i<30;i++) {


        //ui->listWidget->takeItem(i);
        QListWidgetItem *newItem = new QListWidgetItem();

        ui->listWidget->insertItem(i,newItem);
        //ui->listWidget->setItemWidget()
        ui->listWidget->setItemWidget(newItem, dasbuddy[i]);


        //qDebug()<<"ui->listWidget->count():"<<ui->listWidget->count();

    }


    for (int j=30;j<60;j++) {
        ui->listWidget->takeItem(30);
        //qDebug()<<"ui->listWidget->count():"<<ui->listWidget->count();
    }


}




void MainForm::on_pushButton_searchfiles_clicked()
{
    //emit gosearch();

    //
    loginForm = new LoginForm(2);
    //loginForm->setWindowFlag(Qt::)
    loginForm->setWindowModality(Qt::ApplicationModal);
    loginForm->show();
    connect(loginForm,SIGNAL(loginsuccess(int,QString,QStringList)),this,SLOT(checkjurstr(int,QString,QStringList)));
}

void MainForm::getZFYConnectPortNumber(int num,int battery){

    //emit connectfinish();
    if(insertZFYDialog != nullptr){
        insertZFYDialog->hide();
        insertZFYDialog->deleteLater();
        insertZFYDialog = nullptr;
    }

    qDebug()<<"Connect num:" <<num;



    //dasbuddy[num]->setUserName("ABC");
    dasbuddy[num]->setBackgroundColor(QColor(68,114,196));
    dasbuddy[num]->setUserUpload(tr("waiting"));

    dasbuddy[num]->setMybattery(battery);
    //dasbuddy[num]->setphoto(":/image/police_2.png");
    //dasbuddy[num]->setNetworkPic("http://120.25.145.34:8818/Files/Images/PolImage/130dedf2d94e4e2d995255edf9c921ee.jpg");


    uploadingnum->setiswaitcopy(num,true);

}

void MainForm::todeleteZFY(int deleteNum)
{
    qDebug()<<"delete test!!!" <<deleteNum;
    qDebug()<<"change device status!";

    qDebug()<<"delete form!";
    qDebug()<<"delete Number:"<<deleteNum;
    dasbuddy[deleteNum]->stopShine();
    dasbuddy[deleteNum]->stopTimer();
    dasbuddy[deleteNum]->setUserName("");
    dasbuddy[deleteNum]->setBackgroundColor(QColor("white"));
    //dasbuddy[deleteNum]->setUserUpload("");

    dasbuddy[deleteNum]->setBatteryZero();
    dasbuddy[deleteNum]->setPhotoText(QString::number(deleteNum+1));
    dasbuddy[deleteNum]->setIsConnect(false);

    dasbuddy[deleteNum]->clearuploadsize();

    //add a signal to add user_log to sqlite

    QString struuid = QUuid::createUuid().toString().remove("{").remove("}");
    QVariantMap map;
    map["TaskId"] = struuid;
    map["OperaType"] = 2;
    map["OperaContent"] = "ZhifaYi delete";
    map["Time"] = QDateTime::currentDateTime();
    map["DevNo"] = dasbuddy[deleteNum]->getDriverid();
    map["PolNo"] = dasbuddy[deleteNum]->getUserid();

    emit toinsert_user_Log(map);


    uploadingnum->setiswaitcopy(deleteNum,false);

}

void MainForm::toinsertUserLogAndPost(int num)
{
    //

    //to insert log
    QString struuid = QUuid::createUuid().toString().remove("{").remove("}");
    QVariantMap map;
    map["TaskId"] = struuid;
    map["OperaType"] = 1;
    map["OperaContent"] = "ZhifaYi insert";
    map["Time"] = QDateTime::currentDateTime();
    map["DevNo"] = dasbuddy[num]->getDriverid();
    map["PolNo"] = dasbuddy[num]->getUserid();

    emit toinsert_user_Log(map);

    //http post to get the name and photo



}



void MainForm::updateProgress(QVariant dataVar,qreal progress,int zfynum)
{
    File_copy_Info copy_file_info = dataVar.value<File_copy_Info>();
    //mysql->insertb_files(copy_file_info.file_uuid,copy_file_info.file_name,copy_file_info.file_creattime,copy_file_info.file_est);
    qDebug()<<"progress"<<progress;
    dasbuddy[zfynum]->setUserUpload(QString::number(progress,'f',1)+"%");


    //emit insertfiles_sqlite(dataVar);
}


QString MainForm::getBattery(QString path){
    //zfylist[0].charge
    QRegExp batteryText("Battery0=(\\d{1,2})");
    QString batteryNumber = "-1";
    QFile file(path);
    //Battery0=(\d{1,2})
    QString string;
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        while (!file.atEnd())
        {
            string = file.readLine();
            batteryText.indexIn(string);
            //qDebug()<<batteryTest.cap(1);

        }
        file.close();
        batteryNumber = batteryText.cap(1);
        qDebug()<<"batteryNumber"<<batteryText.cap(1);


    }
    return batteryNumber;

}
void MainForm::closeThread(){

    qDebug()<<tr("关闭线程");
    if(thread1->isRunning())
    {
        //zfy1->closeThread();  //关闭线程槽函数
        thread1->quit();            //退出事件循环
        thread1->wait();            //释放线程槽函数资源
    }
}

void MainForm::finishedThreadSlot(){

    qDebug()<<tr("多线程触发了finished信号");
}


void MainForm::on_pushButton_setup_clicked()
{

    //
    loginForm = new LoginForm(1);
    //loginForm->setWindowFlag(Qt::)
    loginForm->setWindowModality(Qt::ApplicationModal);
    loginForm->show();
    connect(loginForm,SIGNAL(loginsuccess(int,QString,QStringList)),this,SLOT(checkjurstr(int,QString,QStringList)));

}

void MainForm::checkjurstr(int type,QString login_UserName,QStringList jurstr){
    loginForm->hide();
    loginForm->deleteLater();
    if(type == 1){
        if(jurstr.contains("40")){


            QList<int> connetnumber;
            for (int i=0;i<30;i++) {
                if(dasbuddy[i]->getIsConnect()){
                    connetnumber.append(i);
                }
            }
            qDebug()<<"connetnumber:"<<connetnumber;

            emit gosetup(connetnumber);

            //to write to sql


            QString struuid = QUuid::createUuid().toString().remove("{").remove("}");
            QVariantMap map;
            map["TaskId"] = struuid;
            map["OperaType"] = 5;
            map["OperaContent"] = "login to go setup";
            map["Time"] = QDateTime::currentDateTime();
            map["DevNo"] = QString("");
            map["PolNo"] = login_UserName;

            emit toinsert_user_Log(map);




        }else {

            QMessageBox::warning(NULL, "INFO", tr("login success,but your account do not have permission to go setting!"), QMessageBox::Ok );
        }
    }if(type == 2){
        if(jurstr.contains("32")){
            //see the files this user upload
            bool videojur =false;
            if(jurstr.contains("3")){
                videojur = true;
            }
            emit gosearch(1,login_UserName,videojur);

            QString struuid = QUuid::createUuid().toString().remove("{").remove("}");
            QVariantMap map;
            map["TaskId"] = struuid;
            map["OperaType"] = 3;
            map["OperaContent"] = "login to find files";
            map["Time"] = QDateTime::currentDateTime();
            map["DevNo"] = QString("");
            map["PolNo"] = login_UserName;

            emit toinsert_user_Log(map);


        }else if (jurstr.contains("33") || jurstr.contains("34")) {

            //see all files

            bool videojur =false;
            if(jurstr.contains("3")){
                videojur = true;
            }
            emit gosearch(0,login_UserName,videojur);

            QString struuid = QUuid::createUuid().toString().remove("{").remove("}");
            QVariantMap map;
            map["TaskId"] = struuid;
            map["OperaType"] = 3;
            map["OperaContent"] = tr("login to find files");
            map["Time"] = QDateTime::currentDateTime();
            map["DevNo"] = QString("");
            map["PolNo"] = login_UserName;

            emit toinsert_user_Log(map);

        }else {
            //can not see any files
            QMessageBox::warning(NULL, tr("INFO"), tr("login success,but your account do not have permission to search file!"), QMessageBox::Ok );
        }
    }if(type == 3){

        QList<int> uploadfinishlist;
        for (int i=0;i<30;i++) {
            if(dasbuddy[i]->getIsConnect()){
                if(dasbuddy[i]->getUpload() =="100.0%"){
                    uploadfinishlist.append(i);
                }

            }
        }

        qDebug()<<"uploadfinishlist:"<<uploadfinishlist;


        int maxbattery = -1;
        if(uploadfinishlist.isEmpty()){

            qDebug()<<"no zfy upload finished!";
            QMessageBox::warning(NULL, tr("INFO"), tr("all the zfy is uploading,please wait!"), QMessageBox::Ok );
        }else {
            for (int i=0;i<uploadfinishlist.count();i++) {
                if(dasbuddy[uploadfinishlist.at(i)]->getMybattery() == 100){

                    maxindex = uploadfinishlist.at(i);
                    maxbattery = dasbuddy[uploadfinishlist.at(i)]->getMybattery();
                    break;

                }
                else if(maxbattery < dasbuddy[uploadfinishlist.at(i)]->getMybattery()){
                    maxbattery = dasbuddy[uploadfinishlist.at(i)]->getMybattery();
                    maxindex = uploadfinishlist.at(i);
                }

            }

            qDebug()<<"maxindex:"<<maxindex<<"maxbattery"<<maxbattery;

            if(login_UserName == "admin1"){


                QString url = Config().Get("server","serverIp").toString() +"/api/Staff/AjaxClientGetAllUser";
                qDebug()<<"getalluesrurl:"<<url;
                request.setUrl(QUrl(url));
                request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/x-www-form-urlencoded"));
                naManager->post(request,"");

            }else {

                QString setuser = QString("%1%2").arg("@UserID#").arg(login_UserName);
                qDebug()<<"setuser:"<<setuser;
                dasbuddy[maxindex]->tosetUserId(setuser);

                QThread::msleep(3000);
                //"please take out the device "+i
                QString str = tr("%1 %2").arg("please take out the device in form").arg(maxindex+1);
                QMessageBox::warning(NULL, tr("Message"), str, QMessageBox::Ok );

                //emit toshineBackground();
                dasbuddy[maxindex]->initTimer_shine();

            }
        }

    }if(type ==4){
        if(login_UserName == "admin1"){
            QString url = QCoreApplication::applicationDirPath() + "/Config.ini";
            qDebug()<<"config url"<<url;
            QDesktopServices::openUrl(QUrl(url));
            exit(0);
        }else {
            QMessageBox::warning(NULL, tr("INFO"), tr("login success,but your account do not have permission to change config!"), QMessageBox::Ok );
        }
    }



}

void MainForm::reloadconfig(int old_num){
    //Config()::G

    thread1->reloadConfig();
    thread1->clearForm(old_num);
}

void MainForm::on_btn_takecamera_clicked()
{
    qDebug()<<"btn_takecamera clicked!";

    //check connectlist
    QList<int> connetnumber;
    for (int i=0;i<30;i++) {
        if(dasbuddy[i]->getIsConnect()){
            connetnumber.append(i);
        }
    }
    qDebug()<<"connetnumber:"<<connetnumber;

    if(connetnumber.isEmpty()){
        //please connect device first
        //qDebug()<<"getBattery:"<<QString("86%").replace("%","").toInt();

        QMessageBox::warning(NULL, tr("Problem"), tr("please connect device first"), QMessageBox::Yes );
    }else {


        loginForm = new LoginForm(3);
        loginForm->setWindowModality(Qt::ApplicationModal);
        loginForm->show();
        connect(loginForm,SIGNAL(loginsuccess(int,QString,QStringList)),this,SLOT(checkjurstr(int,QString,QStringList)));
        //        for (int i=0;i<connetnumber.count();i++) {
        //            //get the battery
        //            //qDebug()<<"getBattery:"<<dasbuddy[connetnumber.at(i)]->getBattery().replace("%","").toInt();
        //            //get the uploadstatus
        //            //100.0%
        //            //qDebug()<<"getUpload:"<<dasbuddy[connetnumber.at(i)]->getUpload();


        //            if(dasbuddy[connetnumber.at(i)]->getMybattery() ==100 &&
        //                    dasbuddy[connetnumber.at(i)]->getUpload() =="100.0%"
        //                    ){
        //                qDebug()<<"device "<<i<<"full battery and upload finished!";

        //                //emit takedevice(i);
        //                dasbuddy[connetnumber.at(i)]->tosetUserId("@UserID#111111");

        //                QThread::msleep(3000);
        //                //"please take out the device "+i
        //                QString str = tr("%1 %2").arg("please take out the device in form").arg(i+1);
        //                QMessageBox::warning(this, tr("Message"), str, QMessageBox::Ok );
        //                //only need one device everytime ,need to break this for();
        //                break;
        //            }

    }

}


void MainForm::on_btn_manager_clicked()
{
    //QString url = Config().Get("server","serverIp").toString();
    //QDesktopServices::openUrl(QUrl(url));

    emit toopenbroswer();
}

void MainForm::toinsertZFYDialog(int num)
{
    qDebug()<<"mainform init ZFYDialog!";
    if(insertZFYDialog == nullptr){
        qDebug()<<"insertZFYDialog is null";
    }else {

        qDebug()<<"insertZFYDialog not null";
        insertZFYDialog->deleteLater();
    }
    insertZFYDialog = new InsertZFYDialog(num);

    insertZFYDialog->setWindowModality(Qt::ApplicationModal);
    insertZFYDialog->show();
    //connect(this,SIGNAL(connectfinish()),insertZFYDialog,SLOT(connectfinish()));
    connect(insertZFYDialog,SIGNAL(accepted()),this,SLOT(deleteZFYDialog()));
    connect(insertZFYDialog,SIGNAL(sigreconnect(int)),zfysetting,SLOT(toreconnect(int)));
}
void MainForm::deleteZFYDialog(){
    //insertZFYDialog->hide();
    qDebug()<<"delete zfydialog!";
    insertZFYDialog->deleteLater();
    insertZFYDialog = nullptr;
}
void MainForm::requestFinished(QNetworkReply* reply) {
    // 获取http状态码
    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(statusCode.isValid())
        qDebug() << "status code=" << statusCode.toInt();

    QVariant reason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
    if(reason.isValid())
        qDebug() << "reason=" << reason.toString();

    QNetworkReply::NetworkError err = reply->error();
    if(err != QNetworkReply::NoError) {
        qDebug() << "Failed: " << reply->errorString();
    }
    else {
        // 获取返回内容
        QString str = reply->readAll();
        qDebug() << str;
        QJsonParseError err_rpt;
        QJsonDocument  root_Doc = QJsonDocument::fromJson(str.toLocal8Bit(), &err_rpt);//字符串格式化为JSON

        if(err_rpt.error != QJsonParseError::NoError)
        {
            qDebug() << "JSON格式错误";
            //return -1;
        }else{
            QJsonObject root_Obj = root_Doc.object();
            int result_status = root_Obj.value("status").toInt();
            if(result_status == 0){
                //fail
                QString result_msg = root_Obj.value("msg").toString();
                qDebug()<<"result_status:"<<result_status<<"msg:"<<result_msg;

                //QMessageBox::warning(NULL, "Login Fail", result_msg, QMessageBox::Ok );
            }
            if(result_status == 1){

                chooseUserDialog = new ChooseUserDialog();
                //loginForm->setWindowFlag(Qt::)
                chooseUserDialog->setWindowModality(Qt::ApplicationModal);
                chooseUserDialog->show();
                connect(this,SIGNAL(getUsers(QStringList)),chooseUserDialog,SLOT(addUsers(QStringList)));
                connect(chooseUserDialog,SIGNAL(towriteUser(QString)),this,SLOT(adminwriteuser(QString)));
                //success
                QJsonArray result_data = root_Obj.value("data").toArray();

                QStringList userlist;
                for (int i=0;i<result_data.count();i++) {
                    QString usermsg = result_data.at(i).toObject().value("polno").toString()
                            +":" + result_data.at(i).toObject().value("polName").toString();
                    userlist.append(usermsg);
                }

                emit getUsers(userlist);


            }





        }

    }
}
void MainForm::adminwriteuser(QString polno){

    chooseUserDialog->deleteLater();

    QString setuser = QString("%1%2").arg("@UserID#").arg(polno);
    qDebug()<<"setuser:"<<setuser<<"maxindex:"<<maxindex;
    dasbuddy[maxindex]->tosetUserId(setuser);

    QThread::msleep(3000);
    //"please take out the device "+i
    QString str = tr("%1 %2").arg("please take out the device in form").arg(maxindex+1);
    QMessageBox::warning(NULL, tr("Message"), str, QMessageBox::Ok );

    dasbuddy[maxindex]->initTimer_shine();
}
void MainForm::on_btn_config_clicked()
{


    //
    loginForm = new LoginForm(4);
    //loginForm->setWindowFlag(Qt::)
    loginForm->setWindowModality(Qt::ApplicationModal);
    loginForm->show();
    connect(loginForm,SIGNAL(loginsuccess(int,QString,QStringList)),this,SLOT(checkjurstr(int,QString,QStringList)));
    //    QString url = QCoreApplication::applicationDirPath() + "/Config.ini";
    //    qDebug()<<"config url"<<url;
    //    QDesktopServices::openUrl(QUrl(url));
}

void MainForm::updatedisksizelabel(double disksize,bool pingresult)
{
    ui->label_disksize->setText(QString::number(disksize)+" %");

    if(pingresult){
        ui->label_networkstatus->setText("connect");
    }else {
        ui->label_networkstatus->setText("disconnect");
    }

}
void MainForm::setIPV4Label(QString ipv4Address){
    ui->label_hostip->setText(ipv4Address);
}
void MainForm::on_pushButton_setup2_clicked()
{
    emit gonewsetup();
}

void MainForm::slottoinsertsql(QVariantMap copyfilemap)
{
    emit insertfiles_sqlite(copyfilemap);
}

void MainForm::slottocheckfile(QFileInfoList filelist, QString driverid, int num)
{
    emit sigsqltocheckfile(filelist,driverid,num);
}

void MainForm::slotstartcopy(QFileInfoList filelist, int num)
{
    qDebug()<<"zfy num"<<num<<"filelistcount:"<<filelist.count();
    dasbuddy[num]->startcopynoexits(filelist);
}

void MainForm::slottocppy(int num)
{
    //emit sigtobuddycheckcopy(num);
    dasbuddy[num]->starttocheckcopy(num);
}

void MainForm::slottosubuploadingnum()
{
    uploadingnum->subUploadingnum();
}
