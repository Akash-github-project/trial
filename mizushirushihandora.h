#ifndef MIZUSHIRUSHIHANDORA_H
#define MIZUSHIRUSHIHANDORA_H

#include <qgraphicsitem.h>

#include <QGraphicsOpacityEffect>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QLabel>
#include <QObject>
#include <QPropertyAnimation>
#include <QRandomGenerator>
#include <QSequentialAnimationGroup>
#include <QString>
#include <QTimer>
#include <QVector>
#include <cstdlib>  // For rand() and srand()
#include <ctime>    // For time()
#include <iostream>
#include <string>

#include "customgraphicstextitem.h"
#include "playercontrollerwidget.h"
#define PRIMARY_WATERMARK_TEXT only_pain

struct XYforMizu {
  qreal x;
  qreal y;
};

struct SmMizu {
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
  bool detect_external_display;
  SmMizu *smConfig;
  LgMizu *lgConfig;
  FlashMizu *flConfig;
  DFlashMizu *dflConfig;
};

class CustomGraphicsTextItem;
class MizuShirushiHandora : public QObject {
  Q_OBJECT
 public:
  explicit MizuShirushiHandora(QObject *parent = nullptr);
  MizuShirushiHandora(QObject *parent, QWidget *sceen, QString userIdentifier,
                      int rows, int columns, MizuConfig *config);

  CustomGraphicsTextItem *PRIMARY_WATERMARK_TEXT;
  MizuConfig *config;
  QVector<QLabel *> textItems;
  QWidget *sceen;
  QString phoneNumber = nullptr;
  QTimer *animationTimer;
  QMap<QLabel *, QGraphicsOpacityEffect *> effects;  // Store QLabel effects
  int numParts;
  int numRows;
  int numCols;
  int screenWidth = 0;
  int screenHeight = 0;

 public:
  void updateWatermark(int sceenWidth, int sceenHeight);
  QString generateRandomCharacters(int length);
  QVector<QString> splitPhoneNumber(const QString &phoneNumber, int numParts);
  void postionPrimary(qreal sceneWidth, qreal sceneHeight);
  void moveText(QLabel *textItem, int x, int y);
  QString insertSpecialChars(const std::string &input,
                             const std::string &specialChars, int numInserts);
  void repositionGraphicsTextItems(int sceneHeight, int sceneWeight);
  QList<QPoint> generateRandomPoints(int width, int height, int numPoints);
  bool isOverlapping(const QPoint &newPoint, const QList<QPoint> &points,
                     int itemSize, int padding);
  double distance(const QPoint &p1, const QPoint &p2);

  QColor generateRandomColor();
 signals:
 private:
  void animateLabel(QLabel *label);
  void moveWm();
};

#endif  // MIZUSHIRUSHIHANDORA_H
