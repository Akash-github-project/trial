#ifndef APIMANAGER_H
#define APIMANAGER_H

#include <QObject>
#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QUrl>
#include <QObject>
#include <QJsonDocument>
#include <QDebug>


struct VideoData {
    QString duration;
    QString key;
    QString iv;
    QString fileName;
};


class ApiManager : public QObject
{
    Q_OBJECT

private:
    int * lisgOfKeys;
public:

    QNetworkAccessManager * manager;
    explicit ApiManager(QObject *parent = nullptr);
    QUrl * url;

    void GetKeysForChunk(QString testToken,QString courseId,QString videoId);
    QList<VideoData> parseVideoData(const QString &data);
signals:
    void onKeyFetchFinished(QList<VideoData>);
public slots:
    void onFinished(QNetworkReply *reply);
signals:
};

#endif // APIMANAGER_H
