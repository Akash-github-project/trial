#ifndef CLICKABLEQFRAME_H
#define CLICKABLEQFRAME_H

#include <QFrame>
#include <QMouseEvent>
#include <QWidget>

class ClickableQFrame : public QFrame
{
    Q_OBJECT
public:
    ClickableQFrame();
    ClickableQFrame(QWidget * widget = nullptr);
    ClickableQFrame(QString unFocusedColor,QString focusedColor,int borderRadius);
public:
    void updateColorScene(QString unFocusedColor,QString focusedColor,int borderRadius);

signals:
    void clicked();
protected:
    void mousePressEvent(QMouseEvent * event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;
    //void paintEvent(QPaintEvent * event) override;
private:
    bool isPressed = false;
    int borderRadius = 0;

    QString focusedColor = "background:%1;";
    QString unFocusedColor = "background:%1;";
    QString borderRadiusString = "border-radius:%1px;";
    QString unFocusedStyleSheet = "QFrame {"
                             "%1"
                             "%2"
                             "}";
    QString focusedStyleSheet = "QFrame {"
                             "%1"
                             "%2"
                             "}";


};


#endif // CLICKABLEQFRAME_H
