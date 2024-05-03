#include "loginform.h"
#include "ui_loginform.h"


LoginForm::LoginForm(int type,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginForm)
{
    ui->setupUi(this);

    this->setWindowTitle(tr("Login"));
    this->type = type;
    //Qt::WindowFlags windowflag = Qt::Dialog;

    //windowflag  |= Qt::WindowCloseButtonHint;
    setWindowFlags(Qt::WindowCloseButtonHint);
    setFixedSize(this->width(),this->height());
    naManager = new QNetworkAccessManager(this);
    connect(naManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));


    QRegExp regx("[0-9A-Za-z]{6}");
    QValidator *validator = new QRegExpValidator(regx,ui->lineEdit_username);
    ui->lineEdit_username->setValidator(validator);


}



LoginForm::~LoginForm()
{
    qDebug()<<"~LoginForm!!";
    delete ui;
}

void LoginForm::on_pushButton_clicked()
{

//    this->hide();

//    MainWindow *main = new MainWindow(this);
//    main->show();
//    this->destroy();

//    ui->lineEdit_username->text();
//    ui->lineEdit_password->text();
    if(ui->lineEdit_username->text()=="" || ui->lineEdit_password->text() ==""){

        qDebug()<<"username or password can't be null!";


        QMessageBox::warning(NULL, tr("INFO"), tr("username or password can't be null!"), QMessageBox::Ok );

    }else{
        QString postData = QString("%1%2%3%4")
                .arg("username=").arg(ui->lineEdit_username->text())
                .arg("&password=").arg(ui->lineEdit_password->text());
        QString url = Config().Get("server","serverIp").toString() +"/api/Login/AjaxLogin";
        qDebug()<<"loginurl:"<<url;
        request.setUrl(QUrl(url));
        request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/x-www-form-urlencoded"));
        naManager->post(request, postData.toLocal8Bit());
    }





}

void LoginForm::requestFinished(QNetworkReply* reply) {
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
                //login fail
                QString result_msg = root_Obj.value("msg").toString();
                qDebug()<<"result_status:"<<result_status<<"msg:"<<result_msg;

                QMessageBox::warning(NULL, tr("Login Fail"), result_msg, QMessageBox::Ok );
            }
            if(result_status == 1){
                //login success
                QJsonObject result_data = root_Obj.value("data").toObject();
                QString result_token = result_data.value("token").toString();
                QString result_jurstr = result_data.value("jurstr").toString();

                login_UserName = ui->lineEdit_username->text();
                qDebug()<<"result_status:"<<result_status<<"result_token:"<<result_token<<"result_jurstr:"<<result_jurstr;

                QStringList jurstrlist = result_jurstr.split(",");
                qDebug()<<"jurstrlist"<<jurstrlist;

                emit loginsuccess(type,login_UserName,jurstrlist);
//                    this->hide();

//                    MainWindow *main = new MainWindow(ui->lineEdit_username->text(),this);
//                    main->show();
//                    this->destroy();


            }




            //qDebug()<<"result_status:"<<result_status<<"msg:"<<result_msg<<"result_token:"<<result_token<<"result_jurstr:"<<result_jurstr;
        }

    }
}
