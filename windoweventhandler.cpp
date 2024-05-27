#include "windoweventhandler.h"

WindowEventHandler::WindowEventHandler(QObject *parent)
    : QObject{parent}
{}

WindowEventHandler::WindowEventHandler(QObject *parent ,QMainWindow *window){
    this->window = window;
};

void WindowEventHandler::setWindowFullScreen(){
     window->setWindowState(Qt::WindowFullScreen);
     emit onFullScreenStateChanged(true);
}

void WindowEventHandler::resetWindowState(){
     window->setWindowState(Qt::WindowNoState);
     emit onFullScreenStateChanged(false);
}
