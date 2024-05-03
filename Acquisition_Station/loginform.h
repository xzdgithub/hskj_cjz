#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QDialog>
//#include<mainwindow.h>
#include <QtNetwork>
#include<QJsonArray>
#include<QJsonDocument>
#include<QJsonObject>
#include<QNetworkRequest>
#include<QMessageBox>
#include<config.h>
#include<QRegExpValidator>
namespace Ui {
class LoginForm;
}

class LoginForm : public QDialog
{
    Q_OBJECT

public:
    explicit LoginForm(int type,QWidget *parent = nullptr);

    ~LoginForm();
signals:
    void loginsuccess(int type,QString login_UserName,QStringList jurstrlist);
private slots:
    void on_pushButton_clicked();

    void requestFinished(QNetworkReply *reply);

private:
    Ui::LoginForm *ui;

    QNetworkRequest request;
    QNetworkAccessManager* naManager;
    int type = -1;
    QString login_UserName;
};

#endif // LOGINFORM_H
