#include "mainwindow.h"

#include <QApplication>
#include <QTimer>
int main(int argc, char *argv[])
{
    qDebug()<<"hello there";
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
