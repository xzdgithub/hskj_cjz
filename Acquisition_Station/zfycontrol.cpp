#include "zfycontrol.h"

ZFYControl::ZFYControl(bool autodeletecopyfile,QObject *parent) : QObject(parent)
{
    qDebug()<<"ZFYControl::start "<<zfynum<<endl;

    path = "";

    this->autodeletecopyfile = autodeletecopyfile;
    qRegisterMetaType<QVariant>("QVariant");
}

ZFYControl::ZFYControl(int zfynum)
{
    this->zfynum = zfynum;

}

ZFYControl::~ZFYControl()
{
    qDebug()<<"ZFYControl::~ZFYControl "<<zfynum<<endl;

}

void ZFYControl::mountToUdisk(int num)
{

    qDebug()<<"mountthread:"<<QThread::currentThreadId();
    //QThread::msleep(15000);
    QThread::msleep(2000);
    emit start_mount(num);
    QThread::msleep(3000);
    initMount();
    qDebug()<<"start to mount";
    QString mountPath = QString("%1%2%3").arg(Config().Get("path","mountpath").toString()).arg("zfy").arg(num+1);

    QString mountDev = QString("%1%2").arg("/dev/zfy").arg(num+1);
    qDebug()<<"mountDev"<<mountDev;
    qDebug()<<"mountPath"<<mountPath;
    mkMutiDir(mountPath);
    //    QString output;
    //    bool result = myProcess("/bin/mount", QStringList() <<mountDev <<mountPath , output);
    //    //bool result = myProcess("echo 123123123 | sudo -S mount", QStringList() <<mountDev <<mountPath  , output);
    //    //bool result = myProcess("mount", QStringList() << "/dev/zfy1" <<"/mnt/usb/zfy1" , output);
    //    qDebug()<<"output"<<output<<"mountresult:"<<result;


    QString password = "123123123";
    //QString mountText = "echo " + password+ " | sudo -S mount "+mountDev+" "+mountPath;
    QString mountText = "mount "+mountDev+" "+mountPath;
    //QString mountText ="mount -h";
    qDebug()<<"mountText"<<mountText;
    m_proces_bash->write(mountText.toLocal8Bit() + '\n');


    emit mount_finish(num);
//    QThread::msleep(2000);

//    QString filebattery = mountPath + "/LOG/BATTERY.TXT";
//    bool mountresult = isFileExist(filebattery);
//    qDebug()<<"mountresult:"<<mountresult;
//    if(mountresult){
//       emit mount_finish(num);
//    }


    //return false;
}


QFileInfoList ZFYControl::copyFile(QStringList *uuid,QString dirpath)

{
    qDebug()<<"mythread QThread::currentThreadId()=="<<QThread::currentThreadId();
    //QString dstPath = "/home/itventi/copyfile/";
    //QDir dir(path + "/LOG");

    QString mkmutidir = mkMutiDir(dirpath);
    qDebug()<<"mkmutidir"<<mkmutidir;
    //QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    QFileInfoList file_list = getAllFile();
    qDebug()<<"sizeall:"<<sizeall;
    QFileInfoList success_list;
    for(int i = 0; i < file_list.count(); i++)
    {

        if(file_list.at(i).fileName() != "BATTERY.TXT" && file_list.at(i).suffix()!="NMEA"){
            QString struuid = QUuid::createUuid().toString().remove("{").remove("}");
            QString dstFilePath = dirpath + struuid +"." + file_list.at(i).suffix();


            QFile::setPermissions(dstFilePath, QFile::WriteOwner);

            bool ok = QFile::copy(file_list.at(i).filePath(), dstFilePath);
            if(ok == true){
                uuid->append(struuid);
                success_list.append(file_list.at(i));

                copySize += file_list.at(i).size();
                qDebug()<<"copySize:"<<copySize;
                qDebug()<<"(copySize*100.00/sizeall)"<<(copySize*100.00/sizeall);
                //emit copyonesuccess((copySize*100.00/(sizeall*1.00)));
                QThread::msleep(50);
            }
            qDebug()<<"ok"<<ok<<"copy " <<file_list.at(i).filePath();
        }

    }

    return success_list;
}


