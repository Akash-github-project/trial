#ifndef REDDOTWIDGET_H
#define REDDOTWIDGET_H

#include <QPainter>
#include <QWidget>


class RedDotWidget : public QWidget {
    Q_OBJECT

public:
    explicit RedDotWidget(QWidget *parent = nullptr, const QColor &color = Qt::red);
    void setColor(QColor color);

protected:
    void paintEvent(QPaintEvent *) override;


private:
    QColor dotColor;
};


#endif // REDDOTWIDGET_H
