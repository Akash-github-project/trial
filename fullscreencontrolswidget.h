#ifndef FULLSCREENCONTROLSWIDGET_H
#define FULLSCREENCONTROLSWIDGET_H

#include <QTimer>
#include <QWidget>

class FullScreenControlsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FullScreenControlsWidget(QWidget *parent,QWidget* controlWidget);

    void animateVisibility(QWidget *widget, bool visible);
protected:
    // Detect mouse movement within the widget
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

signals:
    void exitFullScreen();

public slots:
    void showAndResetTimer();

private:
    QTimer *inactivityTimer; // Timer to track inactivity
    QWidget* controlWidget;
    bool isFullyShown = false;
};

#endif // FULLSCREENCONTROLSWIDGET_H
