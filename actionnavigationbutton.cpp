#include "actionnavigationbutton.h"

ActionNavigationButton::ActionNavigationButton() {

}

// ActionNavigationButton::paintEvent(QPaintEvent *event) override {
//     QPainter painter(this);
//     painter.setRenderHint(QPainter::Antialiasing);

//     // Button dimensions and corner radius
//     int width = this->width();
//     int height = this->height();
//     int cornerRadius = 10;

//     // Calculate arrow area and button background area
//     int arrowWidth = width * 0.2; // Adjust arrow width as needed (20% of button width)
//     int buttonBackgroundWidth = width - arrowWidth;

//     // Create rounded rectangle paths for button and arrow areas
//     QPainterPath buttonPath;
//     buttonPath.addRoundedRect(0, 0, buttonBackgroundWidth, height, cornerRadius, cornerRadius);

//     QPainterPath arrowPath;
//     arrowPath.addRoundedRect(buttonBackgroundWidth, 0, arrowWidth, height, cornerRadius, cornerRadius);

//     // Set clip paths for separate drawing
//     painter.setClipPath(buttonPath);
//     painter.fillRect(0, 0, buttonBackgroundWidth, height, Qt::darkGray); // Darker background

//     painter.setClipPath(arrowPath);
//     painter.fillRect(buttonBackgroundWidth, 0, arrowWidth, height, Qt::lightGray); // Button background color

//     // Draw arrow icon (replace with your image loading and drawing logic)
//     QPixmap arrowIcon(":/arrow-next"); // Assuming resource file named "arrow.png"
//     int iconWidth = arrowIcon.width();
//     int iconHeight = arrowIcon.height();
//     painter.drawPixmap(buttonBackgroundWidth + (arrowWidth - iconWidth) / 2, (height - iconHeight) / 2, iconWidth, iconHeight);

//     // Draw button text (optional)
//     QPushButton::paintEvent(event);
// }
