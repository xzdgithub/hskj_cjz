#include "setupform.h"
#include "ui_setupform.h"

SetUpForm::SetUpForm(DASBuddy **buddy,QList<int> connectnumber,ZFYSetting *zfysetting,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SetUpForm)
{
    ui->setupUi(this);

    qDebug()<<"init zfysetting form!";
    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());

    this->zfysetting = zfysetting;
    isUserSelect = false;
    isAutoGetLanguage = false;
    //setupthread = new QThread();

    //    if(setting_dialog == NULL){
    //        qDebug()<<"init dialog";
    //    }


    QRegExp regx("[0-9A-Za-z]{6}");
    QValidator *validator = new QRegExpValidator(regx,ui->lineEdit_UserId);
    ui->lineEdit_UserId->setValidator(validator);

    QRegExp regx2("[0-9A-Za-z]{7}");
    QValidator *validator2 = new QRegExpValidator(regx2,ui->lineEdit_DriverId);
    ui->lineEdit_DriverId->setValidator(validator2);

    ui->label->setVisible(false);
    ui->comboBox_mylist->setVisible(false);
    ui->label_2->setVisible(false);
    ui->comboBox_change->setVisible(false);
    ui->pushButton_change->setVisible(false);
    setcomboxdisable();
    getLanguage();
    for (int i=0;i<30;i++) {
        dasbuddy[i] = buddy[i];

        QListWidgetItem *newItem = new QListWidgetItem();

        ui->listWidget->insertItem(i,newItem);

        ui->listWidget->setItemWidget(newItem, dasbuddy[i]);
    }
    //    zfysetting = new ZFYSetting(dasbuddy);
    //    zfysetting->moveToThread(setupthread);
    //    setupthread->start();
    connect(this,SIGNAL(togetSetting(int)),zfysetting,SLOT(togetZFYSettings(int)));
    connect(this,SIGNAL(towriteSetting(int,QString)),zfysetting,SLOT(tosetZFY(int,QString)));


    connect(this,SIGNAL(towriteSettingList(int,QStringList)),zfysetting,SLOT(tosetlistZFY(int,QStringList)));

    connect(this,SIGNAL(towriteListofSettingList(QList<int>,QStringList)),zfysetting,SLOT(tosetlistZFYlist(QList<int>,QStringList)));

    //connect(zfysetting,SIGNAL(setListsuccess()),this,SLOT(tosetZFY()));
    ui->comboBox_mylist->setItemData(0,QVariant("AS"),Qt::UserRole-1);
    //ui->comboBox_language->setItemData(0,QVariant("AS"),Qt::UserRole-1);
    qDebug()<<"SetUpForm:get number:"<<connectnumber;
    if(!connectnumber.isEmpty()){
        for (int i=0;i<connectnumber.count();i++) {
            ui->comboBox_mylist->addItem(QString::number(connectnumber.at(i)+1));

            ui->comboBox_change->removeItem(connectnumber.at(i)-i);
        }

    }

    //ui->comboBox_mylist->setCurrentIndex(-1);

    initProcess();

}
void SetUpForm::setLanguage(QString language){

    Config().Set("language","setlanguagefile",language);
}

void SetUpForm::getLanguage()
{
    QString language = Config().Get("language","setlanguagefile").toString();
    if(language == "lang_en.qm"){
        ui->comboBox_language->setCurrentIndex(0);
    }else if(language == "lang_cn.qm"){
        ui->comboBox_language->setCurrentIndex(1);
    }else if(language == "lang_ru.qm"){
        ui->comboBox_language->setCurrentIndex(2);
    }
    isAutoGetLanguage = true;
}
SetUpForm::~SetUpForm()
{
    qDebug()<<"~SetUpForm!!!";
    //    if(setupthread->isRunning())
    //    {
    //        //zfy1->closeThread();  //关闭线程槽函数
    //        setupthread->quit();            //退出事件循环
    //        setupthread->wait();            //释放线程槽函数资源
    //    }
    //    delete setupthread;
    delete ui;

}

void SetUpForm::on_pushButton_goBack_clicked()
{

    emit goback();

}

void SetUpForm::on_pushButton_change_clicked()
{


    if(ui->comboBox_mylist->currentIndex() != 0){


        //start to change

        int number_old = ui->comboBox_mylist->currentText().toInt();
        int number_change = ui->comboBox_change->currentText().toInt();

        qDebug()<<"ui->comboBox_mylist->currentText()"<<ui->comboBox_mylist->currentText();



        changeUdevFile(number_old,number_change);


        changeConfig(number_old,number_change);

        QThread::msleep(1000);

        restartUdevService();



    }
}