QString ZFYControl::getBattery(){

    path = QString("%1%2%3").arg(Config().Get("path","mountpath").toString()).arg("zfy").arg(zfynum+1);
    //zfylist[0].charge
    QRegExp batteryText("Battery0=(\\d{1,2})");
    QString batteryNumber = "-1";
    QFile file(path + "/LOG/BATTERY.TXT");
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

void ZFYControl::closeThread()
{
    isStop = true;
}

void ZFYControl::copyFile2(QVariantMap map)
{


    //QThread::msleep(10000);
    qDebug()<<"copyFile2thread QThread::currentThreadId()=="<<QThread::currentThreadId();

    QString dirpath = map.value("dirpath","#-1").toString();
    QDateTime uploadDate = map.value("uploadDate","#-1").toDateTime();
    QString userid = map.value("userid","#-1").toString();
    QString driverid = map.value("driverid","#-1").toString();
    int num = map.value("num","#-1").toInt();
    //    if(recievezfynum != zfynum){
    //        return;
    //    }

    QString mkmutidir = mkMutiDir(dirpath);
    qDebug()<<"mkmutidir"<<mkmutidir;
    //QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);


    //QString mountPath = QString("%1%2%3").arg(Config().Get("path","mountpath").toString()).arg("zfy").arg(num+1)
    path = QString("%1%2%3").arg(Config().Get("path","mountpath").toString()).arg("zfy").arg(zfynum+1);
    QFileInfoList file_list = getAllFile();
    qDebug()<<"sizeall:"<<sizeall;

    QVariantMap gpsMap;
    QFileInfoList gpsfile;
    for(int i = 0; i < file_list.count(); i++)
    {
        //if file suffix is NMEA ,add to list.
        if(file_list.at(i).suffix() == "NMEA"){
            QString result;
            qDebug()<<"coor file path:"<<file_list.at(i).path() +"/"+file_list.at(i).fileName();
            QFile file(file_list.at(i).path() +"/"+ file_list.at(i).fileName());
            gpsfile.append(file);
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
            gpsMap[file_list.at(i).completeBaseName()] = result;
        }
    }

    for(int i = 0; i < file_list.count(); i++)
    {
        //QThread::msleep(1000);

        if(file_list.at(i).fileName() != "BATTERY.TXT" && file_list.at(i).suffix()!="NMEA"){

            //            if(file_list.at(i).fileName() &&){

            //            }
            QString struuid = QUuid::createUuid().toString().remove("{").remove("}");
            QString dstFilePath = dirpath + struuid +"." + file_list.at(i).suffix();



            QFile::setPermissions(dstFilePath, QFile::WriteOwner);

            bool ok = QFile::copy(file_list.at(i).filePath(), dstFilePath);
            if(ok == true){
                File_copy_Info file_copy_one;
                file_copy_one.file_uuid = struuid;
                file_copy_one.file_name = file_list.at(i).fileName();
                file_copy_one.file_creattime = file_list.at(i).created();
                file_copy_one.file_size = file_list.at(i).size();
                file_copy_one.file_est = file_list.at(i).completeSuffix();
                file_copy_one.file_path = dirpath;
                file_copy_one.file_uploadtime = uploadDate;
                file_copy_one.user_id = userid;
                file_copy_one.driver_id = driverid;
                if(file_list.at(i).suffix() =="JPG"){
                    file_copy_one.file_type = 1;
                    file_copy_one.file_gps = QString("");
                }else if(file_list.at(i).suffix() =="MP4"){
                    file_copy_one.file_type = 2;
                    file_copy_one.file_gps = gpsMap.value(file_list.at(i).completeBaseName(),QString("")).toString();
                }else if(file_list.at(i).suffix() =="TXT"){
                    file_copy_one.file_type = 4;
                    file_copy_one.file_gps = QString("");
                }

                //copylist.append(file_copy_one);
                copySize += file_list.at(i).size();
                qDebug()<<"copySize:"<<copySize;
                qDebug()<<"(copySize*100.00/sizeall)"<<"zfynum "<<zfynum<<" "<<(copySize*100.00/sizeall);

                QVariant v;
                v.setValue(file_copy_one);
                emit copyonesuccess(v,(copySize*100.00/(sizeall*1.00)),zfynum);
                //QThread::msleep(50);


                if(autodeletecopyfile){
                    QFile file_delete(file_list.at(i).filePath());
                    file_delete.remove();
                    file_delete.close();
                }

            }
            qDebug()<<"ok"<<ok<<"copy " <<file_list.at(i).filePath();
        }



    }


    if(autodeletecopyfile){
        for (int i=0;i<gpsfile.count();i++) {
            QFile file_delete(gpsfile.at(i).filePath());
            file_delete.remove();
            file_delete.close();
        }
    }






}

void ZFYControl::readBashStandardOutputInfo()
{
    qDebug()<<"readBashStandardOutputInfo";
    QByteArray cmdout = m_proces_bash->readAllStandardOutput();
    if(!cmdout.isEmpty()){
        //ui->textEdit_bashmsg->append(QString::fromLocal8Bit(cmdout));
        qDebug()<<"readBashStandardOutputInfo"<<QString::fromLocal8Bit(cmdout);
    }
}

void ZFYControl::readBashStandardErrorInfo()
{
    qDebug()<<"readBashStandardErrorInfo";
    QByteArray cmdout = m_proces_bash->readAllStandardError();
    if(!cmdout.isEmpty()){
        //ui->textEdit_bashmsg->append(QString::fromLocal8Bit(cmdout));
        qDebug()<<"readBashStandardErrorInfo"<<QString::fromLocal8Bit(cmdout);

        //"mount: special device /dev/zfy1 does not exist\n"
        if(QString::fromLocal8Bit(cmdout).contains("special device") &&QString::fromLocal8Bit(cmdout).contains("does not exist")){
            emit mount_fail();
        }
    }
}

void ZFYControl::setPath(const QString &value)
{
    path = value;
}


QString ZFYControl::mkMutiDir(const QString path){
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

QFileInfoList ZFYControl::getAllFile(){
    QDir dirLog( + "/LOG");
    QDir dirMovie(path + "/DCIM/Movie");
    QDir dirPhoto(path + "/DCIM/Photo");

    QFileInfoList file_log = dirLog.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    //qDebug()<<"path:"<<path;
    //qDebug()<<"dirLogpath:"<<dirLog.path();
    //qDebug()<<"file_log.count():"<<file_log.count();
    QFileInfoList file_photo = dirPhoto.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    QFileInfoList file_movie = dirMovie.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);

    QFileInfoList file_all;

    file_all.append(file_log);
    file_all.append(file_photo);
    file_all.append(file_movie);

    if(!file_all.isEmpty()){
        for (int i=0;i<file_all.count();i++) {
            if(file_all.at(i).fileName() != "BATTERY.TXT" && file_all.at(i).suffix()!="NMEA"){
                sizeall += file_all.at(i).size();
            }
        }
    }

    return  file_all;
}

void ZFYControl::setZfynum(int value)
{
    zfynum = value;
}

bool ZFYControl::myProcess(QString cmd, QStringList param, QString &output)
{
    QProcess process;
    process.start(cmd, param);
    // 等待进程启动
    if (!process.waitForStarted())
    {
        qDebug() <<  "命令执行开始失败\n";
        return false;
    }
    process.closeWriteChannel();

    // 用于保存进程的控制台输出
    QByteArray procOutput;
    while (!process.waitForFinished(1000))
    {
        qDebug() <<  "命令执行结束失败\n";
        return false;
    }
    procOutput = process.readAll();
    output = procOutput;
    return true;
}
void ZFYControl::initMount(){
    m_proces_bash = new QProcess;
    m_proces_bash->start("bash");
    m_proces_bash->waitForStarted();
    qDebug()<<"initMount";
    connect(m_proces_bash,SIGNAL(readyReadStandardOutput()),this,SLOT(readBashStandardOutputInfo()));
    connect(m_proces_bash,SIGNAL(readyReadStandardError()),this,SLOT(readBashStandardErrorInfo()));
}
void ZFYControl::clearSize(){
    copySize = 0;
    sizeall = 0;
}
bool ZFYControl::isDirExist(QString dirpath){
    QDir dir(dirpath);
    qDebug()<<"isDirExist dir:"<<dirpath;
    if(dir.exists()){
        return true;
    }else {
        return false;
    }
}
bool ZFYControl::isFileExist(QString filepath){
    QFileInfo file(filepath);
    qDebug()<<"isFileExist file:"<<filepath;
    if(file.exists()){
        return true;
    }else {
        return false;
    }
}
