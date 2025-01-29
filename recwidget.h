#ifndef RECWIDGET_H
#define RECWIDGET_H

#include "reddotwidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>

class RecWidget : public QWidget {
    Q_OBJECT

public:
    explicit RecWidget(QWidget *parent = nullptr, const QColor &color = Qt::red);
    RedDotWidget *redDot;
    QLabel *recText;
    QVBoxLayout *layout;
    void setColor(QColor color);
};

#endif // RECWIDGET_H
