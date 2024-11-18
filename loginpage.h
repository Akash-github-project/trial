#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QDialog>

namespace Ui {
class LoginPage;
}

class LoginPage : public QDialog
{
    Q_OBJECT

public:
    explicit LoginPage(QWidget *parent = nullptr);
    ~LoginPage();

signals:
    void otpRequestedForPhoneNumber(QString phoneNumber);

private slots:
    void LoginButtonPressed();
private:
    Ui::LoginPage *ui;
};

#endif // LOGINPAGE_H
