#include "onboardingwrapper.h"
#include "ui_onboardingwrapper.h"

OnboardingWrapper::OnboardingWrapper(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::OnboardingWrapper)
{
    ui->setupUi(this);
    login = new LoginPage(this);
    loginOtp = new LoginOtpScreen(this);
    registerDevice = new RegisterDeviceConfirmation(this);

    ui->stackedWidget->addWidget(login);
    ui->stackedWidget->addWidget(loginOtp);
    ui->stackedWidget->setCurrentIndex(0);

    connect(login,&LoginPage::otpRequestedForPhoneNumber,this,OnboardingWrapper::otpRequested);
    connect(loginOtp,&LoginOtpScreen::onOtpSubmitedSuccess,this,&OnboardingWrapper::otpRequestAccepted);
}

void OnboardingWrapper::otpRequested(QString phoneNumber){
    ui->stackedWidget->setCurrentIndex(1);
}

void OnboardingWrapper::otpRequestAccepted(QString phoneNumber){
    ui->stackedWidget->addWidget(registerDevice);
    ui->stackedWidget->setCurrentIndex(2);
    qDebug()<<phoneNumber;
}

OnboardingWrapper::~OnboardingWrapper()
{
    delete ui;
}
