#ifndef MIZUSHIRUSHIHANDORA_H
#define MIZUSHIRUSHIHANDORA_H

#include <QObject>
#include <qgraphicsitem.h>
#include <QGraphicsView>
#include <QGraphicsTextItem>
#include <QString>
#include <QRandomGenerator>
#include <QVector>
#include <iostream>
#include <string>
#include <cstdlib>  // For rand() and srand()
#include <ctime>    // For time()
#include <QLabel>
#include "playercontrollerwidget.h"
#include "customgraphicstextitem.h"
#define PRIMARY_WATERMARK_TEXT only_pain

struct XYforMizu {
    qreal x;
    qreal y;
};

struct SmMizu{
    int font_size;
    int transparent_start;
    int transparent_end;
    int count_per_frame;
    int interval;
    QStringList colors;
};

struct LgMizu {
    int font_size;
    int transparency;
    int width_percent;
    QString color;
    int angle;
};

struct FlashMizu {
    QList<int> transparency;
    QString color;
    int chn_duration;
};

struct DFlashMizu {
    QList<int> transparency;
    QString color;
    int chn_duration;
};


struct MizuConfig {
    int log_activity_interval;
    int resume_at_seconds;
    SmMizu * smConfig;
    LgMizu * lgConfig;
    FlashMizu * flConfig;
    DFlashMizu * dflConfig;
};

class CustomGraphicsTextItem;
class MizuShirushiHandora : public QObject
{
    Q_OBJECT
public:
    explicit MizuShirushiHandora(QObject *parent = nullptr);
    MizuShirushiHandora(QObject *parent, QWidget *sceen, QString userIdentifier, int rows, int columns,MizuConfig * config);

    CustomGraphicsTextItem * PRIMARY_WATERMARK_TEXT;
    MizuConfig * config;
    QVector<QLabel*> textItems;
    QWidget *sceen;
    QString phoneNumber = nullptr;
    int numParts;
    int  numRows;
    int numCols;

public:
    void updateWatermark(int sceenWidth,int sceenHeight);
    QString generateRandomCharacters(int length);
    QVector<QString> splitPhoneNumber(const QString &phoneNumber, int numParts);
    void postionPrimary(qreal sceneWidth, qreal sceneHeight);
    void moveText(QLabel *textItem, int x, int y);
    QString insertSpecialChars(const std::string &input, const std::string &specialChars, int numInserts);
    void repositionGraphicsTextItems(int sceneHeight, int sceneWeight);
    QList<QPoint> generateRandomPoints(int width, int height, int numPoints);
    bool isOverlapping(const QPoint &newPoint, const QList<QPoint> &points, int itemSize, int padding);
    double distance(const QPoint &p1, const QPoint &p2);

    QColor generateRandomColor();
signals:
};

#endif // MIZUSHIRUSHIHANDORA_H
