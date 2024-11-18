#ifndef MAINHOMESCREEN_H
#define MAINHOMESCREEN_H

#include <QMainWindow>

namespace Ui {
class MainHomeScreen;
}

class MainHomeScreen : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainHomeScreen(QWidget *parent = nullptr);
    ~MainHomeScreen();

private:
    Ui::MainHomeScreen *ui;
};

#endif // MAINHOMESCREEN_H
