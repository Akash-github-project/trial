#include "clickableqframe.h"

ClickableQFrame::ClickableQFrame(){
    this->unFocusedColor = this->unFocusedColor.arg("#676767");
    this->focusedColor = this->focusedColor.arg("#865743");

    this->borderRadiusString = this->borderRadiusString.arg("0");
    this->unFocusedStyleSheet = this->unFocusedStyleSheet.arg(this->unFocusedColor,this->borderRadiusString);
    this->focusedStyleSheet = this->focusedStyleSheet.arg(this->focusedColor,this->borderRadiusString);
}

ClickableQFrame::ClickableQFrame(QWidget *widget):QFrame(widget){
    // this->unFocusedColor = this->unFocusedColor.arg("#676767");
    // this->focusedColor = this->focusedColor.arg("#865743");
    // this->borderRadiusString = this->borderRadiusString.arg("0");
    // this->unFocusedStyleSheet = this->unFocusedStyleSheet.arg(this->unFocusedColor,this->borderRadiusString);
    // this->focusedStyleSheet = this->focusedStyleSheet.arg(this->focusedColor,this->borderRadiusString);
}

ClickableQFrame::ClickableQFrame(QString unFocusedColor,QString focusedColor,int borderRadius) {
    this->unFocusedColor = this->unFocusedColor.arg(unFocusedColor);
    this->focusedColor = this->focusedColor.arg(focusedColor);

    this->borderRadiusString = this->borderRadiusString.arg(borderRadius);
    this->unFocusedStyleSheet = this->unFocusedStyleSheet.arg(this->unFocusedColor,this->borderRadiusString);
    this->focusedStyleSheet = this->focusedStyleSheet.arg(this->focusedColor,this->borderRadiusString);
}

void ClickableQFrame::updateColorScene(QString unFocusedColor,QString focusedColor,int borderRadius){
    this->unFocusedColor = this->unFocusedColor.arg(unFocusedColor);
    this->focusedColor = this->focusedColor.arg(focusedColor);

    this->borderRadiusString = this->borderRadiusString.arg(borderRadius);
    this->unFocusedStyleSheet = this->unFocusedStyleSheet.arg(this->unFocusedColor,this->borderRadiusString);
    this->focusedStyleSheet = this->focusedStyleSheet.arg(this->focusedColor,this->borderRadiusString);
    this->setStyleSheet(this->unFocusedStyleSheet);
}

void ClickableQFrame::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        emit clicked();
        isPressed = true;
        event->accept();
        this->setStyleSheet(this->focusedStyleSheet);
    }
        QFrame::mousePressEvent(event);
}

void ClickableQFrame::mouseReleaseEvent(QMouseEvent *event){
    isPressed = false;
    this->setStyleSheet(this->unFocusedStyleSheet);
    QFrame::mouseReleaseEvent(event);
}



// void ClickableQFrame::paintEvent(QPaintEvent * event){
//     if(isPressed){

//     } else {

//     }
//     QFrame::paintEvent(event);
// }

