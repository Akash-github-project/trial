#include "warningdialog.h"
#include "ui_warningdialog.h"

WarningDialog::WarningDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::WarningDialog)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);
}

WarningDialog::~WarningDialog()
{
    delete ui;
}

void WarningDialog::on_pushButton_clicked()
{
    emit closed();
    //this->close();
}

