#ifndef APIMANAGER_H
#define APIMANAGER_H
//#define PROD

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
#include <QRegularExpression>

#include "bsonobjectid.h"
#include "sharekeygenerator.h"
#include <mizushirushihandora.h>

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
    QString requestId = "";
    QString	credentials = "";
    QString token = "";
    QString deviceId = "";
#ifdef PROD
    QString url = "https://secure.vidsafe.in";
    QString appVersion = "241201";
#else
   QString url = "https://test-server.vidsafe.in";
    QString appVersion = "241002";
#endif
public:
    BSONObjectID * bsonObjectGenerator;
    QNetworkAccessManager * manager;
    QNetworkAccessManager * userInfoLoggerManager;
    QByteArray clientPublicKey;
    explicit ApiManager(QObject *parent = nullptr);
    ApiManager(QString credentials,QString token,QString deviceId,QObject *parent = nullptr);
    // QUrl * url;
    ShareKeyGenerator gen;
    EC_KEY* key;
    void GetKeysForChunk(QString testToken,QString courseId,QString videoId,QString courseItemId);
    QList<VideoData> parseVideoData(const QString &data);
    void sendUserWatchTime(QString token, QString courseId,QString courseItemId,QString videoId,qint64 playbackTime,qint64 currentPosition);
    QSslConfiguration getSslConfig();
    QString getVideoMetadata(QString spk, QString iv, QString text);
    QJsonDocument jsonStringToDocument(const QString &jsonString);
    QString getMotherboardSerialNumber();
    QString sessionId = "";
    void getSessionId(QString token, QString courseId, QString courseItemId, QString videoId);
    QString handlePreconditionFailed(QJsonDocument errorData);
    void sendErrorInfo(QJsonDocument errorData, QString url);
signals:
    void onKeyFetchFinished(QList<VideoData> list,MizuConfig * config,int duration);
    void noNetwork(QString message);
    void noNetworkForTimer(QString message);
    void onUserTimeSentSuccess();
    //void onSessionReceived(QString sessionId);
public slots:
    void onFinished(QNetworkReply *reply);
    void onSubmitUserInfo(QNetworkReply *reply);
signals:
};

#endif // APIMANAGER_H
