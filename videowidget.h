#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>
#include <QImage>
#include <QMutex>
#include <QMutexLocker>
#include <QPainter>
#include <QDebug>

class VideoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VideoWidget(QWidget* parent = nullptr);
    ~VideoWidget();

public slots:
    /**
     * Receives a new video frame to render.
     * This slot should be connected using Qt::QueuedConnection if called from another thread.
     *
     * @param frame The QImage representing the video frame.
     */
    void renderFrame(const QImage& frame);

protected:
    /**
     * Overridden paint event to draw the current video frame.
     *
     * @param event The paint event.
     */
    void paintEvent(QPaintEvent* event) override;

private:
    QImage currentFrame;   // Current video frame
    QMutex frameMutex;     // Mutex to protect access to currentFrame and currentPixmap
    QImage currentImage;
    /**
     * Calculates the target rectangle to maintain aspect ratio.
     *
     * @return QRect The target rectangle within the widget.
     */
    QRect calculateTargetRect() const;
};


#endif // VIDEOWIDGET_H
