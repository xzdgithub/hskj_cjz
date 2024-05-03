#include "dasbuddy.h"
#include<QHBoxLayout>
#include<QVBoxLayout>
#include<QStackedLayout>


#define RETRY_MAX                     5
#define BOMS_GET_MAX_LUN              0xFE
static char* MANUFACTURER= "NOVATEK\0";
static uint8_t ENDPOINT_OUT = 0x02;
static uint8_t ENDPOINT_IN = 0x81;

// Section 5.2: Command Status Wrapper (CSW)
struct command_status_wrapper {
    uint8_t dCSWSignature[4];
    uint32_t dCSWTag;
    uint32_t dCSWDataResidue;
    uint8_t bCSWStatus;
};


static const uint8_t cdb_length[256] = {
    //	 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
    06,06,06,06,06,06,06,06,06,06,06,06,06,06,06,06,  //  0
    06,06,06,06,06,06,06,06,06,06,06,06,06,06,06,06,  //  1
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  //  2
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  //  3
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  //  4
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  //  5
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,  //  6
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,  //  7
    16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,  //  8
    16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,  //  9
    12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  //  A
    12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  //  B
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,  //  C
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,  //  D
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,  //  E
    00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,  //  F
};
#define ERR_EXIT(errcode) do { qDebug()<<"error: "<<errcode<<"   "<<libusb_strerror((enum libusb_error)errcode); return -1; } while (0)

#define CALL_CHECK(fcall) do { int _r=fcall; if (_r < 0) ERR_EXIT(_r); } while (0)





DASBuddy::DASBuddy(bool deletefile,QWidget *parent) : QWidget(parent)
{


    isConnect = false;
    //initProcessBar();
    this->deletefile = deletefile;


    zfy = new ZFYControl(deletefile);

    controlthread = new QThread(this);
    zfy->moveToThread(controlthread);

    connect(controlthread,SIGNAL(finished()),this,SLOT(finishedThreadSlot()));


    //controlthread->start();
    qDebug()<<"DasBuddy QThread::currentThreadId()=="<<QThread::currentThreadId();


    connect(zfy,SIGNAL(copyonesuccess(QVariant,qreal,int)),this,SLOT(updateProgress(QVariant,qreal,int)));

    connect(this,SIGNAL(startToMount(int)),zfy,SLOT(mountToUdisk(int)));

    connect(zfy,SIGNAL(start_mount(int)),this,SLOT(changewriting()));

    connect(zfy,SIGNAL(mount_finish(int)),this,SLOT(checkmount(int)));

    connect(this,SIGNAL(mountsuccess(int)),this,SLOT(waittoCopy(int)));

    connect(this,SIGNAL(mountsuccess(int)),this,SLOT(postTogetNameAndimage()));

    connect(zfy,SIGNAL(mount_fail()),this,SLOT(changewritingfail()));

    //checkmount


    //connect(this,SIGNAL(startCopy(QVariantMap)),zfy,SLOT(copyFile2(QVariantMap)));


    //connect(controlthread,SIGNAL(finished()),zfy,SLOT(deleteLater()));



    initUi();

    myDataVm = new DataVM();
    connect(myDataVm, SIGNAL(sigCopyFileProgress(float)), this, SLOT(slotProgressBar(float)));
    connect(myDataVm, SIGNAL(sigAllCopyFilesFinished(bool)), this, SLOT(slotCopyFinished(bool)));
    connect(myDataVm, SIGNAL(copyonesuccess(QString,QString)), this, SLOT(slotCopyonesuccess(QString,QString)));


}

DASBuddy::~DASBuddy()
{
    qDebug()<<"DASBuddy::~DASBuddy "<<this->num<<endl;
}
void DASBuddy::initUi(){





    frame_background = new QFrame(this);
    frame_background->setProperty("shine",false);
    UploadText = new QLabel(this);

    UploadProcessText = new QLabel(this);
    UserName = new QLabel(this);
    UserNameText = new QLabel(this);
    label_photo = new QLabel(this);

    BatteryText = new QLabel(this);
    Battery = new QLabel(this);
    frame_text = new QFrame(this);


    label_photo->setFixedSize(135,100);

    label_photo->setStyleSheet("QLabel{background-color: white;border:2px solid black;border-radius:10px;}");

    frame_background->setStyleSheet("background-color: white");

    frame_text->setFixedSize(135,70);
    frame_text->setStyleSheet("QFrame{background-color: white;border:2px solid black;border-radius:10px;}");
    //frame_text->setStyleSheet("border:1px solid rgb(0,255,0)");


    UserNameText->setStyleSheet("QLabel{background-color: white;border:0px solid black;border-radius:0px;}");
    UserName->setStyleSheet("QLabel{background-color: white;border:0px solid black;border-radius:0px;}");
    Battery->setStyleSheet("QLabel{background-color: white;border:0px solid black;border-radius:0px;}");
    BatteryText->setStyleSheet("QLabel{background-color: white;border:0px solid black;border-radius:0px;}");
    UploadText->setStyleSheet("QLabel{background-color: white;border:0px solid black;border-radius:0px;}");
    UploadProcessText->setStyleSheet("QLabel{background-color: white;border:0px solid black;border-radius:0px;}");
    UserNameText->setText(tr("Name:"));
    BatteryText->setText(tr("Charge:"));
    UploadProcessText->setText(tr("Upload:"));



    QHBoxLayout *hlayusername = new QHBoxLayout();
    hlayusername->addWidget(UserNameText);
    hlayusername->addWidget(UserName);
    hlayusername->setContentsMargins(5,0,0,0);


    QHBoxLayout *hlayupload = new QHBoxLayout();
    hlayupload->addWidget(UploadProcessText);
    hlayupload->addWidget(UploadText);
    hlayupload->setContentsMargins(5,0,0,0);

    QHBoxLayout *hlaybattery = new QHBoxLayout();
    hlaybattery->addWidget(BatteryText);
    hlaybattery->addWidget(Battery);
    hlaybattery->setContentsMargins(5,0,0,0);


    QVBoxLayout *vl_text = new QVBoxLayout();

    vl_text->addItem(hlayusername);
    vl_text->addItem(hlayupload);
    vl_text->addItem(hlaybattery);
    vl_text->setContentsMargins(5,0,5,0);
    frame_text->setLayout(vl_text);



    QHBoxLayout *hlayphoto = new QHBoxLayout();
    hlayphoto->addWidget(label_photo);
    hlayphoto->setContentsMargins(0,0,0,100);
    label_photo->setAlignment(Qt::AlignTop|Qt::AlignCenter);

    QVBoxLayout *vlay = new QVBoxLayout();
    vlay->setAlignment(Qt::AlignCenter);
    vlay->setSpacing(0);
    vlay->setContentsMargins(5,10,5,10);

    vlay->addItem(hlayphoto);
    vlay->addWidget(frame_text,Qt::AlignBottom |Qt::AlignCenter);
    frame_background->setLayout(vlay);

    QVBoxLayout *vlay2 = new QVBoxLayout();
    vlay2->setContentsMargins(0,0,0,0);
    vlay2->addWidget(frame_background);
    this->setLayout(vlay2);


}

