#include "getsettingdialog.h"
#include "ui_getsettingdialog.h"

getSettingDialog::getSettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::getSettingDialog)
{
    ui->setupUi(this);
    qDebug()<<"init getSettingDialog";
    this->setWindowTitle("Waiting");
    changetounfinsh();
    this->setWindowModality(Qt::ApplicationModal);
    setFixedSize(this->width(),this->height());
    ui->label->setWordWrap(true);
    starttimer = new QTimer(this);
    connect(starttimer, SIGNAL(timeout()), this, SLOT(getsettingfail()));
    starttimer->start(7000);

}

getSettingDialog::~getSettingDialog()
{
    qDebug()<<"~getSettingDialog";
    delete ui;
}

void getSettingDialog::getfinish()
{
    ui->btn_OK->setEnabled(true);
    ui->label->setText(tr("get setting finish,click ok button to exit the dialog!"));
}

void getSettingDialog::on_btn_OK_clicked()
{
    this->accept();
    changetounfinsh();

}

void getSettingDialog::closeEvent(QCloseEvent *event)
{
    qDebug()<<"close event";
    event->ignore();
}

void getSettingDialog::changetounfinsh()
{

    ui->btn_OK->setEnabled(false);
    ui->label->setText(tr("now is get all settings,please wait!"));
}
void getSettingDialog::getsettingfail()
{
    starttimer->stop();
    ui->btn_OK->setEnabled(true);
    ui->label->setText(tr("get settings fail,click button to exit this dialog"));
}
