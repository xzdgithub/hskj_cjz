#include "mysqllite.h"

MySqlLite::MySqlLite(QObject *parent) : QObject(parent)
{

    fileName = Config().Get("path","dbpath").toString();
    qDebug()<<"fileName"<<fileName;
    emit insertsuccesstest();
}
void MySqlLite::checkconnection(){

    // 2、检查数据库是否关闭
    bool isOpen = sqliteDatabase.isOpen();

    // 1.1 若已经打开数据库
    if (true  == isOpen)
    {
        // 1.1.1 关闭数据库
        sqliteDatabase.close();
    }
    else
    {
        // 1.1.2 数据库没有打开
#ifdef QT_DEBUG
        qDebug() << "数据库没有打开, 什么也不做";
#endif
    }
}
void MySqlLite::openmysql(){
    sqliteDatabase = QSqlDatabase::addDatabase("QSQLITE");
    sqliteDatabase.setDatabaseName(fileName);
    //    if (QSqlDatabase::contains(fileName)) {
    //        sqliteDatabase = QSqlDatabase::database(fileName);
    //    } else {
    //        sqliteDatabase = QSqlDatabase::addDatabase("QSQLITE", fileName);
    //    }

    // 1.1 尝试打开数据库
    bool openFlag  = sqliteDatabase.open();

    // 1.2  若打开失败
    if (false  == openFlag)
    {
        //QMessageBox::critical(this, tr("warning"),  tr("open database file err"));
        qDebug()<<"open database file err";
        return;
    }

    sql_query = new QSqlQuery();
}
QStringList MySqlLite::gettableNameList(){
    // 2、读取数据库中的所有表名， 排除重复的表名 和 sqlite_sequence
    QStringList tableNameList   = sqliteDatabase.tables();
#ifdef QT_DEBUG
    qDebug() << "查询的表名的个数= " << tableNameList.count();
#endif


    // 读取数据库中的表名
    realTableName.clear();
    int realTableNameCount  = 0;
    bool isFind             = false;
    foreach (QString name, tableNameList)
    {
        isFind              = false;
        // sqlite_sequence 排除
        if ( QString("sqlite_sequence") == name)
        {
            continue;
        }

        realTableNameCount  = realTableName.count();
        // 若已经存在了，跳过
        for (int i = 0; i < realTableNameCount; i++)
        {
            // 已经存在
            if (name    == realTableName.at(i))
            {
                isFind  = true;
                break;
            }
        }
        // 若没找到， 说名name是唯一存在的， 则添加到数组
        if (false       == isFind)
        {
            // 若没有找到，则添加到列表中
            realTableName << name;
        }
    }

    return realTableName;
}

void MySqlLite::closeSqlLite(){
    // 3、文件存在，判断是否打开
    bool isOpen = sqliteDatabase.isOpen();
    // 3.1 若没有打开
    if (false == isOpen)
    {
#ifdef QT_DEBUG
        qDebug() << "数据库没有打开";
        return ;
#endif
    }
    // 3.2 打开了数据库，再关闭
    else
    {
        sqliteDatabase.close();
#ifdef QT_DEBUG
        qDebug() << "数据库已经打开，现在已经关了";
#endif
    }

    // 4、移除数据库连接
    QSqlDatabase::removeDatabase("QSQLITE");
}
void MySqlLite::readTable(){

    if(!sql_query->exec("select * from staffs"))
    {
        qDebug()<<"sql_query"<<sql_query->lastError();
    }
    else
    {
        while(sql_query->next())
        {
            int id = sql_query->value(0).toInt();
            QString name = sql_query->value(1).toString();
            QString photo_url = sql_query->value(2).toString();
            qDebug()<<QString("id:%1    name:%2    photo_url:%3").arg(id).arg(name).arg(photo_url);
        }
    }

}


void MySqlLite::insertb_user(){

}