void SetUpForm::changeUdevFile(int number_old,int number_change){


    // read file: /etc/udev/rules.d/00-usb-permissions.rules

    QList<QString> mylist;
    QFile file("/etc/udev/rules.d/00-usb-permissions.rules");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        //qDebug()<<line;
        mylist.append(line);
    }

    file.close();

    //get the kernels
    QRegExp KERNELSText("(KERNELS==\\\"(\\d-(\\d.){0,}))");
    KERNELSText.indexIn(mylist.at(number_old));
    QString kernels_old = KERNELSText.cap(1);
    qDebug()<<"kernels_old"<<kernels_old;


    KERNELSText.indexIn(mylist.at(number_change));
    QString kernels_change = KERNELSText.cap(1);
    qDebug()<<"kernels_change"<<kernels_change;


    mylist.operator [](number_old).replace(kernels_old, kernels_change);
    mylist.operator [](number_change).replace(kernels_change, kernels_old);

    //write file: /etc/udev/rules.d/00-usb-permissions.rules


    QFile file_write("/etc/udev/rules.d/00-usb-permissions.rules");
    //打开文件,只写方式
    bool isOK = file_write.open(QIODevice::WriteOnly);
    if (isOK == true){
        for (int i=0;i<mylist.count();i++) {
            file_write.write(mylist.at(i).toStdString().data());
        }

    }

    file_write.close();

}
void SetUpForm::changeConfig(int number_old,int number_change){

    qDebug()<<"change Config file!";
    QString port_old = QString("%1%2").arg("port").arg(number_old-1);
    QString port_change = QString("%1%2").arg("port").arg(number_change-1);

    QVariant old_number = Config().Get("portInfo",port_old).toString();
    QVariant change_number = Config().Get("portInfo",port_change).toString();

    Config().Set("portInfo",port_old,change_number);
    Config().Set("portInfo",port_change,old_number);


}
void SetUpForm::restartUdevService(){

    qDebug()<<"restart udev service!";


    QString bash_text = "/etc/init.d/udev restart";
    m_proces_bash->write(bash_text.toLocal8Bit() + '\n');
}

void SetUpForm::initProcess()
{
    m_proces_bash = new QProcess;
    m_proces_bash->start("bash");
    m_proces_bash->waitForStarted();
    qDebug()<<"initProcess";
    connect(m_proces_bash,SIGNAL(readyReadStandardOutput()),this,SLOT(readBashStandardOutputInfo()));
    connect(m_proces_bash,SIGNAL(readyReadStandardError()),this,SLOT(readBashStandardErrorInfo()));
}

void SetUpForm::readBashStandardOutputInfo()
{
    qDebug()<<"readBashStandardOutputInfo";
    QByteArray cmdout = m_proces_bash->readAllStandardOutput();
    if(!cmdout.isEmpty()){
        //ui->textEdit_bashmsg->append(QString::fromLocal8Bit(cmdout));
        qDebug()<<"readBashStandardOutputInfo"<<QString::fromLocal8Bit(cmdout);

        if(QString::fromLocal8Bit(cmdout).contains("Restarting udev (via systemctl): udev.service")){
            //emit changesuccess(ui->comboBox_mylist->currentText().toInt()-1);
            QMessageBox::StandardButton rb = QMessageBox::warning(this, tr("Info"), tr("Change success,you need to reconnect the device!"), QMessageBox::Ok );
            if(rb == QMessageBox::Ok)
            {
                //exit(1);
                emit changesuccess(ui->comboBox_mylist->currentText().toInt()-1);
            }
        }
    }
}

void SetUpForm::readBashStandardErrorInfo()
{
    qDebug()<<"readBashStandardErrorInfo";
    QByteArray cmdout = m_proces_bash->readAllStandardError();
    if(!cmdout.isEmpty()){
        //ui->textEdit_bashmsg->append(QString::fromLocal8Bit(cmdout));
        qDebug()<<"readBashStandardErrorInfo"<<QString::fromLocal8Bit(cmdout);
    }
}



void SetUpForm::on_comboBox_mylist_currentIndexChanged(int index)
{
    qDebug()<<"on_comboBox_mylist_currentIndexChanged:"<<index;
    isUserSelect = false;
    //go to get setting
    if(index != 0){
        //dasbuddy[ui->comboBox_mylist->currentText().toInt()-1]->getAllSetting();



        emit togetSetting(ui->comboBox_mylist->currentText().toInt()-1);



        setting_dialog = new getSettingDialog(this);
        setting_dialog->show();
        //connect(this,SIGNAL(getsettingfinish()),setting_dialog,SLOT(getfinish()));
        connect(setting_dialog,SIGNAL(accepted()),this,SLOT(deleteSettingDialog()));

    }else {
        setcomboxdisable();
    }


}
void SetUpForm::deleteSettingDialog(){
    //insertZFYDialog->hide();
    qDebug()<<"delete SettingDialog!";
    setting_dialog->deleteLater();
    ui->comboBox_mylist->setCurrentIndex(0);
}

