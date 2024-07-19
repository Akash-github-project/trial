#include "loginpage.h"
#include "ui_loginpage.h"

LoginPage::LoginPage(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginPage)
{
    ui->setupUi(this);
    ui->LoginButton->updateColorScene("#F58F30","#EE821F",0);
    connect(ui->LoginButton,&ClickableQFrame::clicked,this,&LoginPage::LoginButtonPressed);
}

void LoginPage::LoginButtonPressed(){
   //validate
    //
    emit otpRequestedForPhoneNumber("29292929");
}
LoginPage::~LoginPage()
{
    delete ui;
}
