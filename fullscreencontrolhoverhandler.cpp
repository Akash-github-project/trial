#include "fullscreencontrolhoverhandler.h"

#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

FullScreenControlHoverHandler::FullScreenControlHoverHandler(QObject *parent)
    : QGraphicsRectItem(nullptr), controlledWidget(nullptr) {
    setAcceptHoverEvents(true);
}

void FullScreenControlHoverHandler::mousePressEvent(QGraphicsSceneMouseEvent* event) {
        QPointF postion = event->pos();
        QRectF containerRect = this->rect();
        emit onPostionClicked(postion,containerRect);
        QGraphicsRectItem::mousePressEvent(event);
 }

void FullScreenControlHoverHandler::animateFade(QGraphicsItem *item, bool fadeIn) {
/*    int duration = 1000;
    // Use QPropertyAnimation to animate the opacity of the QGraphicsItem directly
    QPropertyAnimation *animation = new QPropertyAnimation(item, "customOpacity");
    animation->setDuration(duration);

    if (fadeIn) {
        animation->setStartValue(0);   // Start invisible
        animation->setEndValue(0.86f);     // Fully visible
    } else {
        animation->setStartValue(0.86f);   // Start fully visible
        animation->setEndValue(0);     // Fade out to invisible
    }

    animation->start(QAbstractAnimation::DeleteWhenStopped)*/;
}

qreal FullScreenControlHoverHandler::customOpacity() const {
    // return m_customOpacity;
    return 0;
}

void FullScreenControlHoverHandler::setCustomOpacity(qreal opacity) {
/*    if (qFuzzyCompare(m_customOpacity, opacity))
        return;

    m_customOpacity = opacity;

    // Apply the opacity to the QGraphicsOpacityEffect
    if (auto *effect = dynamic_cast<QGraphicsOpacityEffect *>(graphicsEffect())) {
        effect->setOpacity(m_customOpacity);
    }

    // Optionally, trigger a repaint if necessary
    update()*/;
}
