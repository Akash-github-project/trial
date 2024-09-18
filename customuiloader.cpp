#include "customuiloader.h"

CustomUILoader::CustomUILoader(QObject *parent)
    : QUiLoader{parent}
{

}

QWidget* CustomUILoader::createWidget(const QString &className, QWidget *parent, const QString &name) {
    if (className == "CustomSeekbar") {
        CustomSeekbar *widget = new CustomSeekbar(parent);
        widget->setObjectName(name);  // Set the object name as given in the .ui file
        return widget;
    }
    // For all other widgets, use the default implementation
    return QUiLoader::createWidget(className, parent, name);
}
