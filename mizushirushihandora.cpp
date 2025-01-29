#include "mizushirushihandora.h"

#include <QGraphicsOpacityEffect>
#include <QtMath>


MizuShirushiHandora::MizuShirushiHandora(QObject *parent) : QObject{parent} {}

MizuShirushiHandora::MizuShirushiHandora(QObject *parent,QWidget *sceen,QString userIdentifier,int rows, int columns,MizuConfig * config)
    : QObject{parent}
{
    this->config = config;
    PRIMARY_WATERMARK_TEXT = new CustomGraphicsTextItem(userIdentifier,config->lgConfig,sceen);
    PRIMARY_WATERMARK_TEXT->setTextColor(config->lgConfig->color);
    PRIMARY_WATERMARK_TEXT->setOutlineThickness(1);
    this->numCols = columns;
    this->numRows = rows;
    this->numParts = rows;
    this->phoneNumber = userIdentifier;
    this->sceen = sceen;
    textItems = QVector<QLabel*>();
    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(this);
    opacityEffect->setOpacity(0.85f); // 0.0 = fully transparent, 1.0 = fully opaque
    //this->sceen->addItem(PRIMARY_WATERMARK_TEXT);

    for(int i = 0;i<config->smConfig->count_per_frame;i++){
        QLabel * textItemsParts = new QLabel(sceen);
        QFont font("Arial", config->smConfig->font_size * 2, QFont::Bold);
        textItemsParts->setFont(font);
        textItemsParts->setText("                                       ");
        textItems.append(textItemsParts);
    }
}

QColor MizuShirushiHandora::generateRandomColor() {
    if(config->smConfig->colors.isEmpty()){
        int red = QRandomGenerator::global()->bounded(256);   // Random value between 0 and 255
        int green = QRandomGenerator::global()->bounded(256); // Random value between 0 and 255
        int blue = QRandomGenerator::global()->bounded(256);  // Random value between 0 and 255
        return QColor(red,green,blue);
    }


    int indexOfColor = QRandomGenerator::global()->bounded(config->smConfig->colors.count());
    return QColor(config->smConfig->colors[indexOfColor]);
}

void MizuShirushiHandora::postionPrimary(qreal sceneWidth, qreal sceneHeight){
    // Fixed initial dimensions of the watermark
    int textWidth = 421; // Replace with the actual base width of the text
    int textHeight = 207; // Replace with the actual base height of the text

    // Calculate rotated bounding box dimensions
    double radians = qDegreesToRadians(config->lgConfig->angle);
    double rotatedWidth = textWidth * fabs(cos(radians)) + textHeight * fabs(sin(radians));
    double rotatedHeight = textWidth * fabs(sin(radians)) + textHeight * fabs(cos(radians));

    // Calculate centered position
    int posx = (sceneWidth - rotatedWidth) / 2;
    int posy = (sceneHeight - rotatedHeight) / 2;

    // Set consistent font and appearance
    QFont font("Arial", config->lgConfig->font_size * 2, QFont::Bold);
    PRIMARY_WATERMARK_TEXT->setFont(font);
    PRIMARY_WATERMARK_TEXT->setTextColor(QColor(config->lgConfig->color));

    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(this);
    opacityEffect->setOpacity(config->lgConfig->transparency / 100.0f); // 0.0 = fully transparent, 1.0 = fully opaque
    PRIMARY_WATERMARK_TEXT->setGraphicsEffect(opacityEffect);

    // Apply rotation angle
    PRIMARY_WATERMARK_TEXT->setRotationAngle(config->lgConfig->angle);

    // Set geometry with fixed dimensions
    PRIMARY_WATERMARK_TEXT->setGeometry(posx, posy, rotatedWidth, rotatedHeight);

    // Debug output for verification
    qDebug() << "Centered Geometry - x:" << posx << "y:" << posy
             << "width:" << rotatedWidth << "height:" << rotatedHeight;
}

QString MizuShirushiHandora::generateRandomCharacters(int length) {
    QString chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789@#$&|";
    QString randomString;

    int index = QRandomGenerator::global()->bounded(0,  chars.length() - length);
    randomString += chars.mid(index,length);

    return randomString;
}


QVector<QString>  MizuShirushiHandora::splitPhoneNumber(const QString &phoneNumber, int numParts) {
    QVector<QString> parts;
    int length = phoneNumber.length();
    int partLength = length / numParts;
    int remainder = length % numParts;
    int start = 0;

    for (int i = 0; i < numParts; ++i) {
        int currentPartLength = partLength + (i < remainder ? 1 : 0);
        parts.append(phoneNumber.mid(start, currentPartLength));
        start += currentPartLength;
    }
    return parts;
}