void SetUpForm::deletewriteSettingDialog()
{
    qDebug()<<"delete writeSettingDialog!";
    writesetting_dialog->deleteLater();
}
void SetUpForm::recieveAllSetting(QVariantMap map)
{
    setcomboxenable();
    //emit getsettingfinish();

    setting_dialog->hide();
    setting_dialog->deleteLater();

    ui->lineEdit_UserId->setText(map.value("userid").toString());
    ui->lineEdit_DriverId->setText(map.value("driverid").toString());

    QRegExp getSettingText("#(\\d{1,2})");
    qDebug()<<"recieveAllSetting_ moviesize:"<<map.value("moviesize","#-1");

    getSettingText.indexIn(map.value("moviesize","#-1").toString());
    qDebug()<<"getSettingText:"<<getSettingText.cap(1);
    set_combobox_text(ui->comboBox_Moviesize,getSettingText.cap(1).toInt());

    getSettingText.indexIn(map.value("photosize","#-1").toString());
    set_combobox_text(ui->comboBox_Photosize,getSettingText.cap(1).toInt());

    getSettingText.indexIn(map.value("moviecycTime","#-1").toString());
    set_combobox_text(ui->comboBox_MoviecycTime,getSettingText.cap(1).toInt());

    getSettingText.indexIn(map.value("moviecyc","#-1").toString());
    set_combobox_text(ui->comboBox_Moviecyc,getSettingText.cap(1).toInt());

    getSettingText.indexIn(map.value("dateWater","#-1").toString());
    set_combobox_text(ui->comboBox_DateWater,getSettingText.cap(1).toInt());

    getSettingText.indexIn(map.value("gsensor","#-1").toString());
    set_combobox_text(ui->comboBox_Gsensor,getSettingText.cap(1).toInt());

    getSettingText.indexIn(map.value("moviecodec","#-1").toString());
    set_combobox_text(ui->comboBox_Moviecodec,getSettingText.cap(1).toInt());

    getSettingText.indexIn(map.value("prerec","#-1").toString());
    set_combobox_text(ui->comboBox_Prerec,getSettingText.cap(1).toInt());

    getSettingText.indexIn(map.value("delayrec","#-1").toString());
    set_combobox_text(ui->comboBox_Delayrec,getSettingText.cap(1).toInt());

    getSettingText.indexIn(map.value("gPS","#-1").toString());
    set_combobox_text(ui->comboBox_GPS,getSettingText.cap(1).toInt());

    getSettingText.indexIn(map.value("motiondet","#-1").toString());
    set_combobox_text(ui->comboBox_Motiondet,getSettingText.cap(1).toInt());

    getSettingText.indexIn(map.value("ircut","#-1").toString());
    set_combobox_text(ui->comboBox_Ircut,getSettingText.cap(1).toInt());

    getSettingText.indexIn(map.value("recmode","#-1").toString());
    set_combobox_text(ui->comboBox_Recmode,getSettingText.cap(1).toInt());

    isUserSelect = true;
}
void SetUpForm::set_combobox_text(QComboBox *box,int num){

    if(box == ui->comboBox_Moviesize){
        if(num ==6){
            box->setCurrentIndex(0);
        }else if (num ==7) {
            box->setCurrentIndex(1);
        } else if (num ==10) {
            box->setCurrentIndex(2);
        }else if (num ==11) {
            box->setCurrentIndex(3);
        }else if (num ==14) {
            box->setCurrentIndex(4);
        }else if (num ==15) {
            box->setCurrentIndex(5);
        }else if (num ==16) {
            box->setCurrentIndex(6);
        }else if (num ==18) {
            box->setCurrentIndex(7);
        }
    }else if (box == ui->comboBox_Photosize) {


        if(num ==0){
            box->setCurrentIndex(0);
        }else if (num ==1) {
            box->setCurrentIndex(1);
        } else if (num ==2) {
            box->setCurrentIndex(2);
        }else if (num ==3) {
            box->setCurrentIndex(3);
        }else if (num ==4) {
            box->setCurrentIndex(4);
        }else if (num ==5) {
            box->setCurrentIndex(5);
        }else if (num ==6) {
            box->setCurrentIndex(6);
        }else if (num ==7) {
            box->setCurrentIndex(7);
        }

    }else if (box == ui->comboBox_MoviecycTime) {


        if(num ==0){
            box->setCurrentIndex(0);
        }else if (num ==1) {
            box->setCurrentIndex(1);
        } else if (num ==2) {
            box->setCurrentIndex(2);
        }else if (num ==3) {
            box->setCurrentIndex(3);
        }else if (num ==4) {
            box->setCurrentIndex(4);
        }else if (num ==5) {
            box->setCurrentIndex(5);
        }
    }else if (box == ui->comboBox_Moviecyc) {


        if(num ==1){
            box->setCurrentIndex(0);
        }else if (num ==0) {
            box->setCurrentIndex(1);
        }
    }else if (box == ui->comboBox_DateWater) {


        if(num ==1){
            box->setCurrentIndex(0);
        }else if (num ==0) {
            box->setCurrentIndex(1);
        }
    }else if (box == ui->comboBox_Gsensor) {


        if(num ==0){
            box->setCurrentIndex(0);
        }else if (num ==1) {
            box->setCurrentIndex(1);
        }else if (num ==2) {
            box->setCurrentIndex(2);
        }
        else if (num ==3) {
            box->setCurrentIndex(3);
        }
    }
    else if (box == ui->comboBox_Moviecodec) {


        if(num ==0){
            box->setCurrentIndex(0);
        }else if (num ==1) {
            box->setCurrentIndex(1);
        }
    }else if (box == ui->comboBox_Prerec) {


        if(num ==0){
            box->setCurrentIndex(0);
        }else if (num ==1) {
            box->setCurrentIndex(1);
        }
    }else if (box == ui->comboBox_Delayrec) {


        if(num ==0){
            box->setCurrentIndex(0);
        }else if (num ==1) {
            box->setCurrentIndex(1);
        } else if (num ==2) {
            box->setCurrentIndex(2);
        }else if (num ==3) {
            box->setCurrentIndex(3);
        }

    }else if (box == ui->comboBox_GPS) {


        if(num ==0){
            box->setCurrentIndex(0);
        }else if (num ==1) {
            box->setCurrentIndex(1);
        }
    }else if (box == ui->comboBox_Motiondet) {


        if(num ==0){
            box->setCurrentIndex(0);
        }else if (num ==1) {
            box->setCurrentIndex(1);
        }
    }else if (box == ui->comboBox_Ircut) {


        if(num ==0){
            box->setCurrentIndex(0);
        }else if (num ==1) {
            box->setCurrentIndex(1);
        }
    }else if (box == ui->comboBox_Recmode) {


        if(num ==0){
            box->setCurrentIndex(0);
        }else if (num ==1) {
            box->setCurrentIndex(1);
        }else if (num ==2) {
            box->setCurrentIndex(2);
        }else if (num ==3) {
            box->setCurrentIndex(3);
        }
    }
}

