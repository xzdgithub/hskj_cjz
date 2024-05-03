#include "insertzfydialog.h"
#include "ui_insertzfydialog.h"

InsertZFYDialog::InsertZFYDialog(int zfynum,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InsertZFYDialog)
{
    ui->setupUi(this);
    qDebug()<<"init InsertZFYDialog";
    this->zfynum = zfynum;
    this->setWindowTitle(tr("Connecting"));
    setWindowFlags(Qt::WindowCloseButtonHint);
    setFixedSize(this->width(),this->height());
    changetounfinsh();
    ui->label_msg->setWordWrap(true);
    starttimer = new QTimer(this);
    connecttime = 0;
    connect(starttimer, SIGNAL(timeout()), this, SLOT(connectfail()));
    starttimer->start(12000);
}

InsertZFYDialog::~InsertZFYDialog()
{
    delete ui;
    qDebug()<<"~InsertZFYDialog";
}

void InsertZFYDialog::on_btn_Ok_clicked()
{
    this->accept();
    changetounfinsh();
}
void InsertZFYDialog::connectfinish()
{
    starttimer->stop();
    ui->btn_Ok->setEnabled(true);
    ui->label_msg->setText(tr("connect finish,click button to exit this dialog"));
}

void InsertZFYDialog::connectfail()
{

    starttimer->stop();
    connecttime += 1;
    if(connecttime<=2){
        ui->label_msg->setText(tr("connect ZFY fail,now is reconnect ")+QString::number(connecttime)+tr(" time"));
        starttimer->start();
        emit sigreconnect(zfynum);
    }else {
        ui->btn_Ok->setEnabled(true);
        ui->label_msg->setText(tr("connect ZFY fail,please delete it and try again"));
    }
    //ui->btn_Ok->setEnabled(true);
    //ui->label_msg->setText(tr("connect ZFY fail,please delete it and try again"));
}
void InsertZFYDialog::closeEvent(QCloseEvent *event)
{
    qDebug()<<"close event";
    event->ignore();
}

void InsertZFYDialog::changetounfinsh()
{

    ui->btn_Ok->setEnabled(false);
    ui->label_msg->setText(tr("now is connecting the ZFY,please don not insert another ZFY before connect finish!"));
}
