#include "hotplugthread.h"

QString insertNum = "AA";
QString deleteNum = "AA";
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

    uint8_t bus_number = libusb_get_bus_number(dev);

    uint8_t path[8];
    int ret = libusb_get_port_numbers(dev, path, sizeof(path));
    QString port_num;
    if (ret > 0) {

        port_num.append(QString("%1").arg(path[0], 0, 10));
        for (int j = 1; j < ret; ++j){

            port_num.append(".").append(QString("%1").arg(path[j], 0, 10));

        }
    }

    insertNum = QString::number(bus_number) + "-" + port_num;

    qDebug()<< "Device attached:"<< desc.idVendor<<";" <<desc.idProduct<<"insertNum"<<insertNum;

    QThread::msleep(500);
    //emit insertZFY();

    isInsert = true;
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

    uint8_t bus_number = libusb_get_bus_number(dev);

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
    deleteNum = QString::number(bus_number) + "-" + port_num;
    qDebug()<<"Device detached"<<"deleteNum"<<deleteNum;
    isInsert = false;

    //    if (handle) {
    //        libusb_close (handle);
    //        handle = NULL;
    //    }


    return 0;
}




HotPlugThread::HotPlugThread()
{

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

            if(isInsert){
                if(insertNum != "AA"){
                    qDebug()<<"insertNum"<<insertNum;
                    emit insertZFY(insertNum);
                    insertNum = "AA";
                    isInsert = false;
                }

            }else {
                if(deleteNum != "AA"){
                    qDebug()<<"deleteNum"<<deleteNum;
                    emit deleteZFY(deleteNum);
                    deleteNum = "AA";
                }

            }



        }



    }
}
