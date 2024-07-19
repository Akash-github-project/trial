#include "mainhomescreen.h"
#include "ui_mainhomescreen.h"

MainHomeScreen::MainHomeScreen(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainHomeScreen)
{
    ui->setupUi(this);
}

MainHomeScreen::~MainHomeScreen()
{
    delete ui;
}