void SetUpForm::setcomboxdisable()
{
    ui->lineEdit_UserId->setEnabled(false);
    ui->lineEdit_DriverId->setEnabled(false);
    ui->comboBox_change->setEnabled(false);
    ui->comboBox_Moviesize->setEnabled(false);
    ui->comboBox_Photosize->setEnabled(false);
    ui->comboBox_MoviecycTime->setEnabled(false);
    ui->comboBox_Moviecyc->setEnabled(false);
    ui->comboBox_DateWater->setEnabled(false);
    ui->comboBox_Gsensor->setEnabled(false);
    ui->comboBox_Moviecodec->setEnabled(false);
    ui->comboBox_Prerec->setEnabled(false);
    ui->comboBox_Delayrec->setEnabled(false);
    ui->comboBox_GPS->setEnabled(false);
    ui->comboBox_Motiondet->setEnabled(false);
    ui->comboBox_Ircut->setEnabled(false);
    ui->comboBox_Recmode->setEnabled(false);
}

void SetUpForm::setcomboxenable()
{

    ui->lineEdit_UserId->setEnabled(true);
    ui->lineEdit_DriverId->setEnabled(true);
    ui->comboBox_change->setEnabled(true);
    ui->comboBox_Moviesize->setEnabled(true);
    ui->comboBox_Photosize->setEnabled(true);
    ui->comboBox_MoviecycTime->setEnabled(true);
    ui->comboBox_Moviecyc->setEnabled(true);
    ui->comboBox_DateWater->setEnabled(true);
    ui->comboBox_Gsensor->setEnabled(true);
    ui->comboBox_Moviecodec->setEnabled(true);
    ui->comboBox_Prerec->setEnabled(true);
    ui->comboBox_Delayrec->setEnabled(true);
    ui->comboBox_GPS->setEnabled(true);
    ui->comboBox_Motiondet->setEnabled(true);
    ui->comboBox_Ircut->setEnabled(true);
    ui->comboBox_Recmode->setEnabled(true);
}





