#ifndef CUSTOMCONTROLSWIDGET_H
#define CUSTOMCONTROLSWIDGET_H

#include <QWidget>

class CustomControlsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CustomControlsWidget(QWidget *parent = nullptr);


protected:
    void mousePressEvent(QMouseEvent *event) override
    {
        // if(itemAt(event) == nullptr){

        // }
        QWidget::mousePressEvent(event);  // Propagate the event to child widgets
    }

signals:
};

#endif // CUSTOMCONTROLSWIDGET_H
