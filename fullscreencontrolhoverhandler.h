#ifndef FULLSCREENCONTROLHOVERHANDLER_H
#define FULLSCREENCONTROLHOVERHANDLER_H

#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsProxyWidget>
#include <QGraphicsSceneMouseEvent>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QEvent>
#include <QObject>


class FullScreenControlHoverHandler :public QObject, public QGraphicsRectItem
{
   Q_OBJECT
   Q_PROPERTY(qreal customOpacity READ customOpacity WRITE setCustomOpacity)
public:
    explicit FullScreenControlHoverHandler(QObject *parent = nullptr);
    FullScreenControlHoverHandler(QGraphicsWidget* widget, const QRectF& rect)
        : QObject(nullptr),
        QGraphicsRectItem(rect),
        controlledWidget(widget) {

        controlledWidget->hide();
    }
    qreal customOpacity() const;
    void animateFade(QGraphicsItem *item, bool fadeIn);
    void setCustomOpacity(qreal opacity);
    qreal m_customOpacity;  // The custom opacity value
signals:
    void onPostionClicked(QPointF clickedPoint,QRectF parentRect);
    void doubleClickedScreen();

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override {
        controlledWidget->show();
        //controlledWidget->setOpacity(0.86f);
        //animateFade(controlledWidget,true);
        QGraphicsRectItem::hoverEnterEvent(event);
    }

    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override {
        //animateFade(controlledWidget,false);
        controlledWidget->hide();
        QGraphicsRectItem::hoverLeaveEvent(event);
    }

    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override {
        emit doubleClickedScreen();
        event->accept();
    }

private:
    QGraphicsWidget* controlledWidget;
};

#endif // FULLSCREENCONTROLHOVERHANDLER_H

