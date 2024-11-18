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
    void onNoInternetDialogClose(bool closeWindow);
public:

    explicit NoInternetDialog(QWidget *parent = nullptr);
    explicit NoInternetDialog(QWidget *parent,QString message);
    ~NoInternetDialog();

private slots:
    void on_pushButton_clicked();

protected:
    void closeEvent(QCloseEvent * event) override {
        if(message.contains("No Internet connection")){
            emit onNoInternetDialogClose(false);
        }else {
            emit onNoInternetDialogClose(true);
        }
        QDialog::closeEvent(event);
    }

private:
    Ui::NoInternetDialog *ui;
    QString message = "No Internet connection";
};

#endif // NOINTERNETDIALOG_H
