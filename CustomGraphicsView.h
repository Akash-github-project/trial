#ifndef CUSTOMGRAPHICSVIEW_H
#define CUSTOMGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>

/**
 * @brief The CustomGraphicsView class
 *
 * A custom subclass of QGraphicsView that disables mouse scrolling by ignoring wheel events.
 */
class CustomGraphicsView : public QGraphicsView {
    Q_OBJECT

public:
    /**
     * @brief Constructor for CustomGraphicsView
     *
     * @param parent The parent widget, if any.
     */
    explicit CustomGraphicsView(QWidget* parent = nullptr);

protected:
    /**
     * @brief Override of the QGraphicsView wheel event handler
     *
     * This function ignores all wheel events, effectively disabling mouse scrolling.
     *
     * @param event The wheel event to be ignored.
     */
    void wheelEvent(QWheelEvent* event) override {
        // Ignore the wheel event to disable scrolling
        event->ignore();
    }
};

#endif // CUSTOMGRAPHICSVIEW_H
