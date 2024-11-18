#include "mizushirushihandora.h"


MizuShirushiHandora::MizuShirushiHandora(QObject *parent) : QObject{parent} {}

MizuShirushiHandora::MizuShirushiHandora(QObject *parent,PlayerControllerWidget *sceen,QString userIdentifier,int rows, int columns,MizuConfig * config)
    : QObject{parent}
{
    this->config = config;
    PRIMARY_WATERMARK_TEXT = new CustomGraphicsTextItem(userIdentifier,config->lgConfig);
    PRIMARY_WATERMARK_TEXT->setOutlineColor(Qt::gray);
    PRIMARY_WATERMARK_TEXT->setTextColor(config->lgConfig->color);
    PRIMARY_WATERMARK_TEXT->setOutlineThickness(1);
    this->numCols = columns;
    this->numRows = rows;
    this->numParts = rows;
    this->phoneNumber = userIdentifier;
    this->sceen = sceen;
    textItems = QVector<QGraphicsTextItem*>();
    this->sceen->addItem(PRIMARY_WATERMARK_TEXT);

    for(int i = 0;i<config->smConfig->count_per_frame;i++){
        QGraphicsTextItem * textItemsParts = new QGraphicsTextItem("");
        QFont font("Arial", config->smConfig->font_size * 2, QFont::Bold);
        textItemsParts->setFont(font);
        textItemsParts->setDefaultTextColor(Qt::red);
        // int rotation = QRandomGenerator::global()->bounded(0,360);
        // textItemsParts->setRotation(rotation);
        textItemsParts->setPos(100, 100);
        textItemsParts->setOpacity(0.85f);
        textItemsParts->setPlainText("");
        textItems.append(textItemsParts);
        this->sceen->addItem(textItemsParts);
        this->sceen->update();
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
    int textWidth = PRIMARY_WATERMARK_TEXT->boundingRect().width();
    int posx = (sceneWidth / 2) - (textWidth / 2);
    int posy = (sceneHeight / 2) - (sceneHeight / 3);


    PRIMARY_WATERMARK_TEXT->setPos(posx,posy);
    PRIMARY_WATERMARK_TEXT->setRotation(config->lgConfig->angle);
    QFont font("Arial", config->lgConfig->font_size * 2, QFont::Bold);
    PRIMARY_WATERMARK_TEXT->setFont(font);
    PRIMARY_WATERMARK_TEXT->setTextColor(Qt::white);
    PRIMARY_WATERMARK_TEXT->setOpacity(config->lgConfig->transparency / 100.0f);
    // this->sceen->addItem(PRIMARY_WATERMARK_TEXT);
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


void MizuShirushiHandora::moveText(QGraphicsTextItem* textItem,int x,int y){
    //textItem->setPlainText(part); // Update text
    qDebug()<<"x: "<<x << "y: " <<y;
    textItem->setPos(x, y); // Reposition text items
}

void MizuShirushiHandora::updateWatermark(int sceenWidth,int sceenHeight){

    // int cellWidth = width / this->numCols;  // Width of each column
    // int cellHeight = height / this->numRows; // Height of each row

    QVector<QString> phoneParts;
    for(int i = 0;i < this->config->smConfig->count_per_frame;i++){
        std::string specialChars = "!@#$%^&*()";
        phoneParts.append(insertSpecialChars(this->phoneNumber.toStdString(),specialChars,5));
    }
    // Grid dimensions and spacing
    try {
        for(int i = 0;i < textItems.size();i++){
            textItems.value(i)->setDefaultTextColor(generateRandomColor());
            int opacity = QRandomGenerator::global()->bounded(config->smConfig->transparent_start,config->smConfig->transparent_end);
            if(opacity < 10){
                opacity = 10;
            }
            textItems.value(i)->setOpacity(opacity / 100.0f);
            textItems.value(i)->setPlainText(phoneParts[i]);
        }
        repositionGraphicsTextItems(sceenHeight,sceenWidth);
    }catch (std::exception e){
        qDebug()<<e.what() << " error";
    }
}

//new code
// Function to calculate distance between two points
double MizuShirushiHandora::distance(const QPointF &p1, const QPointF &p2) {
    return std::sqrt(std::pow(p1.x() - p2.x(), 2) + std::pow(p1.y() - p2.y(), 2));
}

// Function to check if a new point overlaps with existing points
bool MizuShirushiHandora::isOverlapping(const QPointF &newPoint, const QList<QPointF> &points, int itemSize, int padding) {
    for (const QPointF &point : points) {
        if (distance(newPoint, point) < itemSize + padding) {
            return true; // Items overlap
        }
    }
    return false; // No overlap
}

// Function to generate random points ensuring no overlap
QList<QPointF> MizuShirushiHandora::generateRandomPoints(int width, int height, int numPoints) {
    QList<QPointF> points;
    for (int i = 0; i < numPoints; ++i) {
        int regionWidth = width / 2;
        int regionHeight = height / 2;

        int regionX = (i % 2 == 0) ? 0 : regionWidth;      // Alternate left and right regions
        int regionY = (i / 2 % 2 == 0) ? 0 : regionHeight;  // Alternate top and bottom regions

        int randomX = QRandomGenerator::global()->bounded(regionX + 60, regionX + regionWidth - 100);
        int randomY = QRandomGenerator::global()->bounded(regionY + 60, regionY + regionHeight - 100);

        QPointF newPoint(randomX, randomY);
        points.append(newPoint);
    }
    return points;
}

// Usage Example in QGraphicsScene
void MizuShirushiHandora::repositionGraphicsTextItems(int sceneHeight,int sceneWeight) {
    int numPoints = textItems.size();
    // Generate random non-overlapping positions for each text item
    QList<QPointF> positions = generateRandomPoints(sceneWeight - 20, sceneHeight - 20, numPoints);
    postionPrimary(sceneWeight , sceneHeight);
    // Reposition each QGraphicsTextItem in the scene
    for (int i = 0; i < numPoints; ++i) {
        textItems.value(i)->setPos(positions[i]);
    }
}
