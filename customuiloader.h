#ifndef CUSTOMUILOADER_H
#define CUSTOMUILOADER_H

#include <QUiLoader>
#include "customseekbar.h"

class CustomUILoader : public QUiLoader
{
public:
    explicit CustomUILoader(QObject *parent = nullptr);
    QWidget *createWidget(const QString &className, QWidget *parent, const QString &name) override;
};

#endif // CUSTOMUILOADER_H
