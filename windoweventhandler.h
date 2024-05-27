#ifndef WINDOWEVENTHANDLER_H
#define WINDOWEVENTHANDLER_H

#include <QMainWindow>
#include <QObject>

class WindowEventHandler : public QObject
{
    Q_OBJECT

private:
    QMainWindow *window;
public:

    explicit WindowEventHandler(QObject *parent = nullptr);
    WindowEventHandler(QObject *parent ,QMainWindow *window);
    void setWindowFullScreen();
    void resetWindowState();
signals:
    void onFullScreenStateChanged(bool isFullScreen);
};

#endif // WINDOWEVENTHANDLER_H
