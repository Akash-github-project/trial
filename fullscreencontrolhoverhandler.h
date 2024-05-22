#ifndef FULLSCREENCONTROLHOVERHANDLER_H
#define FULLSCREENCONTROLHOVERHANDLER_H

#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsProxyWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QEvent>

class FullScreenControlHoverHandler : public QGraphicsRectItem
{
public:
    explicit FullScreenControlHoverHandler(QObject *parent = nullptr);
    FullScreenControlHoverHandler(QGraphicsWidget* widget, const QRectF& rect) :  QGraphicsRectItem(rect), controlledWidget(widget){
        controlledWidget->hide();
    }

signals:

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override {
        controlledWidget->show();
        controlledWidget->setOpacity(0.5f);
        QGraphicsRectItem::hoverEnterEvent(event);
    }

    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override {
        controlledWidget->hide();
        QGraphicsRectItem::hoverLeaveEvent(event);
    }


private:
    QGraphicsWidget* controlledWidget;
};

#endif // FULLSCREENCONTROLHOVERHANDLER_H