void DASBuddy::setphoto(QString path)
{

    QPixmap *image = new QPixmap(path);
    QPixmap fitpixmap=image->scaled(label_photo->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    label_photo->setPixmap(fitpixmap);
}
void DASBuddy::setNetworkPic(const QString &szUrl)
{
    QUrl url(szUrl);
    QNetworkAccessManager manager;
    QEventLoop loop;

    // qDebug() << "Reading picture form " << url;
    QNetworkReply *reply = manager.get(QNetworkRequest(url));
    //请求结束并下载完成后，退出子事件循环
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    //开启子事件循环
    loop.exec();

    QByteArray jpegData = reply->readAll();
    QPixmap pixmap;
    pixmap.loadFromData(jpegData);
    //QPixmap fitpixmap=pixmap.scaled(label_photo->size());
    QPixmap fitpixmap=pixmap.scaled(label_photo->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    fitpixmap = round(fitpixmap,15);//
    label_photo->setPixmap(fitpixmap); // 你在QLabel显示图片


}

void DASBuddy::setPhotoText(QString text){

    label_photo->setText(text);
}


void DASBuddy::setBackgroundColor(QColor color){
    //QString("%1").arg(path[0], 0, 10)
    //"background-color: white"
    frame_background->setStyleSheet(QString("background-color: %1").arg(converRGB2HexStr(color)));
}

QString DASBuddy::converRGB2HexStr(QColor _rgb)

{
    QString redStr = QString("%1").arg(_rgb.red(),2,16,QChar('0'));

    QString greenStr = QString("%1").arg(_rgb.green(),2,16,QChar('0'));

    QString blueStr = QString("%1").arg(_rgb.blue(),2,16,QChar('0'));

    QString hexStr = "#"+redStr+greenStr+blueStr;

    return hexStr;

}

int DASBuddy::getMybattery() const
{
    return mybattery;
}

void DASBuddy::setMybattery(const int &value)
{
    mybattery = value;
    Battery->setText(QString("%1%2").arg(mybattery).arg("%"));
}



void DASBuddy::setIsConnect(volatile bool value)
{
    isConnect = value;
}

void DASBuddy::getAllSetting()
{
    communicate(handle,toGetMoviesize);
}

void DASBuddy::changewriting()
{
    writingnow = "@ismounting";
}

void DASBuddy::changewritingfail()
{
    qDebug()<<"mount fail!";
    writingnow = "@mountingfail";
}

volatile bool DASBuddy::getIsConnect() const
{
    return isConnect;
}

QString DASBuddy::getDriverid() const
{
    return driverid;
}

void DASBuddy::toOpen(libusb_device *dev,int number)
{
    qDebug()<<"Start opendevice!!!";
    libusb_open(dev,&handle);

    this->num = number;

    qDebug()<<"insert number"<<number;
}

void DASBuddy::toConnectZFY()
{
    communicate(handle,passwordtext);


}

void DASBuddy::tosetUserId(QString userid)
{
    communicate(handle,userid);
    this->userid = userid;
}

void DASBuddy::tosetText(QString text)
{
    communicate(handle,text);
    if(text.contains("@UserID#")){

        QRegExp getUesrIDText("#([A-Za-z0-9]{6})");
        getUesrIDText.indexIn(text);
        this->userid = getUesrIDText.cap(1);
        qDebug()<<"setuserid:"<<userid;
    }
    if(text.contains("@DriveID#")){

        QRegExp getDriveIDText("#([A-Za-z0-9]{7})");
        getDriveIDText.indexIn(text);
        this->driverid = getDriveIDText.cap(1);
        qDebug()<<"setdriveid:"<<driverid;

    }

}



QString DASBuddy::getUserid() const
{
    return userid;
}
void DASBuddy::setUserName(QString name){
    UserName->setText(name);
}
void DASBuddy::setUserUpload(QString upload){
    UploadText->setText(QString("%1").arg(upload));
}
void DASBuddy::setBatteryZero(){
    Battery->setText("");
}



QString DASBuddy::getUpload()
{
    return UploadText->text();
}
//void DASBuddy::initProcessBar(){
//    processbar = new QProgressBar();
//    processbar->setRange(0,100);
//    processbar->setOrientation(Qt::Horizontal);
//    processbar->setAlignment(Qt::AlignCenter);
//    processbar->setTextVisible(true);
//    processbar->setVisible(true);
//    processbar->setValue(0);
//}






static int get_mass_storage_status(libusb_device_handle *handle, uint8_t endpoint, uint32_t expected_tag)
{
    int i, r, size;
    struct command_status_wrapper csw;

    // The device is allowed to STALL this transfer. If it does, you have to
    // clear the stall and try again.
    i = 0;
    do {
        r = libusb_bulk_transfer(handle, endpoint, (unsigned char*)&csw, 13, &size, 1000);
        if (r == LIBUSB_ERROR_PIPE) {
            libusb_clear_halt(handle, endpoint);
        }
        i++;
    } while ((r == LIBUSB_ERROR_PIPE) && (i<RETRY_MAX));
    if (r != LIBUSB_SUCCESS) {
        qDebug()<<"   get_mass_storage_status:" << libusb_strerror((enum libusb_error)r);
        return -1;
    }
    if (size != 13) {
        qDebug()<<"   get_mass_storage_status: received "<<size<<" bytes (expected 13)";
        return -1;
    }
    if (csw.dCSWTag != expected_tag) {
        qDebug()<<"   get_mass_storage_status: mismatched tags (expected"<<expected_tag<<", received " << csw.dCSWTag<<")";
        return -1;
    }
    // For this test, we ignore the dCSWSignature check for validity...
    qDebug()<<"   Mass Storage Status: " << csw.bCSWStatus <<" "<< (csw.bCSWStatus?"FAILED":"Success\n");
    if (csw.dCSWTag != expected_tag)
        return -1;
    if (csw.bCSWStatus) {
        // REQUEST SENSE is appropriate only if bCSWStatus is 1, meaning that the
        // command failed somehow.  Larger values (2 in particular) mean that
        // the command couldn't be understood.
        if (csw.bCSWStatus == 1)
            return -2;	// request Get Sense
        else
            return -1;
    }

    // In theory we also should check dCSWDataResidue.  But lots of devices
    // set it wrongly.
    return 0;
}

int DASBuddy::transferBulkData(libusb_device_handle * usbHandle,command_block_wrapper * cbw,int data_length,unsigned char * buffer ) {
    //    QThread::msleep(500);
    int transferredLength = 0;

    int i = 0,r = 0;


    do {
        // The transfer length must always be exactly 31 bytes.
        r = libusb_bulk_transfer(usbHandle,ENDPOINT_OUT,(unsigned char *)cbw,31,&transferredLength,1000);
        if (r == LIBUSB_ERROR_PIPE) {
            libusb_clear_halt(usbHandle, ENDPOINT_OUT);
            qDebug()<<"LIBUSB_ERROR_PIPE ERROR, clear";
        }
        i++;
    } while ((r == LIBUSB_ERROR_PIPE) && (i<RETRY_MAX));

    if (r==0 && transferredLength == 31) {
        qDebug()<<"\ntransferBulkData:Write USBC successful!";
    }
    else {
        qDebug()<<("\ntransferBulkData:Write USBC error, code")<<r;
        return r;
    }


    if (buffer) {
        memset(buffer, 0, 64);

        CALL_CHECK(libusb_bulk_transfer(usbHandle, ENDPOINT_IN, (unsigned char *)buffer, data_length, &transferredLength, 1000));



        if (r==0) {
            qDebug()<<"transferBulkData:Received:"<<transferredLength<<" chars";
            qDebug()<<QLatin1String((const char *)buffer,transferredLength);
            //            ui->lineResponse->setText(QLatin1String((char *)strResponse,transferredLength));




        }
        else {
            qDebug()<<"transferBulkData:Read error"<<r;
            return r;
        }
    }


    return get_mass_storage_status(usbHandle,ENDPOINT_IN,cbw->dCBWTag);



}

void DASBuddy::setWritingnow(const QString &value)
{
    writingnow = value;
}

QString DASBuddy::getWritingnow() const
{
    return writingnow;
}



int DASBuddy::communicate(libusb_device_handle * devHandle,QString text) {



    int r;

    //    if (ENDPOINT_IN & LIBUSB_ENDPOINT_IN) {
    //        qDebug()<<"cannot send command on IN endpoint\n";
    ////        return;
    //    }

    //    if (ENDPOINT_OUT & LIBUSB_ENDPOINT_OUT) {
    //        qDebug()<<"cannot send command on OUT endpoint\n";
    ////        return;
    //    }



    r = libusb_set_auto_detach_kernel_driver(devHandle,1);
    if (r<0) {
        qDebug()<<"Detach kernel driver:"<<r;
        return r;
    }
    //    r = libusb_set_configuration(devHandle,1);
    //    if (r<0) {
    //        qDebug()<<"Set Configuration error:"<<r;
    //        return;
    //    }

    r = libusb_claim_interface(devHandle, 0);
    if (r<0) {
        qDebug()<<"Claim interface error:"<<r;
        return r;
    }
    uint8_t lun;
    //printf("Reading Max LUN:\n");
    qDebug()<<"Reading Max LUN:\n";
    r = libusb_control_transfer(devHandle, LIBUSB_ENDPOINT_IN|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE,
                                BOMS_GET_MAX_LUN, 0, 0, &lun, 1, 1000);
    // Some devices send a STALL instead of the actual value.
    // In such cases we should set lun to 0.
    if (r == 0) {
        lun = 0;
    } else if (r < 0) {
        //printf("   Failed: %s", libusb_strerror((enum libusb_error)r));
        qDebug()<<"   Failed:"<<libusb_strerror((enum libusb_error)r);
    }
    qDebug()<<"   Max LUN ="<<lun;

    //   uint8_t cdb[16] = {0xC5,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0xFF,0x02,0x00,0x00,0x00,0x00};	// SCSI Command Descriptor Block

    uint8_t cdbInquery[16] = {0x12,0x00,0x00,0x00,0x24,0x00};	// SCSI Command Descriptor Block

    struct command_block_wrapper cbw;

    memset(&cbw, 0, sizeof(cbw));
    cbw.dCBWSignature[0] = 'U';
    cbw.dCBWSignature[1] = 'S';
    cbw.dCBWSignature[2] = 'B';
    cbw.dCBWSignature[3] = 'C';
    cbw.dCBWTag = 1;
    cbw.dCBWDataTransferLength = 0x24;
    cbw.bmCBWFlags = LIBUSB_ENDPOINT_IN;
    cbw.bCBWLUN = lun;
    // Subclass is 1 or 6 => cdb_len
    cbw.bCBWCBLength = cdb_length[cdbInquery[0]];;
    memcpy(cbw.CBWCB, cdbInquery, 0x06);

    uint8_t buffer[64];
    this->transferBulkData(devHandle,&cbw,0x24,(unsigned char *)&buffer);

    char vid[9], pid[9], rev[5];

    int i;
    // The following strings are not zero terminated
    for (i=0; i<8; i++) {
        vid[i] = buffer[8+i];
        pid[i] = buffer[16+i];
        rev[i/2] = buffer[32+i/2];	// instead of another loop
    }
    vid[8] = 0;
    pid[8] = 0;
    rev[4] = 0;
    qDebug()<<"    VID:PID:REV "<<vid<<":"<<pid<<":"<<rev;




    int tag = 100;
    unsigned char cdb[16];	// SCSI Command Descriptor Block


    tag++;


    memset(cdb,0,sizeof(cdb));
    cdb[0] = 0xC5;
    cdb[1] = 0x07;
    //cdb[9] = ui->lineRequest->text().length();
    cdb[9] = text.length();
    cdb[10] = 0x02;
    cdb[11] = 0x03;
    cdb[12] = 0x01;
    memset(&cbw, 0, sizeof(cbw));
    cbw.dCBWSignature[0] = 'U';
    cbw.dCBWSignature[1] = 'S';
    cbw.dCBWSignature[2] = 'B';
    cbw.dCBWSignature[3] = 'C';
    cbw.dCBWTag = tag;
    //cbw.dCBWDataTransferLength = ui->lineRequest->text().length();
    cbw.dCBWDataTransferLength = text.length();
    cbw.bmCBWFlags = LIBUSB_ENDPOINT_OUT;
    cbw.bCBWLUN = lun;
    // Subclass is 1 or 6 => cdb_len
    cbw.bCBWCBLength = 0x10;
    memcpy(cbw.CBWCB, cdb, 0x10);


    int transferredLength = 0;


    i = 0;

    do {
        // The transfer length must always be exactly 31 bytes.
        r = libusb_bulk_transfer(devHandle,ENDPOINT_OUT,(unsigned char *)&cbw,31,&transferredLength,1000);
        //qDebug()<<"(unsigned char *)&cbw:"<<&cbw;
        //qDebug()<<"transferredLength:"<<transferredLength;
        if (r == LIBUSB_ERROR_PIPE) {
            libusb_clear_halt(devHandle, ENDPOINT_OUT);
        }
        i++;
    } while ((r == LIBUSB_ERROR_PIPE) && (i<RETRY_MAX));

    if (r==0 && transferredLength == 31) {
        qDebug()<<"Write USBC successful!";
    }
    else {
        qDebug()<<("Write USBC error, code")<<r;
        return r;
    }


    //QByteArray tempData = ui->lineRequest->text().toLocal8Bit();
    QByteArray tempData = text.toLocal8Bit();
    transferredLength = 0;
    int stringLength = tempData.length();

    i = 0;
    do {

        // The transfer length must always be exactly 31 bytes.
        uint8_t requestData[64];
        memset(requestData,0,sizeof(requestData));
        memcpy(&requestData,tempData.data(),stringLength);
        r = libusb_bulk_transfer(devHandle,ENDPOINT_OUT,(unsigned char *)&requestData,stringLength,&transferredLength,1000);
        qDebug()<<"stringLength:"<<stringLength;
        if (r == LIBUSB_ERROR_PIPE) {
            libusb_clear_halt(devHandle, ENDPOINT_OUT);
        }
        i++;
    } while ((r == LIBUSB_ERROR_PIPE) && (i<RETRY_MAX));



    if (r==0 && transferredLength == stringLength) {
        qDebug()<<"Write command successful! " <<tempData<<" "<<transferredLength<<"bytes";
        writingnow = tempData;


    }
    else {
        qDebug()<<("Write error, code")<<r;
        return r;
    }


    char strResponse[32];

    memset(strResponse,32,'\0');
    i = 0;
    do {
        r = libusb_bulk_transfer(devHandle,ENDPOINT_IN,(unsigned char *)strResponse,13,&transferredLength,1000);
        if (r == LIBUSB_ERROR_PIPE) {
            libusb_clear_halt(devHandle, ENDPOINT_IN);
        }
        i++;
    } while ((r == LIBUSB_ERROR_PIPE) && (i<RETRY_MAX));



    if (r==0) {
        qDebug()<<"Received22:"<<transferredLength<<" chars";
        qDebug()<<QLatin1String((const char *)strResponse,transferredLength);
        //            ui->lineResponse->setText(QLatin1String((char *)strResponse,transferredLength));

        if(tempData == "@Ver#800000000"){
            emit insertZFY(num);
        }
        if(tempData == "@Ner#800000000"){
            qDebug()<<"@Ner#800000000Thread"<<QThread::currentThreadId();


            if(!controlthread->isRunning()){
                controlthread->start();
            }




            qDebug()<<"wait to mount!!";
            //emit changetoUdisksuccess();
            isConnect = true;


            //sleep
            //QThread::msleep(2000);
            //start to mount


            emit startToMount(num);


            //mount_finish
            //writingnow = "1";

            emit sig_insertUserLog(num);
            //                    QThread::msleep(2000);
            //                    bool mountresult = true;

            //                    zfy->setZfynum(num);

            //                    if(mountresult){
            //                        //startcopy
            //                        //dirpath = "/home/itventi/copyfile/"+QDateTime::currentDateTime().toString("yyyy-MM-dd") + "/" + QString::number(user_Id) +"/";
            //                        QString dirpath1 = QString("%1").arg(Config().Get("path","copypath").toString());
            //                                //.arg(QDateTime::currentDateTime().toString("yyyy-MM-dd"));

            //                        QDateTime uploadDate = QDateTime::currentDateTime();
            //                        QString uploadDatestr = uploadDate.toString("yyyy-MM-dd");
            //                        QString dirpath = dirpath1 + "/"+uploadDatestr +"/"+userid +"/";


            //                        qDebug()<<"dirpath::"<<dirpath;


            //                        //zfy->getBattery();

            //                        QVariantMap varmap;
            //                        varmap["dirpath"] = dirpath;
            //                        varmap["uploadDate"] = uploadDate;
            //                        varmap["userid"] = userid;
            //                        varmap["driverid"] = driverid;
            //                        varmap["num"] =

            //                        emit startCopy(varmap);

            //                        emit sendConnectPortNumber(num,zfy->getBattery());
            //                    }



            return 0;
        }

        if(tempData == ""){

        }

    }
    else {
        qDebug()<<"Read error"<<r;
        return r;
    }



    for(int jj=0;jj<1;++jj){
        QThread::msleep(100);
        tag+=jj;


        //        unsigned char cdb123[16] = {0xC5,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1E,0x02,0x02,0x01,0x00,0x00,0x00};	// SCSI Command Descriptor Block
        //        unsigned char cdb[16] = {0xC5,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1E,0x02,0x03,0x01,0x00,0x00,0x00};	// SCSI Command Descriptor Block

        tag++;
        //        unsigned char cdb[16];	// SCSI Command Descriptor Block

        memset(cdb,0,sizeof(cdb));
        cdb[0] = 0xC5;
        cdb[1] = 0x07;
        cdb[9] = 0x10;
        cdb[10] = 0xFF;
        cdb[11] = 0x04;

        memset(&cbw, 0, sizeof(cbw));
        cbw.dCBWSignature[0] = 'U';
        cbw.dCBWSignature[1] = 'S';
        cbw.dCBWSignature[2] = 'B';
        cbw.dCBWSignature[3] = 'C';
        cbw.dCBWTag = tag;
        cbw.dCBWDataTransferLength = 0x10;
        cbw.bmCBWFlags = LIBUSB_ENDPOINT_IN;
        cbw.bCBWLUN = lun;
        // Subclass is 1 or 6 => cdb_len
        cbw.bCBWCBLength = 0x10;
        memcpy(cbw.CBWCB, cdb, 0x10);
        transferBulkData(devHandle,&cbw,0x10,(unsigned char *)&buffer); //04 first

        tag++;
        cbw.dCBWTag = tag;
        transferBulkData(devHandle,&cbw,0x10,(unsigned char *)&buffer); //04 second

        tag++;
        cbw.dCBWTag = tag;
        cdb[11] = 0x06;
        memcpy(cbw.CBWCB, cdb, 0x10);
        transferBulkData(devHandle,&cbw,0x10,(unsigned char *)&buffer); //06


        tag++;
        cbw.dCBWTag = tag;
        cdb[11] = 0x04;
        memcpy(cbw.CBWCB, cdb, 0x10);
        transferBulkData(devHandle,&cbw,0x10,(unsigned char *)&buffer); //04


        tag++;
        cbw.dCBWTag = tag;
        cdb[11] = 0x03;
        memcpy(cbw.CBWCB, cdb, 0x10);
        transferBulkData(devHandle,&cbw,0x10,(unsigned char *)&buffer); //03 first

        tag++;
        cbw.dCBWTag = tag;
        transferBulkData(devHandle,&cbw,0x10,(unsigned char *)&buffer); //03 second


        tag++;
        cbw.dCBWTag = tag;
        cdb[11] = 0x05;
        memcpy(cbw.CBWCB, cdb, 0x10);
        transferBulkData(devHandle,&cbw,0x10,(unsigned char *)&buffer); //05

        tag++;


        cbw.dCBWTag = tag;
        memset(cdb,0,sizeof(cdb));
        cdb[0] = 0xC5;
        cdb[1] = 0x07;
        cdb[9] = 0x1E;
        cdb[10] = 0x02;
        cdb[11] = 0x01;
        cdb[12] = 0x01;
        cbw.dCBWDataTransferLength = 0x1E;
        memcpy(cbw.CBWCB, cdb, 0x10);
        transferBulkData(devHandle,&cbw,0x1E,(unsigned char *)&buffer); //1E

        tag++;
        cbw.dCBWTag = tag;
        memset(cdb,0,sizeof(cdb));
        cdb[0] = 0xC5;
        cdb[1] = 0x07;
        cdb[9] = 0x10;
        cdb[10] = 0xFF;
        cdb[11] = 0x04;
        cbw.dCBWDataTransferLength = 0x10;
        memcpy(cbw.CBWCB, cdb, 0x10);
        transferBulkData(devHandle,&cbw,0x10,(unsigned char *)&buffer); //04


        tag++;
        cbw.dCBWTag = tag;
        cdb[11] = 0x04;
        memcpy(cbw.CBWCB, cdb, 0x10);
        transferBulkData(devHandle,&cbw,0x10,(unsigned char *)&buffer); //04

        tag++;
        cbw.dCBWTag = tag;
        memset(cdb,0,sizeof(cdb));
        cdb[0] = 0xC5;
        cdb[1] = 0x07;
        cdb[9] = 0x1E;
        cdb[10] = 0x02;
        cdb[11] = 0x02;
        cdb[12] = 0x01;
        cbw.dCBWDataTransferLength = 0x1E;
        memcpy(cbw.CBWCB, cdb, 0x10);
        transferBulkData(devHandle,&cbw,0x1E,(unsigned char *)&buffer); //1E


        //ui->lineResponse->setText(QByteArray((const char *)&buffer,12));
        QThread::msleep(100);
        //emit writeoneSuccess(QString((const char *)&buffer));





        if(QString((const char *)&buffer).contains("@Ver:OKONCE")){
            qDebug()<<"123"<<QString((const char *)&buffer);


            communicate(devHandle,toGetBattery);

        }

        if(QString((const char *)&buffer).contains("@BATTERY#")){
            qDebug()<<"Battery"<<QString((const char *)&buffer);

            QString batterytext = QString((const char *)&buffer);
            QRegExp getUesrIDText("#(\\d{1,3})");
            getUesrIDText.indexIn(batterytext);
            this->mybattery = getUesrIDText.cap(1).toInt();
            qDebug()<<"mybattery:"<<mybattery;

            communicate(devHandle,toGetUserID);

        }

        if(QString((const char *)&buffer).contains("@UserID#")){
            qDebug()<<"UserID"<<QString((const char *)&buffer);
            QString useridtext = QString((const char *)&buffer);
            QRegExp getUesrIDText("#([A-Za-z0-9]{6})");
            getUesrIDText.indexIn(useridtext);
            this->userid = getUesrIDText.cap(1);
            qDebug()<<"userid:"<<userid;
            QThread::msleep(100);
            communicate(devHandle,toGetDriveID);
        }

        if(QString((const char *)&buffer).contains("@DriveID#")){
            qDebug()<<"DriveID"<<QString((const char *)&buffer);
            QString driveridtext = QString((const char *)&buffer);
            QRegExp getDriveIDText("#([A-Za-z0-9]{7})");
            getDriveIDText.indexIn(driveridtext);
            this->driverid = getDriveIDText.cap(1);
            qDebug()<<"driverid:"<<driverid;
            communicate(devHandle,toUdisktext);
        }



        if(QString((const char *)&buffer).contains("@Moviesize#")){
            qDebug()<<"Moviesize"<<QString((const char *)&buffer);
            this->moviesize = QString((const char *)&buffer);
            communicate(devHandle,toGetPhotosize);
        }


        if(QString((const char *)&buffer).contains("@Photosize#")){
            qDebug()<<"Photosize"<<QString((const char *)&buffer);
            this->photosize = QString((const char *)&buffer);
            communicate(devHandle,toGetMoviecycTime);
        }

        if(QString((const char *)&buffer).contains("@MoviecycTime#")){
            qDebug()<<"MoviecycTime"<<QString((const char *)&buffer);
            this->moviecycTime = QString((const char *)&buffer);
            communicate(devHandle,toGetMoviecyc);
        }


        if(QString((const char *)&buffer).contains("@Moviecyc#")){
            qDebug()<<"Moviecyc"<<QString((const char *)&buffer);
            this->moviecyc = QString((const char *)&buffer);
            communicate(devHandle,toGetDateWater);
        }

        if(QString((const char *)&buffer).contains("@DateWater#")){
            qDebug()<<"DateWater"<<QString((const char *)&buffer);
            this->dateWater = QString((const char *)&buffer);
            communicate(devHandle,toGetGsensor);
        }

        if(QString((const char *)&buffer).contains("@Gsensor#")){
            qDebug()<<"Gsensor"<<QString((const char *)&buffer);
            this->gsensor = QString((const char *)&buffer);
            communicate(devHandle,toGetMoviecodec);
        }

        if(QString((const char *)&buffer).contains("@Moviecodec#")){
            qDebug()<<"Moviecodec"<<QString((const char *)&buffer);
            this->moviecodec = QString((const char *)&buffer);
            communicate(devHandle,toGetPrerec);
        }

        if(QString((const char *)&buffer).contains("@Prerec#")){
            qDebug()<<"Prerec"<<QString((const char *)&buffer);
            this->prerec = QString((const char *)&buffer);
            communicate(devHandle,toGetDelayrec);
        }

        if(QString((const char *)&buffer).contains("@Delayrec#")){
            qDebug()<<"Delayrec"<<QString((const char *)&buffer);
            this->delayrec = QString((const char *)&buffer);
            communicate(devHandle,toGetGPS);
        }

        if(QString((const char *)&buffer).contains("@GPS#")){
            qDebug()<<"GPS"<<QString((const char *)&buffer);
            this->gPS = QString((const char *)&buffer);
            communicate(devHandle,toGetMotiondet);
        }

        if(QString((const char *)&buffer).contains("@Motiondet#")){
            qDebug()<<"Motiondet"<<QString((const char *)&buffer);
            this->motiondet = QString((const char *)&buffer);
            communicate(devHandle,toGetIrcut);
        }

        if(QString((const char *)&buffer).contains("@Ircut#")){
            qDebug()<<"Ircut"<<QString((const char *)&buffer);
            this->ircut = QString((const char *)&buffer);

            communicate(devHandle,toGetRecmode);

        }
        if(QString((const char *)&buffer).contains("@Recmode#")){
            qDebug()<<"Recmode"<<QString((const char *)&buffer);
            this->recmode = QString((const char *)&buffer);


            QVariantMap varmap;

            varmap["userid"] = userid;
            varmap["driverid"] = driverid;
            varmap["moviesize"] = moviesize;
            varmap["photosize"] = photosize;
            varmap["moviecycTime"] = moviecycTime;
            varmap["moviecyc"] = moviecyc;
            varmap["dateWater"] = dateWater;
            varmap["gsensor"] = gsensor;
            varmap["moviecodec"] = moviecodec;
            varmap["prerec"] = prerec;
            varmap["delayrec"] = delayrec;
            varmap["gPS"] = gPS;
            varmap["motiondet"] = motiondet;
            varmap["ircut"] = ircut;
            varmap["recmode"] = recmode;


            emit getAllSettingStr(varmap);
        }
    }

    return 0;



}


void DASBuddy::closeThread(){


    if(controlthread !=nullptr){
        if(controlthread->isRunning())
        {
            qDebug()<<tr("关闭线程");
            //zfy1->closeThread();  //关闭线程槽函数
            controlthread->quit();            //退出事件循环
            controlthread->wait();            //释放线程槽函数资源
            //controlthread->terminate();
            //controlthread->deleteLater();
            //controlthread->deleteLater();
        }
    }

}

void DASBuddy::finishedThreadSlot(){

    qDebug()<<tr("多线程触发了finished信号");
}
void DASBuddy::checkmount(int num){
    QThread::msleep(2000);
    QString mountpath = QString("%1%2%3").arg(Config().Get("path","mountpath").toString()).arg("zfy").arg(num+1);
    QString batterypath = mountpath + "/LOG/BATTERY.TXT";
    //qDebug()<<"pathtest"<<pathtest;
    bool mountresult = isFileExist(batterypath);
    qDebug()<<"mountresult:"<<mountresult;
    if(mountresult){

        emit mountsuccess(num);
    }
}
void DASBuddy::waittoCopy(int num)
{


    bool mountresult = false;
    //    QThread::msleep(2000);
    //    QString pathee = QString("%1%2%3").arg(Config().Get("path","mountpath").toString()).arg("zfy").arg(num+1);
    //    QString pathtest = pathee + "/LOG/BATTERY.TXT";
    //    //qDebug()<<"pathtest"<<pathtest;
    //    bool testresult = isFileExist(pathtest);
    //    qDebug()<<"testresult"<<testresult;
    //    if(writingnow == "@ismounting"){
    //        writingnow = "1";
    //        mountresult = true;
    //    }else if(writingnow == "@mountingfail"){
    //        mountresult = false;
    //    }

    //    qDebug()<<"mountresult:"<<mountresult<<"writingnow"<<writingnow;

    if(this->isConnect){
        qDebug()<<"writingnow!!!!!!"<<writingnow;
        writingnow = "1";
        mountresult = true;

    }else {
        mountresult = false;
    }
    zfy->setZfynum(num);

    if(mountresult){
        //startcopy
        //dirpath = "/home/itventi/copyfile/"+QDateTime::currentDateTime().toString("yyyy-MM-dd") + "/" + QString::number(user_Id) +"/";
//        QString dirpath1 = QString("%1").arg(Config().Get("path","copypath").toString());
//        //.arg(QDateTime::currentDateTime().toString("yyyy-MM-dd"));

//        uploadDate = QDateTime::currentDateTime();
//        QString uploadDatestr = uploadDate.toString("yyyy-MM-dd");
//        dirpath = dirpath1 + "/"+uploadDatestr +"/"+userid +"/";


//        qDebug()<<"dirpath::"<<dirpath;


        //zfy->getBattery();

        QVariantMap varmap;
        varmap["dirpath"] = dirpath;
        varmap["uploadDate"] = uploadDate;
        varmap["userid"] = userid;
        varmap["driverid"] = driverid;
        varmap["num"] = num;
        //emit startCopy(varmap);


        initTimer_battery();
        emit sendConnectPortNumber(num,this->getMybattery());



        //        QString mkmutidir = mkMutiDir(dirpath);
        //        qDebug()<<"mkmutidir"<<mkmutidir;

        //        QString path = QString("%1%2%3").arg(Config().Get("path","mountpath").toString()).arg("zfy").arg(num+1);
        //        qDebug()<<"path"<<path;
        //        QFileInfoList file_all;
        //        qint64 sizeall;
        //        getAllFile(path,file_all,file_gps,sizeall);
        //        qDebug()<<"file_all.count():"<<file_all.count();
        //        qDebug()<<"file_gps.count():"<<file_gps.count();
        //        qDebug()<<"sizeall:"<<sizeall;



        //        gpsMap.clear();

        //        if(!file_gps.isEmpty()){
        //            for(int i = 0; i < file_gps.count(); i++)
        //            {
        //                //if file suffix is NMEA ,add to list.
        //                if(file_gps.at(i).suffix() == "NMEA"){
        //                    QString result;
        //                    qDebug()<<"coor file path:"<<file_gps.at(i).path() +"/"+file_gps.at(i).fileName();
        //                    QFile file(file_gps.at(i).path() +"/"+ file_gps.at(i).fileName());
        //                    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        //                    {
        //                        while (!file.atEnd())
        //                        {
        //                            QByteArray line = file.readLine();

        //                            QString coor = line;
        //                            //qDebug()<<"coor:"<<coor;
        //                            QStringList source = coor.split(",");

        //                            QString jd = QString::number(source.at(3).mid(0,2).toInt() + source.at(3).mid(2).toDouble()/60,'f',6);
        //                            //qDebug()<<"纬度:"<<jd;
        //                            result += jd +",";
        //                            QString wd = QString::number(source.at(5).mid(0,3).toInt() + source.at(5).mid(3).toDouble()/60,'f',6);
        //                            //qDebug()<<"经度:"<<wd;
        //                            result += wd + ";";
        //                            //qDebug()<<"result:"<<result;
        //                        }
        //                        file.close();


        //                    }
        //                    qDebug()<<"resultgps:"<<result;
        //                    gpsMap[file_gps.at(i).completeBaseName()] = result;
        //                }
        //            }

        //        }


        //        emit checkfileiscopy(file_all,this->driverid,num);


        //        //myDataVm->copyFiles(file_all, dirpath);
    }
}


void DASBuddy::starttocheckcopy(int num){


    QString dirpath1 = QString("%1").arg(Config().Get("path","copypath").toString());
    //.arg(QDateTime::currentDateTime().toString("yyyy-MM-dd"));
    virtualpath = QString("%1").arg(Config().Get("path","virtualpath").toString());
    uploadDate = QDateTime::currentDateTime();
    QString uploadDatestr = uploadDate.toString("yyyy-MM-dd");
    dirpath = dirpath1 + virtualpath + "/"+uploadDatestr +"/"+userid +"/";


    qDebug()<<"dirpath::"<<dirpath;


    QString mkmutidir = mkMutiDir(dirpath);
    qDebug()<<"mkmutidir"<<mkmutidir;

    QString path = QString("%1%2%3").arg(Config().Get("path","mountpath").toString()).arg("zfy").arg(num+1);
    qDebug()<<"path"<<path;
    QFileInfoList file_all;
    qint64 sizeall;
    getAllFile(path,file_all,file_gps,sizeall);
    qDebug()<<"file_all.count():"<<file_all.count();
    qDebug()<<"file_gps.count():"<<file_gps.count();
    qDebug()<<"sizeall:"<<sizeall;



    gpsMap.clear();

    if(!file_gps.isEmpty()){
        for(int i = 0; i < file_gps.count(); i++)
        {
            //if file suffix is NMEA ,add to list.
            if(file_gps.at(i).suffix() == "NMEA"){
                QString result;
                qDebug()<<"coor file path:"<<file_gps.at(i).path() +"/"+file_gps.at(i).fileName();
                QFile file(file_gps.at(i).path() +"/"+ file_gps.at(i).fileName());
                if (file.open(QIODevice::ReadOnly | QIODevice::Text))
                {
                    while (!file.atEnd())
                    {
                        QByteArray line = file.readLine();

                        QString coor = line;
                        //qDebug()<<"coor:"<<coor;
                        QStringList source = coor.split(",");

                        QString jd = QString::number(source.at(3).mid(0,2).toInt() + source.at(3).mid(2).toDouble()/60,'f',6);
                        //qDebug()<<"纬度:"<<jd;
                        result += jd +",";
                        QString wd = QString::number(source.at(5).mid(0,3).toInt() + source.at(5).mid(3).toDouble()/60,'f',6);
                        //qDebug()<<"经度:"<<wd;
                        result += wd + ";";
                        //qDebug()<<"result:"<<result;
                    }
                    file.close();


                }
                qDebug()<<"resultgps:"<<result;
                gpsMap[file_gps.at(i).completeBaseName()] = result;
            }
        }

    }

    emit checkfileiscopy(file_all,this->driverid,num);


}
void DASBuddy::startcopynoexits(QFileInfoList copylist){

    qDebug()<<"startcopynoexits dir:"<<dirpath;
    myDataVm->copyFiles(copylist, dirpath);
}

QString DASBuddy::mkMutiDir(const QString path){
    QDir dir(path);
    if ( dir.exists(path)){
        return path;
    }
    QString parentDir = mkMutiDir(path.mid(0,path.lastIndexOf('/')));
    QString dirname = path.mid(path.lastIndexOf('/') + 1);
    QDir parentPath(parentDir);
    if ( !dirname.isEmpty() )
        parentPath.mkpath(dirname);
    return parentDir + "/" + dirname;
}

void DASBuddy::getAllFile(QString path,QFileInfoList &file_all,QFileInfoList &file_gps,qint64 &sizeall){
    qDebug()<<"getAllFile!!!!";

    file_all.clear();
    file_gps.clear();
    sizeall = 0;
    QDir dirLog(path + "/LOG");
    QDir dirMovie(path + "/DCIM/Movie");
    QDir dirPhoto(path + "/DCIM/Photo");

    QFileInfoList file_log = dirLog.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    QFileInfoList file_photo = dirPhoto.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    QFileInfoList file_movie = dirMovie.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);

    QFileInfoList fileall;

    fileall.append(file_log);
    fileall.append(file_photo);
    fileall.append(file_movie);

    qDebug()<<"fileall:"<<fileall.count();
    if(!fileall.isEmpty()){
        for (int i=0;i<fileall.count();i++) {
            if(fileall.at(i).fileName() != "BATTERY.TXT" && fileall.at(i).suffix()!="NMEA"){
                sizeall += fileall.at(i).size();
                file_all.append(fileall.at(i).filePath());
            }else if(fileall.at(i).suffix()=="NMEA"){
                file_gps.append(fileall.at(i).filePath());
            }
        }
    }

    qDebug()<<"get fileall finished!";
    //return  file_all;
}


void DASBuddy::updateProgress(QVariant dataVar, qreal progress, int zfynum)
{
    emit sendProgress(dataVar,progress,zfynum);
}
void DASBuddy::postTogetNameAndimage(){
    if(this->isConnect){

        qDebug()<<"postTogetNameAndimage Thread:"<<QThread::currentThreadId();
        naManager = new QNetworkAccessManager(this);
        connect(naManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));

        QString postData = QString("%1%2")
                .arg("polno=").arg(userid);
        qDebug()<<"userid:"<<userid;
        QString url = Config().Get("server","serverIp").toString() +"/api/Login/AjaxClientGetPolImage";
        qDebug()<<"postTogetNameAndimageurl:"<<url;
        request.setUrl(QUrl(url));
        request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/x-www-form-urlencoded"));
        naManager->post(request, postData.toLocal8Bit());

    }


}
void DASBuddy::requestFinished(QNetworkReply* reply) {
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
                //success
                QJsonObject result_data = root_Obj.value("data").toObject();
                QString username = result_data.value("username").toString();
                QString imagepath = result_data.value("imagepath").toString();

                qDebug()<<"username:"<<username<<"imagepath:"<<imagepath;

                this->setUserName(username);
                this->setNetworkPic(imagepath);

            }





        }

    }
}
void DASBuddy::initTimer_battery(){
    if(timer_battery == nullptr){
        qDebug()<<"init timer_battery";
        timer_battery = new QTimer(this);
        connect(timer_battery, SIGNAL(timeout()), this, SLOT(updateBattery()));


    }

    timer_battery->start(60000);
    qDebug()<<"timer_battery start";
}


