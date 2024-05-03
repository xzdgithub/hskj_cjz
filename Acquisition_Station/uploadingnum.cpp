#include "uploadingnum.h"

UploadingNum::UploadingNum()
{
    uploadingnum = 0;
    for (int i=0;i<30;i++) {
        iswaitcopy[i] = false;
    }

    uploadingsametime = Config().Get("num","uploadingsametime").toInt();
}

void UploadingNum::run()
{


    while (true) {
        for (int i=0;i<30;i++) {

            while (iswaitcopy[i]) {
                bool addresult = addUploadingnum();
                if(addresult){

                    qDebug()<<"iswaitcopy:"<<i;
                    //start copy
                    emit sigtocopy(i);
                    iswaitcopy[i] = false;
                }else {

                    QThread::msleep(1000);
                }
            }

        }
    }


}

void UploadingNum::subUploadingnum()
{
    //mutex.lock();
    qDebug()<<"sub__uploadingnum:"<<uploadingnum;
    QMutexLocker locker(&mutex);
    if(uploadingnum >0){
        uploadingnum -= 1;

    }

    locker.unlock();

    //return false;
}

void UploadingNum::setiswaitcopy(int num, bool value)
{
    iswaitcopy[num] = value;
}





bool UploadingNum::addUploadingnum()
{

    QMutexLocker locker(&mutex);
    if(uploadingnum < uploadingsametime){
        uploadingnum += 1;
        locker.unlock();
        return true;
    }else {
        locker.unlock();
        return false;
    }

}
