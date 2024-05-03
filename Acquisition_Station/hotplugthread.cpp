#include "hotplugthread.h"

QStringList usbhublist;
int indexNum = -1;
int deleteNum = -1;
DASBuddy *dasbuddy[30];

bool isInsert = false;
static int LIBUSB_CALL hotplug_callback(libusb_context *ctx, libusb_device *dev, libusb_hotplug_event event, void *user_data)
{
    struct libusb_device_descriptor desc;
    int rc;

    (void)ctx;
    (void)dev;
    (void)event;
    (void)user_data;

    rc = libusb_get_device_descriptor(dev, &desc);
    if (LIBUSB_SUCCESS != rc) {
        //fprintf (stderr, "Error getting device descriptor\n");
        qDebug()<<stderr<<"Error getting device descriptor\n";
    }

    uint8_t path[8];
    int ret = libusb_get_port_numbers(dev, path, sizeof(path));
    QString port_num;
    if (ret > 0) {

        port_num.append(QString("%1").arg(path[0], 0, 10));
        for (int j = 1; j < ret; ++j){

            port_num.append(".").append(QString("%1").arg(path[j], 0, 10));

        }
    }


    qDebug()<< "Device attached:"<< desc.idVendor<<";" <<desc.idProduct<<"port"<<port_num;

    isInsert = true;
    QThread::msleep(1500);

    //找到对应窗口

    indexNum = usbhublist.indexOf(port_num);
    //rc = libusb_open (dev, &handle);

    //    if(indexNum !=-1 && !dasbuddy[indexNum]->getIsConnect()){
    //        dasbuddy[indexNum]->toOpen(dev,indexNum);
    //    }
    if(indexNum != -1 ){
        dasbuddy[indexNum]->toOpen(dev,indexNum);

    }





    if (LIBUSB_SUCCESS != rc) {
        qDebug()<< "Error opening device:"<<rc;

    }


    return 0;
}

static int LIBUSB_CALL hotplug_callback_detach(libusb_context *ctx, libusb_device *dev, libusb_hotplug_event event, void *user_data)
{
    (void)ctx;
    (void)dev;
    (void)event;
    (void)user_data;


    uint8_t path[8];

    int ret = libusb_get_port_numbers(dev, path, sizeof(path));
    QString port_num;
    if (ret > 0) {
        //fprintf(stdout, "path: %d", path[0]);
        port_num.append(QString("%1").arg(path[0], 0, 10));
        for (int j = 1; j < ret; ++j){

            //fprintf(stdout, ".%d", path[j]);
            port_num.append(".").append(QString("%1").arg(path[j], 0, 10));

        }
    }

    isInsert = false;
    qDebug()<<"Device detached"<<"port"<<port_num;

    //QThread::msleep(1000);

    deleteNum = usbhublist.indexOf(port_num);


    if(deleteNum !=-1){
        //emit deleteZFY(deleteNum);
    }


    //    if (handle) {
    //        libusb_close (handle);
    //        handle = NULL;
    //    }


    return 0;
}

HotPlugThread::HotPlugThread(DASBuddy **buddy)
{
    //buddy[10]->setUserName("AAA");
    for (int i=0;i<30;i++) {
        dasbuddy[i] = buddy[i];
    }

    //ads = new ConnectADS();
    //
    getconfig = new Config();
    int port_num = getconfig->Get("portInfo","port_num").toInt();
    qDebug() << port_num;
    for (int i=0;i<port_num;i++) {
        QString port_no = "port" + QString::number(i);
        usbhublist.append(getconfig->Get("portInfo",port_no).toString());
        qDebug()<<"port"<<i<<":"<<usbhublist.at(i);
    }


}
void HotPlugThread::reloadConfig(){
    usbhublist.clear();
    //getconfig = new Config();
    int port_num = getconfig->Get("portInfo","port_num").toInt();
    qDebug() << port_num;

    for (int i=0;i<port_num;i++) {
        QString port_no = "port" + QString::number(i);
        usbhublist.append(getconfig->Get("portInfo",port_no).toString());
        qDebug()<<"port"<<i<<":"<<usbhublist.at(i);
    }

}

void HotPlugThread::clearForm(int num){

    dasbuddy[num]->setUserName("");
    dasbuddy[num]->setBackgroundColor(QColor("white"));
    dasbuddy[num]->setUserUpload("");

    dasbuddy[num]->setBatteryZero();
    dasbuddy[num]->setphoto("");
    dasbuddy[num]->setIsConnect(false);

}