void SetUpForm::on_comboBox_Moviesize_currentIndexChanged(int index)
{
    //need to judge the change is userchoose or getsetting change
    if(isUserSelect){
        qDebug()<<"on_comboBox_Moviesize_isUserSelect:"<<isUserSelect;
        qDebug()<<"on_comboBox_Moviesize_currentIndexChanged"<<index;
        qDebug()<<"on_comboBox_Moviesize_currentIndexChanged"<<ui->comboBox_Moviesize->currentText();
        if(settingnum != -1){
            //dasbuddy[ui->comboBox_mylist->currentText().toInt()-1]->getAllSetting();
            //dasbuddy[ui->comboBox_mylist->currentText().toInt()-1]->tosetText("@"+ui->comboBox_Moviesize->currentText());
            //towriteSetting(settingnum,"@"+ui->comboBox_Moviesize->currentText());

        }

    }else {

        qDebug()<<"on_comboBox_Moviesize_isUserSelect:"<<isUserSelect;
    }
}

void SetUpForm::on_comboBox_Photosize_currentIndexChanged(int index)
{
    if(isUserSelect){
        qDebug()<<"on_comboBox_Photosize_isUserSelect:"<<isUserSelect;
        qDebug()<<"on_comboBox_Photosize_currentIndexChanged"<<index;
        qDebug()<<"on_comboBox_Photosize_currentIndexChanged"<<ui->comboBox_Photosize->currentText();
        if(settingnum != -1){
            //dasbuddy[ui->comboBox_mylist->currentText().toInt()-1]->getAllSetting();
            //dasbuddy[ui->comboBox_mylist->currentText().toInt()-1]->tosetText("@"+ui->comboBox_Photosize->currentText());
            //towriteSetting(settingnum,"@"+ui->comboBox_Photosize->currentText());
        }

    }else {

        qDebug()<<"on_comboBox_Photosize_isUserSelect:"<<isUserSelect;
    }
}

void SetUpForm::on_comboBox_MoviecycTime_currentIndexChanged(int index)
{
    if(isUserSelect){
        qDebug()<<"on_comboBox_MoviecycTime_isUserSelect:"<<isUserSelect;
        qDebug()<<"on_comboBox_MoviecycTime_currentIndexChanged"<<index;
        qDebug()<<"on_comboBox_MoviecycTime_currentIndexChanged"<<ui->comboBox_MoviecycTime->currentText();
        if(settingnum != -1){
            //dasbuddy[ui->comboBox_mylist->currentText().toInt()-1]->getAllSetting();
            //dasbuddy[ui->comboBox_mylist->currentText().toInt()-1]->tosetText("@"+ui->comboBox_MoviecycTime->currentText());
            //towriteSetting(settingnum,"@"+ui->comboBox_MoviecycTime->currentText());
        }

    }else {

        qDebug()<<"on_comboBox_MoviecycTime_isUserSelect:"<<isUserSelect;
    }
}

void SetUpForm::on_comboBox_Moviecyc_currentIndexChanged(int index)
{
    if(isUserSelect){
        qDebug()<<"on_comboBox_Moviecyc_isUserSelect:"<<isUserSelect;
        qDebug()<<"on_comboBox_Moviecyc_currentIndexChanged"<<index;
        qDebug()<<"on_comboBox_Moviecyc_currentIndexChanged"<<ui->comboBox_Moviecyc->currentText();
        if(settingnum != -1){

            //dasbuddy[ui->comboBox_mylist->currentText().toInt()-1]->tosetText("@"+ui->comboBox_Moviecyc->currentText());
            //towriteSetting(settingnum,"@"+ui->comboBox_Moviecyc->currentText());
        }

    }
}

void SetUpForm::on_comboBox_DateWater_currentIndexChanged(int index)
{
    if(isUserSelect){
        qDebug()<<"on_comboBox_DateWater_isUserSelect:"<<isUserSelect;
        qDebug()<<"on_comboBox_DateWater_currentIndexChanged"<<index;
        qDebug()<<"on_comboBox_DateWater_currentIndexChanged"<<ui->comboBox_DateWater->currentText();
        if(settingnum != -1){

            //dasbuddy[ui->comboBox_mylist->currentText().toInt()-1]->tosetText("@"+ui->comboBox_DateWater->currentText());
            //towriteSetting(settingnum,"@"+ui->comboBox_DateWater->currentText());
        }

    }
}

void SetUpForm::on_comboBox_Gsensor_currentIndexChanged(int index)
{
    if(isUserSelect){
        qDebug()<<"on_comboBox_Gsensor_isUserSelect:"<<isUserSelect;
        qDebug()<<"on_comboBox_Gsensor_currentIndexChanged"<<index;
        qDebug()<<"on_comboBox_Gsensor_currentIndexChanged"<<ui->comboBox_Gsensor->currentText();
        if(settingnum != -1){

            //dasbuddy[ui->comboBox_mylist->currentText().toInt()-1]->tosetText("@"+ui->comboBox_Gsensor->currentText());
            //towriteSetting(settingnum,"@"+ui->comboBox_Gsensor->currentText());
        }

    }
}

