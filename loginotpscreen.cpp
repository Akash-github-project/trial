#include "loginotpscreen.h"
#include "ui_loginotpscreen.h"

LoginOtpScreen::LoginOtpScreen(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginOtpScreen)
{
    ui->setupUi(this);
    ui->LoginButton->updateColorScene("#F58F30","#EE821F",0);
    ui->ChangePhoneNumber->updateColorScene("#EEEEEE","#E6E6E6",0);
    ui->GoBackButton->updateColorScene("transparent","#E6E6E6",0);


    connect(ui->LoginButton,&ClickableQFrame::clicked,this,&LoginOtpScreen::onOtpValidateInitiated);
}

void LoginOtpScreen::onOtpValidateInitiated(){
    emit onOtpSubmitedSuccess("7878787878");
}

LoginOtpScreen::~LoginOtpScreen()
{
    delete ui;
}