void MySqlLite::gettb_files_all(int type_jur,QString userid)
{
    QList<FileInfo> file_all;
    QString execText;
    if(type_jur == 0){
        execText = "select * from tb_files";
    }if(type_jur == 1){
        execText = QString("%1\"%2\"").arg("select * from tb_files where user_id = ").arg(userid);
    }
    qDebug()<<"gettb_files_all execText:"<<execText;
    if(!sql_query->exec(execText))
    {
        qDebug()<<"sql_query"<<sql_query->lastError();
    }
    else
    {
        while(sql_query->next())
        {
            FileInfo fileinfo;

            fileinfo.file_name = sql_query->value(1).toString();
            fileinfo.file_date = sql_query->value(4).toString();

            fileinfo.file_remark = sql_query->value(12).toString();
            fileinfo.file_est = sql_query->value(14).toString();
            fileinfo.file_shortpath = sql_query->value(13).toString();
            fileinfo.filepath = sql_query->value(13).toString()
                    + sql_query->value(0).toString()+QString(".")+ sql_query->value(14).toString();//file_path + uuid + "." + file_est;
            fileinfo.file_upload_time = sql_query->value(16).toString();
            fileinfo.file_userid = sql_query->value(2).toString();
            fileinfo.file_driverid = sql_query->value(3).toString();
            file_all.append(fileinfo);
        }
        if(sql_query->last()){
            emit gettb_files_success(file_all);
        }
    }

}

void MySqlLite::gettb_files_video(int type_jur,QString userid)
{

    QList<FileInfo> file_video;

    QString execText;
    if(type_jur == 0){
        execText = "select * from tb_files where file_est = \"MP4\"";
    }if(type_jur == 1){
        execText = QString("%1\"%2\"").arg("select * from tb_files where file_est = \"MP4\" and user_id = ").arg(userid);
    }
    qDebug()<<"gettb_files_all execText:"<<execText;

    if(!sql_query->exec(execText))
    {
        qDebug()<<"sql_query"<<sql_query->lastError();
    }
    else
    {
        while(sql_query->next())
        {
            FileInfo fileinfo;

            fileinfo.file_name = sql_query->value(1).toString();
            fileinfo.file_date = sql_query->value(4).toString();
            fileinfo.file_remark = sql_query->value(12).toString();
            fileinfo.file_est = sql_query->value(14).toString();
            fileinfo.file_shortpath = sql_query->value(13).toString();
            fileinfo.filepath = sql_query->value(13).toString()
                    + sql_query->value(0).toString()+QString(".")+ sql_query->value(14).toString();//file_path + uuid + "." + file_est;

            fileinfo.file_upload_time = sql_query->value(16).toString();
            fileinfo.file_userid = sql_query->value(2).toString();
            fileinfo.file_driverid = sql_query->value(3).toString();
            file_video.append(fileinfo);
        }
        if(sql_query->last()){
            emit gettb_files_success(file_video);
        }
    }
}

void MySqlLite::gettb_files_photo(int type_jur,QString userid)
{
    QList<FileInfo> file_photo;

    QString execText;
    if(type_jur == 0){
        execText = "select * from tb_files where file_type = 1";
    }if(type_jur == 1){
        execText = QString("%1\"%2\"").arg("select * from tb_files where file_type = 1 and user_id = ").arg(userid);
    }
    qDebug()<<"gettb_files_all execText:"<<execText;


    if(!sql_query->exec(execText))
    {
        qDebug()<<"sql_query"<<sql_query->lastError();
    }
    else
    {
        while(sql_query->next())
        {
            FileInfo fileinfo;
            fileinfo.file_name = sql_query->value(1).toString();
            fileinfo.file_date = sql_query->value(4).toString();
            fileinfo.file_remark = sql_query->value(12).toString();
            fileinfo.file_est = sql_query->value(14).toString();
            fileinfo.file_shortpath = sql_query->value(13).toString();
            fileinfo.filepath = sql_query->value(13).toString()
                    + sql_query->value(0).toString()+QString(".")+ sql_query->value(14).toString();//file_path + uuid + "." + file_est;
            fileinfo.file_upload_time = sql_query->value(16).toString();
            fileinfo.file_userid = sql_query->value(2).toString();
            fileinfo.file_driverid = sql_query->value(3).toString();

            file_photo.append(fileinfo);
        }
        if(sql_query->last()){
            emit gettb_files_success(file_photo);
        }
    }
}

