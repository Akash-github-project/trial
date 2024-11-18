#include "nointernetdialog.h"
#include "ui_nointernetdialog.h"

NoInternetDialog::NoInternetDialog(QWidget *parent){ }

NoInternetDialog::NoInternetDialog(QWidget *parent,QString message)
    : QDialog(parent)
    , ui(new Ui::NoInternetDialog)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);
    ui->message->setText(message);
    this->message = message;
    if(!message.contains("No Internet connection")){
        ui->pushButton->setText("Close");
    }else {
        ui->pushButton->setText("Retry");
    }
}

NoInternetDialog::~NoInternetDialog()
{
    delete ui;
}

void NoInternetDialog::on_pushButton_clicked()
{
    close();
   //emit onNoInternetDialogClose();
}