void SetUpForm::on_comboBox_Moviecodec_currentIndexChanged(int index)
{
    if(isUserSelect){
        qDebug()<<"on_comboBox_Moviecodec_isUserSelect:"<<isUserSelect;
        qDebug()<<"on_comboBox_Moviecodec_currentIndexChanged"<<index;
        qDebug()<<"on_comboBox_Moviecodec_currentIndexChanged"<<ui->comboBox_Moviecodec->currentText();
        if(settingnum != -1){

            //dasbuddy[ui->comboBox_mylist->currentText().toInt()-1]->tosetText("@"+ui->comboBox_Moviecodec->currentText());
            //towriteSetting(settingnum,"@"+ui->comboBox_Moviecodec->currentText());
        }

    }
}

void SetUpForm::on_comboBox_Prerec_currentIndexChanged(int index)
{
    if(isUserSelect){
        qDebug()<<"on_comboBox_Prerec_isUserSelect:"<<isUserSelect;
        qDebug()<<"on_comboBox_Prerec_currentIndexChanged"<<index;
        qDebug()<<"on_comboBox_Prerec_currentIndexChanged"<<ui->comboBox_Prerec->currentText();
        if(settingnum != -1){

            //dasbuddy[ui->comboBox_mylist->currentText().toInt()-1]->tosetText("@"+ui->comboBox_Prerec->currentText());
            //towriteSetting(settingnum,"@"+ui->comboBox_Prerec->currentText());
        }

    }
}


void SetUpForm::on_comboBox_Delayrec_currentIndexChanged(int index)
{
    if(isUserSelect){
        qDebug()<<"on_comboBox_Delayrec_isUserSelect:"<<isUserSelect;
        qDebug()<<"on_comboBox_Delayrec_currentIndexChanged"<<index;
        qDebug()<<"on_comboBox_Delayrec_currentIndexChanged"<<ui->comboBox_Delayrec->currentText();
        if(settingnum != -1){

            //dasbuddy[ui->comboBox_mylist->currentText().toInt()-1]->tosetText("@"+ui->comboBox_Delayrec->currentText());
            //towriteSetting(settingnum,"@"+ui->comboBox_Delayrec->currentText());
        }

    }
}


void SetUpForm::on_comboBox_GPS_currentIndexChanged(int index)
{
    if(isUserSelect){
        qDebug()<<"on_comboBox_GPS_isUserSelect:"<<isUserSelect;
        qDebug()<<"on_comboBox_GPS_currentIndexChanged"<<index;
        qDebug()<<"on_comboBox_GPS_currentIndexChanged"<<ui->comboBox_GPS->currentText();
        if(settingnum != -1){

            //dasbuddy[ui->comboBox_mylist->currentText().toInt()-1]->tosetText("@"+ui->comboBox_GPS->currentText());
            //towriteSetting(settingnum,"@"+ui->comboBox_GPS->currentText());
        }

    }
}


void SetUpForm::on_comboBox_Motiondet_currentIndexChanged(int index)
{
    if(isUserSelect){
        qDebug()<<"on_comboBox_Motiondet_isUserSelect:"<<isUserSelect;
        qDebug()<<"on_comboBox_Motiondet_currentIndexChanged"<<index;
        qDebug()<<"on_comboBox_Motiondet_currentIndexChanged"<<ui->comboBox_Motiondet->currentText();
        if(settingnum != -1){

            //dasbuddy[ui->comboBox_mylist->currentText().toInt()-1]->tosetText("@"+ui->comboBox_Motiondet->currentText());
            //towriteSetting(settingnum,"@"+ui->comboBox_Motiondet->currentText());
        }

    }
}

void SetUpForm::on_comboBox_Ircut_currentIndexChanged(int index)
{
    if(isUserSelect){
        qDebug()<<"on_comboBox_Ircut_isUserSelect:"<<isUserSelect;
        qDebug()<<"on_comboBox_Ircut_currentIndexChanged"<<index;
        qDebug()<<"on_comboBox_Ircut_currentIndexChanged"<<ui->comboBox_Ircut->currentText();
        if(settingnum != -1){

            //dasbuddy[ui->comboBox_mylist->currentText().toInt()-1]->tosetText("@"+ui->comboBox_Ircut->currentText());
            //towriteSetting(settingnum,"@"+ui->comboBox_Ircut->currentText());
        }

    }
}

void SetUpForm::on_comboBox_Recmode_currentIndexChanged(int index)
{
    if(isUserSelect){
        qDebug()<<"on_comboBox_Recmode_isUserSelect:"<<isUserSelect;
        qDebug()<<"on_comboBox_Recmode_currentIndexChanged"<<index;
        qDebug()<<"on_comboBox_Recmode_currentIndexChanged"<<ui->comboBox_Recmode->currentText();
        if(settingnum != -1){

            //dasbuddy[ui->comboBox_mylist->currentText().toInt()-1]->tosetText("@"+ui->comboBox_Recmode->currentText());
            //towriteSetting(settingnum,"@"+ui->comboBox_Recmode->currentText());
        }

    }
}