void MySqlLite::gettb_files_log(int type_jur,QString userid)
{
    QList<FileInfo> file_log;

    QString execText;
    if(type_jur == 0){
        execText = "select * from tb_files where file_est = \"TXT\"";
    }if(type_jur == 1){
        execText = QString("%1\"%2\"").arg("select * from tb_files where file_est = \"TXT\" and user_id = ").arg(userid);
    }
    qDebug()<<"gettb_files_all execText:"<<execText;


    if(!sql_query->exec(execText))
    {
        qDebug()<<"sql_query"<<sql_query->lastError();
    }
    else
    {
        while(sql_query->next())
        {
            FileInfo fileinfo;
            fileinfo.file_name = sql_query->value(1).toString();
            fileinfo.file_date = sql_query->value(4).toString();
            fileinfo.file_remark = sql_query->value(12).toString();
            fileinfo.file_est = sql_query->value(14).toString();
            fileinfo.file_shortpath = sql_query->value(13).toString();
            fileinfo.filepath = sql_query->value(13).toString()
                    + sql_query->value(0).toString()+QString(".")+ sql_query->value(14).toString();//file_path + uuid + "." + file_est;
            fileinfo.file_upload_time = sql_query->value(16).toString();
            fileinfo.file_userid = sql_query->value(2).toString();
            fileinfo.file_driverid = sql_query->value(3).toString();

            file_log.append(fileinfo);
        }
        if(sql_query->last()){
            emit gettb_files_success(file_log);
        }
    }
}

void MySqlLite::gettb_files_audio(int type_jur, QString userid)
{
    QList<FileInfo> file_audio;

    QString execText;
    if(type_jur == 0){
        execText = "select * from tb_files where file_est = \"m4a\"";
    }if(type_jur == 1){
        execText = QString("%1\"%2\"").arg("select * from tb_files where file_est = \"m4a\" and user_id = ").arg(userid);
    }
    qDebug()<<"gettb_files_all execText:"<<execText;


    if(!sql_query->exec(execText))
    {
        qDebug()<<"sql_query"<<sql_query->lastError();
    }
    else
    {
        while(sql_query->next())
        {
            FileInfo fileinfo;
            fileinfo.file_name = sql_query->value(1).toString();
            fileinfo.file_date = sql_query->value(4).toString();
            fileinfo.file_remark = sql_query->value(12).toString();
            fileinfo.file_est = sql_query->value(14).toString();
            fileinfo.file_shortpath = sql_query->value(13).toString();
            fileinfo.filepath = sql_query->value(13).toString()
                    + sql_query->value(0).toString()+QString(".")+ sql_query->value(14).toString();//file_path + uuid + "." + file_est;
            fileinfo.file_upload_time = sql_query->value(16).toString();
            fileinfo.file_userid = sql_query->value(2).toString();
            fileinfo.file_driverid = sql_query->value(3).toString();

            file_audio.append(fileinfo);
        }
        if(sql_query->last()){
            emit gettb_files_success(file_audio);
        }
    }
}

//select * from tb_files where create_time BETWEEN '2020-01-21 00:00' and '2020-01-21 24::00'
void MySqlLite::gettb_files_bydate(int type,QString userid,QDate date)
{
    QString datestr_start = date.toString("yyyy-MM-dd");
    QString datestr_end = date.addDays(1).toString("yyyy-MM-dd");

    QString execstr;
    if(type == 0){
        execstr = QString("%1 \"%2\" %3 \"%4\"").arg("select * from tb_files where create_time BETWEEN").arg(datestr_start).arg("and").arg(datestr_end);
    }if(type == 1){
        execstr = QString("%1 \"%2\" %3 \"%4\" %5 \"%6\"").arg("select * from tb_files where user_id =")
                .arg(userid).arg("and create_time BETWEEN").arg(datestr_start)
                .arg("and").arg(datestr_end);
    }

    qDebug()<<"execstr"<<execstr;
    QList<FileInfo> file_log;
    if(!sql_query->exec(execstr))
    {
        qDebug()<<"sql_query"<<sql_query->lastError();
    }
    else
    {
        while(sql_query->next())
        {
            FileInfo fileinfo;
            fileinfo.file_name = sql_query->value(1).toString();
            fileinfo.file_date = sql_query->value(4).toString();
            //qDebug()<<"file_date"<<fileinfo.file_date;
            fileinfo.file_remark = sql_query->value(12).toString();
            fileinfo.file_est = sql_query->value(14).toString();
            fileinfo.file_shortpath = sql_query->value(13).toString();
            fileinfo.filepath = sql_query->value(13).toString()
                    + sql_query->value(0).toString()+QString(".")+ sql_query->value(14).toString();//file_path + uuid + "." + file_est;
            fileinfo.file_upload_time = sql_query->value(16).toString();
            fileinfo.file_userid = sql_query->value(2).toString();
            fileinfo.file_driverid = sql_query->value(3).toString();

            file_log.append(fileinfo);
        }
        if(sql_query->last()){
            emit gettb_files_success(file_log);
        }
    }
}

