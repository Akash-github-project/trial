
#ifndef VIDEOCONTAINER_H
#include <QByteArray>
#include <QString>
#define VIDEOCONTAINER_H
struct VideoContainer {
    QByteArray data;
    int index;
    QString fileName;
};

#endif // VIDEOCONTAINER_H