//void SetUpForm::on_btn_setDate_clicked()
//{

//    if(isUserSelect){
//        qDebug()<<"on_btn_setDate_isUserSelect:"<<isUserSelect;

//        QString writedate = ui->dateTimeEdit->date().toString("yyyyMMdd");
//        QString writetime = ui->dateTimeEdit->time().toString("hhmmss");

//        qDebug()<<"on_btn_setDate date:"<<writedate;
//        qDebug()<<"on_btn_setDate time:"<<writetime;
//        if(settingnum != -1){

//            //dasbuddy[ui->comboBox_mylist->currentText().toInt()-1]->tosetText("@"+ui->comboBox_Recmode->currentText());
//            //towriteSetting(settingnum,"@Sdt"+writedate);

//            //QThread::msleep(1500);
//            //towriteSetting(settingnum,"@Stm"+writetime);
//        }

//    }
//}



void SetUpForm::on_comboBox_language_currentIndexChanged(int index)
{
    if(isAutoGetLanguage){
        qDebug()<<"on_comboBox_language_currentIndexChanged"<<index;
        if(index ==0){
            setLanguage("lang_en.qm");
        }else if(index == 1){
            setLanguage("lang_cn.qm");
        }else if(index == 2){
            setLanguage("lang_ru.qm");
        }

        QMessageBox::warning(NULL, tr("INFO"), tr("change language success, the language will change next start!"), QMessageBox::Ok );
    }



}


void SetUpForm::on_listWidget_itemClicked(QListWidgetItem *item)
{

    int num = item->listWidget()->currentRow();
    qDebug()<<"list widget clicked item:"<<num;

    if(!dasbuddy[num]->getIsConnect()){
        qDebug()<<"please click the connect zfy!";
    }else{
        dasbuddy[num]->clearuploadsize();
        //dasbuddy[num]->setIsConnect(false);
        settingnum = num;
        emit togetSetting(num);

        setting_dialog = new getSettingDialog(this);
        setting_dialog->show();
        //connect(this,SIGNAL(getsettingfinish()),setting_dialog,SLOT(getfinish()));
        connect(setting_dialog,SIGNAL(accepted()),this,SLOT(deleteSettingDialog()));


    }
}

void SetUpForm::on_btn_write_clicked()
{


    if(settingnum != -1){

        writesetting_dialog = new writeSettingDialog(this);
        writesetting_dialog->show();
        //connect(zfysetting,SIGNAL(setListsuccess()),this,SLOT(tosetZFY()));
        connect(zfysetting,SIGNAL(setListsuccess()),writesetting_dialog,SLOT(writefinish()));
        connect(writesetting_dialog,SIGNAL(accepted()),this,SLOT(deletewriteSettingDialog()));

        QStringList settinglist;
        if(ui->lineEdit_UserId->text().length() ==6){
            qDebug()<<"ui->lineEdit_UserId->text().length()"<<ui->lineEdit_UserId->text().length();
            settinglist.append("@UserID#"+ui->lineEdit_UserId->text());
        }
        if(ui->lineEdit_DriverId->text().length() ==7){
            qDebug()<<"ui->lineEdit_DriverId->text().length()"<<ui->lineEdit_DriverId->text().length();
            settinglist.append("@DriveID#"+ui->lineEdit_DriverId->text());
        }
        settinglist.append("@"+ui->comboBox_Moviesize->currentText());
        settinglist.append("@"+ui->comboBox_Photosize->currentText());
        settinglist.append("@"+ui->comboBox_MoviecycTime->currentText());
        settinglist.append("@"+ui->comboBox_DateWater->currentText());
        settinglist.append("@"+ui->comboBox_Moviecyc->currentText());
        settinglist.append("@"+ui->comboBox_Gsensor->currentText());
        settinglist.append("@"+ui->comboBox_Moviecodec->currentText());
        settinglist.append("@"+ui->comboBox_Prerec->currentText());
        settinglist.append("@"+ui->comboBox_Delayrec->currentText());
        settinglist.append("@"+ui->comboBox_GPS->currentText());
        settinglist.append("@"+ui->comboBox_Motiondet->currentText());
        settinglist.append("@"+ui->comboBox_Ircut->currentText());
        settinglist.append("@"+ui->comboBox_Recmode->currentText());
        QString writedate = ui->dateTimeEdit->date().toString("yyyyMMdd");
        QString writetime = ui->dateTimeEdit->time().toString("hhmmss");


        qDebug()<<"on_btn_setDate date:"<<writedate;
        qDebug()<<"on_btn_setDate time:"<<writetime;


        settinglist.append("@Sdt#"+writedate);

        settinglist.append("@Stm#"+writetime);

        emit towriteSettingList(settingnum,settinglist);


        //        emit towriteSetting(settingnum,"@"+ui->comboBox_Moviesize->currentText());
        //        //QThread::msleep(1500);

        //        emit towriteSetting(settingnum,"@"+ui->comboBox_Photosize->currentText());
        //        //QThread::msleep(1500);

        //        emit towriteSetting(settingnum,"@"+ui->comboBox_MoviecycTime->currentText());
        //        //QThread::msleep(1500);

        //        emit towriteSetting(settingnum,"@"+ui->comboBox_DateWater->currentText());
        //        //QThread::msleep(1500);

        //        emit towriteSetting(settingnum,"@"+ui->comboBox_Moviecyc->currentText());
        //        //QThread::msleep(1500);

        //        emit towriteSetting(settingnum,"@"+ui->comboBox_Gsensor->currentText());
        //        //QThread::msleep(1500);

        //        emit towriteSetting(settingnum,"@"+ui->comboBox_Moviecodec->currentText());
        //        //QThread::msleep(1500);

        //        emit towriteSetting(settingnum,"@"+ui->comboBox_Prerec->currentText());
        //        //QThread::msleep(1500);

        //        emit towriteSetting(settingnum,"@"+ui->comboBox_Delayrec->currentText());
        //        //QThread::msleep(1500);

        //        emit towriteSetting(settingnum,"@"+ui->comboBox_GPS->currentText());
        //        //QThread::msleep(1500);

        //        emit towriteSetting(settingnum,"@"+ui->comboBox_Motiondet->currentText());
        //        //QThread::msleep(1500);

        //        emit towriteSetting(settingnum,"@"+ui->comboBox_Ircut->currentText());
        //        //QThread::msleep(1500);

        //        emit towriteSetting(settingnum,"@"+ui->comboBox_Recmode->currentText());
        //        //QThread::msleep(1500);


        //        QString writedate = ui->dateTimeEdit->date().toString("yyyyMMdd");
        //        QString writetime = ui->dateTimeEdit->time().toString("hhmmss");


        //        qDebug()<<"on_btn_setDate date:"<<writedate;
        //        qDebug()<<"on_btn_setDate time:"<<writetime;

        //        towriteSetting(settingnum,"@Sdt"+writedate);

        //        //QThread::msleep(1500);
        //        towriteSetting(settingnum,"@Stm"+writetime);
        //        QThread::msleep(2000);
        //        emit writesettingfinish();
    }else{
        qDebug()<<"please select one zfy";
    }



}