void MySqlLite::get_tb_files_type(int type_jur,QString userid,int type)
{
    if(!sqliteDatabase.isOpen()){
        openmysql();
    }
    if(type == 0){
        gettb_files_all(type_jur,userid);
    }else if(type == 1){
        gettb_files_video(type_jur,userid);
    }else if(type == 2){
        gettb_files_photo(type_jur,userid);
    }else if(type == 3){
        gettb_files_log(type_jur,userid);
    }else if(type == 4){
        gettb_files_audio(type_jur,userid);
    }

}

void MySqlLite::get_tb_files_withdate(int type,QString userid,QDate date)
{
    qDebug()<<"date"<<date;
    if(!sqliteDatabase.isOpen()){
        openmysql();
    }
    gettb_files_bydate(type,userid,date);
}

void MySqlLite::get_tb_users()
{
    if(!sqliteDatabase.isOpen()){
        openmysql();
    }
    QList<UserInfo> user_info;
    //SELECT DISTINCT user_id FROM tb_files
    //select * from tb_user
    if(!sql_query->exec("SELECT DISTINCT user_id FROM tb_files"))
    {
        qDebug()<<"sql_query"<<sql_query->lastError();
    }
    else
    {
        while(sql_query->next())
        {
            UserInfo user_one;
            //user_one.user_id = sql_query->value(0).toInt();
            user_one.user_name = sql_query->value(0).toString();
            //user_one.user_photo_url = sql_query->value(2).toString();
            user_info.append(user_one);
        }
        if(sql_query->last()){
            emit gettb_user_success(user_info);
        }
    }
}

void MySqlLite::get_tb_files_withuserName(QString username)
{
    //"select * from tb_files ,tb_user  where tb_files.user_id=tb_user.id AND tb_user.name = 'Wang'"
    QList<FileInfo> file_with_user;
    //QString execstr = QString("%1\"%2\"").arg("select * from tb_files ,tb_user  where tb_files.user_id=tb_user.id AND tb_user.name =").arg(username);
    QString execstr = QString("%1\"%2\"").arg("select * from tb_files   where user_id= ").arg(username);
    qDebug()<<"execstr:"<<execstr;
    if(!sql_query->exec(execstr))
    {
        qDebug()<<"sql_query"<<sql_query->lastError();
    }
    else
    {
        while(sql_query->next())
        {
            FileInfo fileinfo;

            fileinfo.file_name = sql_query->value(1).toString();
            fileinfo.file_date = sql_query->value(4).toString();
            fileinfo.file_remark = sql_query->value(12).toString();
            fileinfo.file_est = sql_query->value(14).toString();
            fileinfo.file_shortpath = sql_query->value(13).toString();
            fileinfo.filepath = sql_query->value(13).toString()
                    + sql_query->value(0).toString()+QString(".")+ sql_query->value(14).toString();//file_path + uuid + "." + file_est;
            fileinfo.file_upload_time = sql_query->value(16).toString();
            fileinfo.file_userid = sql_query->value(2).toString();
            fileinfo.file_driverid = sql_query->value(3).toString();


            file_with_user.append(fileinfo);
        }
        if(sql_query->last()){
            emit gettb_files_success(file_with_user);
        }
    }

}

