#include "registerdeviceconfirmation.h"
#include "ui_registerdeviceconfirmation.h"

RegisterDeviceConfirmation::RegisterDeviceConfirmation(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDeviceConfirmation)
{
    ui->setupUi(this);
    ui->pushButton_yes_register->updateColorScene("#F58F30","#EE821F",0);
}

RegisterDeviceConfirmation::~RegisterDeviceConfirmation()
{
    delete ui;
}
