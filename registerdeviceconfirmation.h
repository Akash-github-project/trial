#ifndef REGISTERDEVICECONFIRMATION_H
#define REGISTERDEVICECONFIRMATION_H

#include <QDialog>

namespace Ui {
class RegisterDeviceConfirmation;
}

class RegisterDeviceConfirmation : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDeviceConfirmation(QWidget *parent = nullptr);
    ~RegisterDeviceConfirmation();

private:
    Ui::RegisterDeviceConfirmation *ui;
};

#endif // REGISTERDEVICECONFIRMATION_H