void SetUpForm::on_btn_writeAll_clicked()
{
    QList<int> connectlist;
    for (int i=0;i<30;i++) {
        if(dasbuddy[i]->getIsConnect()){
            connectlist.append(i);
        }
    }
    if(!connectlist.isEmpty()){
        writesetting_dialog = new writeSettingDialog(this);
        writesetting_dialog->show();
        //connect(zfysetting,SIGNAL(setListsuccess()),this,SLOT(tosetZFY()));
        connect(zfysetting,SIGNAL(setoneListsuccess(int)),writesetting_dialog,SLOT(writeonefinish(int)));
        connect(zfysetting,SIGNAL(setallListsuccess()),writesetting_dialog,SLOT(writeAllfinish()));
        connect(writesetting_dialog,SIGNAL(accepted()),this,SLOT(deletewriteSettingDialog()));


        QStringList settinglist;
        settinglist.append("@"+ui->comboBox_Moviesize->currentText());
        settinglist.append("@"+ui->comboBox_Photosize->currentText());
        settinglist.append("@"+ui->comboBox_MoviecycTime->currentText());
        settinglist.append("@"+ui->comboBox_DateWater->currentText());
        settinglist.append("@"+ui->comboBox_Moviecyc->currentText());
        settinglist.append("@"+ui->comboBox_Gsensor->currentText());
        settinglist.append("@"+ui->comboBox_Moviecodec->currentText());
        settinglist.append("@"+ui->comboBox_Prerec->currentText());
        settinglist.append("@"+ui->comboBox_Delayrec->currentText());
        settinglist.append("@"+ui->comboBox_GPS->currentText());
        settinglist.append("@"+ui->comboBox_Motiondet->currentText());
        settinglist.append("@"+ui->comboBox_Ircut->currentText());
        settinglist.append("@"+ui->comboBox_Recmode->currentText());
        QString writedate = ui->dateTimeEdit->date().toString("yyyyMMdd");
        QString writetime = ui->dateTimeEdit->time().toString("hhmmss");


        qDebug()<<"on_btn_setDate date:"<<writedate;
        qDebug()<<"on_btn_setDate time:"<<writetime;


        settinglist.append("@Sdt#"+writedate);

        settinglist.append("@Stm#"+writetime);


        emit towriteListofSettingList(connectlist,settinglist);
    }
}