QString MizuShirushiHandora::insertSpecialChars(const std::string& input, const std::string& specialChars, int numInserts) {
    std::string result = input;
    srand(time(0));  // Seed the random number generator with the current time

    for (int i = 0; i < numInserts; ++i) {
        // Generate random index for special character insertion
        int pos = rand() % (result.length() + 1);

        // Generate random index for selecting a special character from specialChars
        char specialChar = specialChars[rand() % specialChars.length()];

        // Insert the special character at the random position
        result.insert(pos, 1, specialChar);
    }

    return QString::fromStdString(result);
}


void MizuShirushiHandora::moveText(QLabel* textItem,int x,int y){
    //textItem->setPlainText(part); // Update text
    qDebug()<<"x: "<<x << "y: " <<y;
    textItem->setGeometry(QRect(x,y,textItem->size().width() * 3,textItem->size().height()));
    //textItem->setPos(x, y); // Reposition text items
}

void MizuShirushiHandora::updateWatermark(int sceenWidth,int sceenHeight){

    // int cellWidth = width / this->numCols;  // Width of each column
    // int cellHeight = height / this->numRows; // Height of each row
    int opacity = QRandomGenerator::global()->bounded(config->smConfig->transparent_start,config->smConfig->transparent_end);
    if(opacity < 10){
        opacity = 10;
    }
    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(this);
    opacityEffect->setOpacity(opacity / 100.0f); // 0.0 = fully transparent, 1.0 = fully opaque

    QVector<QString> phoneParts;
    for(int i = 0;i < this->config->smConfig->count_per_frame;i++){
        std::string specialChars = "!@#$%^&*()";
        phoneParts.append(insertSpecialChars(this->phoneNumber.toStdString(),specialChars,5));
    }
    // Grid dimensions and spacing
    try {
        for(int i = 0;i < textItems.size();i++){
            //textItems.value(i)->setDefaultTextColor(generateRandomColor());
            //textItems.value(i)->setStyleSheet("color:" + generateRandomColor().toRgb());
            textItems.value(i)->setGraphicsEffect(opacityEffect);
            //textItems.value(i)->setOpacity();
            textItems.value(i)->setText(phoneParts[i]);
            // textItems.value(i)->setStyleSheet("color:blue;");
            textItems.value(i)->setStyleSheet("color:" + generateRandomColor().name() + ";");
        }
        repositionGraphicsTextItems(sceenHeight,sceenWidth);
    }catch (std::exception e){
        qDebug()<<e.what() << " error";
    }
}

//new code
// Function to calculate distance between two points
double MizuShirushiHandora::distance(const QPoint &p1, const QPoint &p2) {
    return std::sqrt(std::pow(p1.x() - p2.x(), 2) + std::pow(p1.y() - p2.y(), 2));
}

// Function to check if a new point overlaps with existing points
bool MizuShirushiHandora::isOverlapping(const QPoint &newPoint, const QList<QPoint> &points, int itemSize, int padding) {
    for (const QPoint &point : points) {
        if (distance(newPoint, point) < itemSize + padding) {
            return true; // Items overlap
        }
    }
    return false; // No overlap
}

// Function to generate random points ensuring no overlap
QList<QPoint> MizuShirushiHandora::generateRandomPoints(int width, int height, int numPoints) {
    QList<QPoint> points;
    for (int i = 0; i < numPoints; ++i) {
        int regionWidth = width / 2;
        int regionHeight = height / 2;

        int regionX = (i % 2 == 0) ? 0 : regionWidth;      // Alternate left and right regions
        int regionY = (i / 2 % 2 == 0) ? 0 : regionHeight;  // Alternate top and bottom regions

        int randomX = QRandomGenerator::global()->bounded(regionX + 60, regionX + regionWidth - 100);
        int randomY = QRandomGenerator::global()->bounded(regionY + 60, regionY + regionHeight - 100);

        QPoint newPoint(randomX, randomY);
        points.append(newPoint);
    }
    return points;
}

// Usage Example in QGraphicsScene
void MizuShirushiHandora::repositionGraphicsTextItems(int sceneHeight,int sceneWeight) {
    int numPoints = textItems.size();
    // Generate random non-overlapping positions for each text item
    QList<QPoint> positions = generateRandomPoints(sceneWeight - 20, sceneHeight - 20, numPoints);
    postionPrimary(sceneWeight , sceneHeight);
    // Reposition each QGraphicsTextItem in the scene
    for (int i = 0; i < numPoints; ++i) {
        textItems.value(i)->setGeometry(positions[i].x(),positions[i].y(),textItems.value(i)->width() ,textItems.value(i)->height());
    }
}
