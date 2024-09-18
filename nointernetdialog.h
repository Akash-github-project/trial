#ifndef NOINTERNETDIALOG_H
#define NOINTERNETDIALOG_H

#include <QDialog>

namespace Ui {
class NoInternetDialog;
}

class NoInternetDialog : public QDialog
{
    Q_OBJECT

signals:
    void onNoInternetDialogClose();
public:
    explicit NoInternetDialog(QWidget *parent = nullptr);
    ~NoInternetDialog();

private slots:
    void on_pushButton_clicked();

protected:
    void closeEvent(QCloseEvent * event) override {
        emit onNoInternetDialogClose();
        QDialog::closeEvent(event);
    }

private:
    Ui::NoInternetDialog *ui;
};

#endif // NOINTERNETDIALOG_H