void MySqlLite::doinsertfiles(QVariantMap copyfilemap)
{
    if(!sqliteDatabase.isOpen()){
        openmysql();
    }
    //File_copy_Info copy_file_info = dataVar.value<File_copy_Info>();


    //QString file_uuid = copyfilemap.value("").toString();

    //qDebug()<<"uuid"<<copy_file_info.file_uuid;
    //    bool insert_result = insertb_files(copy_file_info.file_uuid,copy_file_info.file_name,
    //                                       copy_file_info.user_id,copy_file_info.driver_id,
    //                                       copy_file_info.file_creattime,copy_file_info.file_uploadtime,
    //                                       copy_file_info.file_size,copy_file_info.file_type,copy_file_info.file_path,
    //                                       copy_file_info.file_est,copy_file_info.file_gps);


    insertb_files(copyfilemap.value("file_uuid").toString(),copyfilemap.value("file_name").toString(),
                  copyfilemap.value("user_id").toString(),copyfilemap.value("driver_id").toString(),
                  copyfilemap.value("file_creattime").toDateTime(),copyfilemap.value("file_uploadtime").toDateTime(),
                  copyfilemap.value("file_size").toLongLong(),copyfilemap.value("file_type").toInt(),copyfilemap.value("file_path").toString(),
                  copyfilemap.value("file_est").toString(),copyfilemap.value("file_gps").toString(),
                  copyfilemap.value("virtualpath").toString());
    //    if(insert_result){
    //        emit insertsuccess(dataVar);
    //    }

}

bool MySqlLite::isfileexit(QString filename,QString driverid)
{
    if(!sqliteDatabase.isOpen()){
        openmysql();
    }

    QString updateext = QString("%1\"%2\"%3\"%4\"%5\"%6\"")
            .arg("select tb_files where id = ")
            .arg(filename)
            .arg(" and id = ");

    qDebug()<<"updateext"<<updateext;
    if(!sql_query->exec(updateext))
    {
        qDebug() << sql_query->lastError();

    }


}

void MySqlLite::WriteCutSql(QVariantMap cutmap)
{

    if(!sqliteDatabase.isOpen()){
        openmysql();
    }


    insertb_files(cutmap.value("id").toString(),cutmap.value("file_name").toString(),
                  cutmap.value("user_id").toString(),cutmap.value("driver_id").toString(),
                  cutmap.value("create_time").toDateTime(),cutmap.value("upload_time").toDateTime(),
                  cutmap.value("file_size").toLongLong(),cutmap.value("file_type").toInt(),cutmap.value("file_path").toString(),
                  cutmap.value("file_est").toString(),cutmap.value("coordinates").toString(),cutmap.value("virtualpath").toString());

}

void MySqlLite::toupdateFromMq(QVariantMap map)
{

    if(!sqliteDatabase.isOpen()){
        openmysql();
    }

    //map.value("CheckCode") map.value("CreateTime") map.value("DevNo") map.value("TaskId")


    QString updateext = QString("%1\"%2\"%3\"%4\"%5\"%6\"")
            .arg("update tb_files set driver_id = ")
            .arg(map.value("DevNo").toString())
            .arg(", create_time = ")
            .arg(map.value("CreateTime").toDateTime().toString("yyyy-MM-dd HH:mm:ss"))
            .arg(" where id = ")
            .arg(map.value("CheckCode").toString());
    qDebug()<<"updateext"<<updateext;
    if(!sql_query->exec(updateext))
    {
        qDebug() << sql_query->lastError();
        emit sendConfirmtoSerVer(0,map.value("TaskId").toString());
    }
    else
    {
        qDebug() << "updated success!";
        //need to inseet activemq
        emit sendConfirmtoSerVer(1,map.value("TaskId").toString());
    }
}

void MySqlLite::todeleteFromMq(QVariantMap map)
{
    if(!sqliteDatabase.isOpen()){
        openmysql();
    }
    //need to find out the file name and then delete the sql
    //map.value("CheckCode");
    //QString execText = "delete from tb_files where id = "+ map.value("CheckCode").toString();

    QString findText = QString("%1\"%2\"")
            .arg("select * from tb_files where id = ").arg(map.value("CheckCode").toString());
    QString filepath;
    qDebug()<<"findText:"<<findText;
    if(!sql_query->exec(findText))
    {
        qDebug()<<"sql_query"<<sql_query->lastError();
    }
    else
    {
        while(sql_query->next())
        {

            filepath = sql_query->value(13).toString()
                    + sql_query->value(0).toString()+QString(".")+ sql_query->value(14).toString();//file_path + uuid + "." + file_est;

        }
        if(sql_query->last()){
            qDebug()<<"delete file:"<<filepath;
            QFile file(filepath);
            file.remove();



        }
    }


    QString deleteText = QString("%1\"%2\"")
            .arg("delete from tb_files where id = ").arg(map.value("CheckCode").toString());
    qDebug()<<"deleteText:"<<deleteText;
    if(!sql_query->exec(deleteText))
    {
        qDebug()<<sql_query->lastError();
    }
    else
    {
        qDebug()<<"delete success!";
    }






}

