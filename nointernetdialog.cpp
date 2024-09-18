#include "nointernetdialog.h"
#include "ui_nointernetdialog.h"

NoInternetDialog::NoInternetDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::NoInternetDialog)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);
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

