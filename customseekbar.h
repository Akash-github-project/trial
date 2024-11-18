#ifndef CUSTOMSEEKBAR_H
#define CUSTOMSEEKBAR_H

#include <QSlider>
#include <QDebug>
#include <QMouseEvent>

class CustomSeekbar : public QSlider
{
    Q_OBJECT
public:
    CustomSeekbar(QWidget *parent = nullptr);

 signals:
    void userClickOnSeekbar();

public slots:
    void onSliderClick(QPointF point,QRectF parentRect);

protected:
    void mousePressEvent ( QMouseEvent * event ) override
    {
        QSlider::mousePressEvent(event);
        if (event->button() == Qt::LeftButton)
        {
            int maxValue = maximum();
            int minValue = minimum();
            int maxAllowedValue = minValue + (maxValue - minValue) * 1.0;

            int newValue;
            if (orientation() == Qt::Vertical)
            {
                newValue = minValue + ((maxValue - minValue) * (height() - event->position().y())) / height();
            }
            else
            {
                newValue = minValue + ((maxValue - minValue) * event->position().x()) / width();
                qDebug()<<minValue << " + " <<"(" <<maxValue <<" - " <<minValue <<") * " << event->position().x() - 4 << " / " << width();
                qDebug()<<"newValue "<<newValue;
                qDebug()<<"maxAllowedValue" << maxAllowedValue;
            }

            // Restrict the value to the maximum allowed value
            if (newValue > maxAllowedValue)
            {
                newValue = maxAllowedValue;
            }
            emit userClickOnSeekbar();

            setValue(newValue);
        }
    }

    void mouseMoveEvent(QMouseEvent * event) override {
        QSlider::mouseMoveEvent(event);
    }

};

#endif // CUSTOMSEEKBAR_H
