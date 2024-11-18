#ifndef ONBOARDINGWRAPPER_H
#define ONBOARDINGWRAPPER_H

#include <QDialog>
#include "loginpage.h"
#include "loginotpscreen.h"
#include "registerdeviceconfirmation.h"

namespace Ui {
class OnboardingWrapper;
}

class OnboardingWrapper : public QDialog
{
    Q_OBJECT

public:
    explicit OnboardingWrapper(QWidget *parent = nullptr);
    ~OnboardingWrapper();

private slots:
    void otpRequested(QString phoneNumber);
    void otpRequestAccepted(QString phoneNumber);
private:
    Ui::OnboardingWrapper *ui;
    LoginPage * login;
    LoginOtpScreen * loginOtp;
    RegisterDeviceConfirmation * registerDevice;
};

#endif // ONBOARDINGWRAPPER_H
