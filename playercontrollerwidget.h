#ifndef PLAYERCONTROLLERWIDGET_H
#define PLAYERCONTROLLERWIDGET_H
#include <QMouseEvent>
#include <QWidget>
#include <QGraphicsScene>

class PlayerControllerWidget : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit PlayerControllerWidget(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override
    {
        // if(itemAt(event) == nullptr){

        // }
        QGraphicsScene::mousePressEvent(event);  // Propagate the event to child widgets
    }

};

#endif // PLAYERCONTROLLERWIDGET_H
