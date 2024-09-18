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
    QNetworkAccessManager * userInfoLoggerManager;
    explicit ApiManager(QObject *parent = nullptr);
    QUrl * url;

    void GetKeysForChunk(QString testToken,QString courseId,QString videoId);
    QList<VideoData> parseVideoData(const QString &data);
    void sendUserWatchTime(QString token, QString courseId,QString courseItemId,QString videoId,qint64 playbackTime);
signals:
    void onKeyFetchFinished(QList<VideoData>);
    void noNetowrk();
    void noNetworkForTimer();
    void onUserTimeSentSuccess();
public slots:
    void onFinished(QNetworkReply *reply);
    void onSubmitUserInfo(QNetworkReply *reply);
signals:
};

#endif // APIMANAGER_H
