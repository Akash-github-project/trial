#ifndef CUSTOMGESTUREWIDGET_H
#define CUSTOMGESTUREWIDGET_H

#include <QWidget>

class CustomGestureWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CustomGestureWidget(QWidget *parent = nullptr);
    void mouseDoubleClickEvent(QMouseEvent *event) override;


signals:
    void makeFullScreen();
};

#endif // CUSTOMGESTUREWIDGET_H