void MySqlLite::insert_AcStation_Log(QVariantMap map)
{
    if(!sqliteDatabase.isOpen()){
        openmysql();
    }
    QString uuid =map.value("TaskId","#-1").toString();
    int opera_type =map.value("OperaType","-1").toInt();
    //qDebug()<<"opera_type"<<opera_type;
    QString opera_content =map.value("OperaContent","#-1").toString();
    QDateTime opera_time = map.value("Time","#-1").toDateTime();
    QString sSQL = "INSERT INTO tb_station_log (id,opera_type,opera_content,opera_time)"
                   " VALUES";

    QString s2 = QString("(\"%1\",%2,\"%3\",\"%4\")")
            .arg(uuid).arg(opera_type).arg(opera_content).arg(opera_time.toString("yyyy-MM-dd HH:mm:ss"));
    QString execText = sSQL +s2;
    qDebug()<<"insert_AcStation_Log_execText:"<<execText;
    if(!sql_query->exec(execText))
    {
        qDebug() << sql_query->lastError();


    }
    else
    {
        qDebug() << "insert AcStation_Log sql success!";

        //insert success,to add activemq

        emit insertlogsuccess(2,map);
    }


}

void MySqlLite::insert_User_Log(QVariantMap map)
{

    if(!sqliteDatabase.isOpen()){
        openmysql();
    }

    QString uuid =map.value("TaskId","#-1").toString();
    int opera_type =map.value("OperaType","-1").toInt();
    QString opera_content =map.value("OperaContent","#-1").toString();
    QDateTime opera_time = map.value("Time","#-1").toDateTime();
    QString driver_id =map.value("DevNo","-1").toString();
    QString user_id =map.value("PolNo","#-1").toString();
    QString sSQL = "INSERT INTO tb_user_log (id,user_id,opera_type,opera_content,driver_id,opera_time)"
                   " VALUES";

    QString s2 = QString("(\"%1\",\"%2\",%3,\"%4\",\"%5\",\"%6\")")
            .arg(uuid).arg(user_id).arg(opera_type)
            .arg(opera_content).arg(driver_id)
            .arg(opera_time.toString("yyyy-MM-dd HH:mm:ss"));
    QString execText = sSQL +s2;
    qDebug()<<"insert_User_Log_execText:"<<execText;
    if(!sql_query->exec(execText))
    {
        qDebug() << sql_query->lastError();


    }
    else
    {
        qDebug() << "insert User_Log sql success!";

        //insert success,to add activemq

        emit insertlogsuccess(3,map);
    }

}

void MySqlLite::slotcheckfile(QFileInfoList filelist, QString driverid, int num)
{
    qDebug()<<"filelist.count():"<<filelist.count()<<"driverid:"<<driverid<<"num:"<<num;
    if(!sqliteDatabase.isOpen()){
        openmysql();
    }
    QFileInfoList filenocopy;
    for (int i=0;i<filelist.count();i++) {

        //SELECT COUNT(*) FROM tb_files WHERE driver_id = '1234567' AND file_name = '202200101_999999.TXT'

        QString execText = QString("%1\"%2\" %3\"%4\"")
                .arg("select COUNT(*) from tb_files where driver_id = ").arg(driverid)
                .arg("and file_name = ").arg(filelist.at(i).fileName());

        qDebug()<<"slotcheckfile execText:"<<execText;
        if(!sql_query->exec(execText))
        {
            qDebug()<<"sql_query"<<sql_query->lastError();
        }
        else
        {
            //sql_query->
            if(sql_query->next())
            {

                int num = sql_query->value(0).toInt();
                qDebug()<<"filelist.at(i).fileName()"<<filelist.at(i).fileName()<<num;

                if(num == 0){
                    filenocopy.append(filelist.at(i));
                }
            }

        }



    }

    emit sig_filenocopy(filenocopy,num);

}