void HotPlugThread::run()
{




    libusb_hotplug_callback_handle hp[2];
    int product_id, vendor_id, class_id;


    //    vendor_id  = (argc > 1) ? (int)strtol (argv[1], NULL, 0) : 0x045a;
    //    product_id = (argc > 2) ? (int)strtol (argv[2], NULL, 0) : 0x5005;
    //    class_id   = (argc > 3) ? (int)strtol (argv[3], NULL, 0) : LIBUSB_HOTPLUG_MATCH_ANY;

    vendor_id  =  0x0603;
    product_id =  0x8611;
    class_id   =  LIBUSB_HOTPLUG_MATCH_ANY;

    rc = libusb_init (NULL);
    if (rc < 0)
    {
        //printf("failed to initialise libusb: %s\n", libusb_error_name(rc));
        qDebug()<<"failed to initialise libusb:"<<libusb_error_name(rc);
        //return EXIT_FAILURE;
    }

    if (!libusb_has_capability (LIBUSB_CAP_HAS_HOTPLUG)) {
        //printf ("Hotplug capabilites are not supported on this platform\n");
        qDebug()<<"Hotplug capabilites are not supported on this platform";
        libusb_exit (NULL);
        //return EXIT_FAILURE;
    }

    rc = libusb_hotplug_register_callback (NULL, LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED, LIBUSB_HOTPLUG_NO_FLAGS, vendor_id,
                                           product_id, class_id, hotplug_callback, NULL, &hp[0]);
    if (LIBUSB_SUCCESS != rc) {
        //fprintf (stderr, "Error registering callback 0\n");
        qDebug()<<stderr<<"Error registering callback 0";
        libusb_exit (NULL);
        //return EXIT_FAILURE;
    }

    rc = libusb_hotplug_register_callback (NULL, LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT, LIBUSB_HOTPLUG_NO_FLAGS, vendor_id,
                                           product_id,class_id, hotplug_callback_detach, NULL, &hp[1]);
    if (LIBUSB_SUCCESS != rc) {
        //fprintf (stderr, "Error registering callback 1\n");
        qDebug()<<stderr<<"Error registering callback 1";
        libusb_exit (NULL);
        //return EXIT_FAILURE;
    }
    while (true) {



        rc = libusb_handle_events (NULL);
        if (rc < 0) {

            qDebug()<<"libusb_handle_events() failed:"<<libusb_error_name(rc);
        } else {

            //qDebug()<<"libusb_handle_events1111:"<<isInsert;
            //判断插入拔出，处理事件
            //if (handle) {
            //insert device

            if(!isInsert &&deleteNum !=-1){
                qDebug()<<"isInsert:"<<isInsert;
                //QThread::msleep(10000);
                qDebug()<<"deleteNum:"<<deleteNum;
                if(dasbuddy[deleteNum]->getWritingnow() == "@Ner#800000000"){

                    qDebug()<<"dasbuddy[deleteNum]->getWritingnow()"<<dasbuddy[deleteNum]->getWritingnow();
                    deleteNum = -1;
                }else if(dasbuddy[deleteNum]->getWritingnow() == "@ismounting"){

                    qDebug()<<"dasbuddy[deleteNum]->getWritingnow()"<<dasbuddy[deleteNum]->getWritingnow();
                    emit deleteZFY(deleteNum);
                    deleteNum = -1;
                }
                else {
                    qDebug()<<"dasbuddy[deleteNum]->getWritingnow()"<<dasbuddy[deleteNum]->getWritingnow();
                    emit deleteZFY(deleteNum);
                    deleteNum = -1;
                }


                //                qDebug()<<"change device status!";

                //                qDebug()<<"delete form!";
                //                qDebug()<<"delete Number:"<<deleteNum;
                //                dasbuddy[deleteNum]->setUserName("");
                //                dasbuddy[deleteNum]->setBackgroundColor(QColor("white"));
                //                dasbuddy[deleteNum]->setUserUpload("");

                //                dasbuddy[deleteNum]->setBattery("");
                //                dasbuddy[deleteNum]->setphoto("");
                //                dasbuddy[deleteNum]->setIsConnect(false);


            }

            if (isInsert ) {
                if(indexNum ==-1){
                    //config error
                    isInsert = false;
                    qDebug()<<"port number config error!";
                }else {
                    if(!dasbuddy[indexNum]->getIsConnect()){
                        qDebug()<<"isInsert:"<<isInsert;
                        qDebug()<<"indexNum:"<<indexNum;
                        //ads->toConnectZFYwithDevice(handle);
                        emit insertZFY(indexNum);
                        QThread::msleep(100);
                        dasbuddy[indexNum]->toConnectZFY();

                        indexNum = -1;
                    }
                }


            }

        }

        //        if(zfyreconnectnum != -1){

        //            qDebug()<<"to reconnect zfy num:"<<zfyreconnectnum;
        //            dasbuddy[zfyreconnectnum]->toConnectZFY();
        //            zfyreconnectnum = -1;
        //        }

    }
}

void HotPlugThread::toreconnect(int zfynum)
{

    zfyreconnectnum = zfynum;
    qDebug()<<"to reconnect zfy num12344:"<<zfyreconnectnum;
}


void HotPlugThread::communicate(){
    //emit deleteZFY(deleteNum);
}
