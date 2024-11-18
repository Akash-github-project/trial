#ifndef LOGINOTPSCREEN_H
#define LOGINOTPSCREEN_H

#include <QDialog>

namespace Ui {
class LoginOtpScreen;
}

class LoginOtpScreen : public QDialog
{
    Q_OBJECT

public:
    explicit LoginOtpScreen(QWidget *parent = nullptr);
    ~LoginOtpScreen();

signals:
    void onOtpSubmitedSuccess(QString phoneNumber);

private slots:
    void onOtpValidateInitiated();
private:
    Ui::LoginOtpScreen *ui;
};

#endif // LOGINOTPSCREEN_H