void DASBuddy::initTimer_shine(){
    if(timer_shine == nullptr){
        qDebug()<<"init timer_shine";
        timer_shine = new QTimer(this);
        connect(timer_shine, SIGNAL(timeout()), this, SLOT(backgroundshine()));
    }

    timer_shine->start(500);
}
void DASBuddy::updateBattery(){
    qDebug()<<"update battery";
    if(mybattery <100){
        this->setMybattery(mybattery+1);
    }if(mybattery >=100){
        this->setBackgroundColor(QColor(112,173,71));
        this->setMybattery(100);
        timer_battery->stop();
    }
}
void DASBuddy::backgroundshine(){
    if(frame_background->property("shine").toBool()){

        setBackgroundColor(QColor(112,173,71));
        frame_background->setProperty("shine",false);
    }else {
        setBackgroundColor(QColor("white"));
        frame_background->setProperty("shine",true);
    }
}

void DASBuddy::slotProgressBar(float value)
{
    //qDebug()<<"copy value:"<<value;
    this->setUserUpload(QString::number(value*100,'f',1)+"%");
}

void DASBuddy::slotCopyFinished(bool result)
{
    if(result){
        qDebug()<<"copy finished!";
        this->setUserUpload(QString::number(100,'f',1)+"%");
        if(deletefile){
            for (int i=0;i<file_gps.count();i++) {
                QFile file_delete(file_gps.at(i).filePath());
                file_delete.remove();
                file_delete.close();
            }
        }
    }else {

        qDebug()<<"copy result!"<<result;
        this->setUserUpload("");
    }

    emit zfycopyfinished();

}