//INSERT INTO tb_files (id, file_name,user_id,driver_id,create_time,upload_time,file_size,file_type,save_max_day,status,is_important,case_no,remark,file_path,file_est) VALUES ('uuid', 'Champs-Elysees')
bool MySqlLite::insertb_files(QString uuid,QString file_name,QString user_id,QString driver_id,QDateTime create_time,QDateTime uploadtime,qint64 file_size,int file_type,QString file_path,QString file_est,QString file_gps,QString virtualpath){
    QDateTime upload_finish_time = QDateTime::currentDateTime();
    qDebug()<<"insertb_files";
    QString sSQL = "INSERT INTO tb_files (id, file_name,user_id,driver_id,create_time,upload_time,"
                   "file_size,file_type,save_max_day,status,"
                   "is_important,case_no,remark,"
                   "file_path,file_est,coordinates,upload_finish_time,virtualpath)"
                   " VALUES";
    //qDebug()<<"sSQL"<<sSQL;


    QString s2 = QString("(\"%1\",\"%2\",\"%3\",\"%4\",\"%5\",\"%6\",%7,")
            .arg(uuid,file_name).arg(user_id).arg(driver_id).arg(create_time.toString("yyyy-MM-dd HH:mm:ss")).arg(uploadtime.toString("yyyy-MM-dd HH:mm:ss")).arg(file_size);

    //qDebug()<<"s2"<<s2;

    QString s3 = QString("%1,%2,%3,%4,\"%5\",\"%6\",\"%7\",\"%8\",\"%9\"")
            .arg(file_type).arg(1).arg(1).arg(0)
            .arg("case_no","remark",file_path,file_est)
            .arg(file_gps);

    QString s4 = QString(",\"%1\",\"%2\")").arg(upload_finish_time.toString("yyyy-MM-dd HH:mm:ss")).arg(virtualpath);
    QString execText = sSQL + s2 + s3 + s4;

    qDebug()<<"execText"<<execText;


    if(!sql_query->exec(execText))
    {
        qDebug() << sql_query->lastError();
        return false;
    }
    else
    {
        qDebug() << "insertsql success!";

        //insert success,to add activemq

        FileAllInfo file_all;
        file_all.file_uuid = uuid;
        file_all.file_name = file_name;
        file_all.user_id = user_id;
        file_all.driver_id = driver_id;
        file_all.create_time = create_time;
        file_all.upload_time = upload_finish_time;
        file_all.file_caseNo = "case_no";
        file_all.file_size = file_size;
        file_all.file_type = file_type;
        file_all.save_max_day = 1;
        file_all.is_important = 0;
        QString textpath = QString("%1/%2/%3/")
                .arg(virtualpath)
                .arg(uploadtime.toString("yyyy-MM-dd"))
                .arg(user_id);
        file_all.file_gps = file_gps;
        file_all.fileshowpath = textpath + uuid + "." + file_est;

        QVariant v;
        v.setValue(file_all);
        emit insertsuccess(v);


        //emit insertsuccesstest();
        return true;


    }


}

void MySqlLite::insertData(){
    if(!sql_query->exec("INSERT INTO tb_user VALUES(12345, \"Wang\", \"url1\")"))
    {
        qDebug() << sql_query->lastError();
    }
    else
    {
        qDebug() << "inserted Wang!";
    }
    if(!sql_query->exec("INSERT INTO tb_user VALUES(2, \"Li\", \"url2\")"))
    {
        qDebug() << sql_query->lastError();
    }
    else
    {
        qDebug() << "inserted Li!";
    }
}
void MySqlLite::updateData(){

    if(!sql_query->exec("update tb_user set name = \"QT\" where id = 1"))
    {
        qDebug() << sql_query->lastError();
    }
    else
    {
        qDebug() << "updated!";
    }

}
void MySqlLite::deleteData(){

    if(!sql_query->exec("delete from staffs where id = 1"))
    {
        qDebug()<<sql_query->lastError();
    }
    else
    {
        qDebug()<<"deleted!";
    }

}
bool MySqlLite::exec(QString text){

    if(!sql_query->exec(text))
    {
        qDebug()<<sql_query->lastError();
        return false;
    }
    else
    {
        qDebug()<<"success!";
        return true;
    }

}
