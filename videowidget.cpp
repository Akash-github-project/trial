#include "videowidget.h"

VideoWidget::VideoWidget(QWidget* parent)
    : QWidget(parent)
{
    // Optional: Enable attribute for faster painting
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    setAttribute(Qt::WA_NoSystemBackground, true);
}

VideoWidget::~VideoWidget()
{
    // Destructor logic if needed
}

void VideoWidget::renderFrame(const QImage& frame)
{
    QMutexLocker locker(&frameMutex);
    if (frame.isNull()) {
        qDebug() << "Received a null frame.";
    }
    currentImage = frame.copy(); // Copy to ensure thread safety
    locker.unlock();
    update(); // Trigger paintEvent
}

void VideoWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    // Fill background with black to prevent artifacts
    painter.fillRect(rect(), Qt::black);

    // Lock the mutex to safely access the current image
    QMutexLocker locker(&frameMutex);

    if (!currentImage.isNull()) {
        // Calculate target rectangle maintaining aspect ratio
        //QRect targetRect = QRect(0,0,this->width(),this->height());
        QRect targetRect = calculateTargetRect();

        // Set render hints for better quality
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter.setRenderHint(QPainter::Antialiasing, false);

        // Draw the image within the target rectangle
        painter.drawImage(targetRect, currentImage);
    }
}

QRect VideoWidget::calculateTargetRect() const
{
    if (currentImage.isNull()) {
        return QRect();
    }

    // Original image size
    QSize imageSize = currentImage.size();

    // Widget size
    QSize widgetSize = size();

    // Calculate scaled size while maintaining aspect ratio
    QSize scaledSize = imageSize.scaled(widgetSize, Qt::KeepAspectRatio);

    // Calculate top-left point to center the image
    QPoint topLeft((widgetSize.width() - scaledSize.width()) / 2,
                   (widgetSize.height() - scaledSize.height()) / 2);

    return QRect(topLeft, scaledSize);
}