void DASBuddy::slotCopyonesuccess(QString srcfilepath,QString uuidstr)
{

    qDebug()<<"copy file name:"<<srcfilepath<<"uuidstr"<<uuidstr;
    QFileInfo fileinfo(srcfilepath);

    QVariantMap copyfilemap;
    copyfilemap["file_uuid"] = uuidstr;
    copyfilemap["file_name"] = fileinfo.fileName();
    copyfilemap["file_creattime"] = fileinfo.created();
    copyfilemap["file_size"] = fileinfo.size();
    copyfilemap["file_est"] = fileinfo.completeSuffix();
    copyfilemap["file_path"] = dirpath;
    copyfilemap["file_uploadtime"] = uploadDate;
    copyfilemap["user_id"] = userid;
    copyfilemap["driver_id"] = driverid;
    copyfilemap["virtualpath"] = virtualpath;

    if(fileinfo.suffix() =="JPG"){
        copyfilemap["file_type"] = 1;
        copyfilemap["file_gps"] = QString("");
    }else if(fileinfo.suffix() =="MP4"){
        copyfilemap["file_type"] = 2;
        copyfilemap["file_gps"] = gpsMap.value(fileinfo.completeBaseName(),QString("")).toString();

    }else if(fileinfo.suffix() =="TXT"){
        copyfilemap["file_type"] = 4;
        copyfilemap["file_gps"] = QString("");
    }else if(fileinfo.suffix() =="m4a"){
        copyfilemap["file_type"] = 3;
        copyfilemap["file_gps"] = QString("");
    }



    if(deletefile){
        QFile file_delete(srcfilepath);
        file_delete.remove();
        file_delete.close();
    }





    emit sigtowirtesql(copyfilemap);


}
void DASBuddy::stopShine(){
    if(timer_shine !=NULL){
        if(timer_shine->isActive()){
            timer_shine->stop();
        }
    }

}
void DASBuddy::stopTimer(){
    if(timer_battery !=NULL){
        if(timer_battery->isActive()){
            timer_battery->stop();
        }
    }

}
QPixmap DASBuddy::round(const QPixmap& img_in, int radius)
{
    if (img_in.isNull())
    {
        return QPixmap();
    }
    QSize size(img_in.size());
    QBitmap mask(size);
    QPainter painter(&mask);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.fillRect(mask.rect(), Qt::white);
    painter.setBrush(QColor(0, 0, 0));
    painter.drawRoundedRect(mask.rect(), radius, radius);
    QPixmap image = img_in;// .scaled(size);
    image.setMask(mask);
    return image;
}
void DASBuddy::clearuploadsize(){
    qDebug()<<"clearzfy!!!!!";

    //    if(zfy !=nullptr){
    //        zfy->clearSize();
    //
    //    }

    myDataVm->interruptCopyFile();

    closeThread();
    //QThread::msleep(3000);
    this->setUserUpload("");
}

bool DASBuddy::isFileExist(QString filepath){
    QFileInfo file(filepath);
    qDebug()<<"isFileExist file:"<<filepath;
    if(file.exists()){
        return true;
    }else {
        return false;
    }
}
