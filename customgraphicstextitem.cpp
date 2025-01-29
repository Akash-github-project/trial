#include "customgraphicstextitem.h"
#include <QPainter>
#include <QPainterPath>

CustomGraphicsTextItem::CustomGraphicsTextItem(const QString &text,LgMizu * config,QWidget *parent)
    : QLabel(text, parent),
    outlineColor(QColor(config->color)),
    textColor(QColor(config->color)),
    outlineThickness(10),
    m_rotationAngle(0),
    config(config) {
    this->config = config;
    setAlignment(Qt::AlignCenter); // Center the text for better rotation handling
}

void CustomGraphicsTextItem::setOutlineColor(const QColor &color) {
    outlineColor = color;
    update();  // Trigger repaint
}

void CustomGraphicsTextItem::setTextColor(const QColor &color) {
    textColor = color;
    update();  // Trigger repaint
}

void CustomGraphicsTextItem::setOutlineThickness(int thickness) {
    outlineThickness = thickness;
    update();  // Trigger repaint
}

// void CustomGraphicsTextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
//     Q_UNUSED(option);
//     Q_UNUSED(widget);

//     // Enable anti-aliasing for smooth edges
//     painter->setRenderHint(QPainter::Antialiasing);

//     // Set up font
//     QFont font = QFont("Arial",config->font_size * 3, QFont::Bold);
//     painter->setFont(font);

//     // Create a shadow color (a semi-transparent black)
//     QColor shadowColor = QColor(0, 0, 0, 160);  // Adjust alpha for transparency

//     // Draw shadow text slightly offset
//     painter->setPen(shadowColor);
//     painter->setBrush(shadowColor);
//     painter->drawText(2, 2, toPlainText());  // Offset shadow by (2, 2)

//     // Draw the main text
//     painter->setPen(QColor(config->color));  // Set text color
//     painter->setBrush(QColor(config->color));  // Fill the text
//     painter->drawText(0, 0, toPlainText());  // Draw main text at the original position
// }




void CustomGraphicsTextItem::setRotationAngle(qreal angle) {
    m_rotationAngle = angle;
    update(); // Trigger a repaint
}

void CustomGraphicsTextItem::paintEvent(QPaintEvent *event)  {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Prepare rotation transformation
    QPoint center = rect().center(); // Center of the QLabel
    painter.translate(center);
    painter.rotate(m_rotationAngle);
    painter.translate(-center);

    // Calculate text bounding box
    QFontMetrics metrics(font());
    QRect textRect = metrics.boundingRect(text());
    textRect.moveCenter(rect().center()); // Center text in the label

    // Draw outline (if thickness > 0)
    if (outlineThickness > 0) {
        QPen outlinePen(outlineColor, outlineThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(outlinePen);
        painter.setBrush(Qt::NoBrush);

        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                if (dx != 0 || dy != 0) {
                    painter.drawText(textRect.translated(dx, dy), Qt::AlignCenter, text());
                }
            }
        }
    }

    // Draw the text
    QPen textPen(QColor(this->config->color));
    painter.setPen(textPen);
}
