#include "apimanager.h"

ApiManager::ApiManager(QObject *parent)
    : QObject{parent}
{
    manager = new QNetworkAccessManager(this);
    userInfoLoggerManager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &ApiManager::onFinished);
    connect(userInfoLoggerManager,&QNetworkAccessManager::finished,this,&ApiManager::onSubmitUserInfo);
}

void ApiManager::GetKeysForChunk(QString testToken,QString courseId,QString videoId){

    QNetworkRequest request(QUrl("https://test-server.vidsafe.in/api/generate-video-metadata/"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QString bearer = "Bearer ";
    QString tk = testToken;
    QString token = bearer + tk;
    //qDebug()<<"debug:: latin " <<token.toLatin1();
    request.setRawHeader("Authorization",token.toLatin1());
    QJsonObject json;
    json["course_id"] = courseId;
    //qDebug()<<"debug:: " << courseId;
    json["video_id"] = videoId;
    //qDebug()<<"debug:: " << videoId;
    QJsonDocument jsonDoc(json);

    // Convert QJsonDocument to QByteArray
    QByteArray postData = jsonDoc.toJson();

     manager->post(request, postData);
}

void ApiManager::onSubmitUserInfo(QNetworkReply* reply){
    if(reply->error() == QNetworkReply::NoError && reply->url().toString().contains("api/log-activity")){
        QByteArray response = reply->readAll();
        QJsonDocument document = QJsonDocument::fromJson(response,nullptr);
        //qDebug()<<"time send response" << document.object();
        emit onUserTimeSentSuccess();
    }else {
        //qWarning() << "Error:" << reply->errorString();
        emit noNetworkForTimer();
    }
    reply->deleteLater();
}


void ApiManager::onFinished(QNetworkReply* reply) {
    if (reply->error() == QNetworkReply::NoError && reply->url().toString().contains("api/generate-video-metadata")) {
        //QByteArray response = reply->readAll();
        QByteArray responseData = reply->readAll();
        QJsonDocument document = QJsonDocument::fromJson(responseData,nullptr);

        //document
        //qDebug()<<object[""].toString();
        QJsonValue value = document.object().value("video_metadata");
        QJsonValue keys = value.toObject().value("metadata");
        //qDebug()<<"hello---"<<keys.toString();
        QList<VideoData> resultOfParsing = parseVideoData(keys.toString());
        //qDebug()<<"Api request done";
        emit onKeyFetchFinished(resultOfParsing);
    } else {
        qWarning() << "Error:" << reply->errorString();
        emit noNetowrk();
    }
    reply->deleteLater();
}

void ApiManager::sendUserWatchTime(QString token, QString courseId,QString courseItemId,QString videoId,qint64 playbackTime){
    QNetworkRequest request(QUrl("https://test-server.vidsafe.in/api/log-activity/"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QString bearer = "Bearer ";
    QString tk = token;
    QString tokenString = bearer + tk;
    //qDebug()<<"debug:: latin " <<tokenString.toLatin1();
    request.setRawHeader("Authorization",tokenString.toLatin1());

    QJsonObject jsonObject;

    // Assign values to the QJsonObject
    jsonObject["activity_type"] = "VIDEO_WATCH";
    jsonObject["course_id"] = courseId;
    jsonObject["course_item_id"] = courseItemId;
    jsonObject["video_id"] = videoId;
    jsonObject["video_watch_duration"] = playbackTime;

    qDebug()<< "--------------------------------------------";
    // qDebug()<<"TIME_API "<< "VIDEO_WATCH";
    // qDebug()<<"TIME_API course id"<< courseId;
    // qDebug()<<"TIME_API course item id"<< courseItemId;
    // qDebug()<<"TIME_API video id"<< videoId;
    // qDebug()<<"TIME_API watch duration"<< playbackTime;
    // qDebug()<< "--------------------------------------------";
    // Create and format the activity datetime
    QDateTime dateTime = QDateTime::currentDateTime(); // Replace with the actual datetime if needed
    QString formattedDateTime = dateTime.toString("yyyy-MM-dd HH:mm:ss");
    jsonObject["activity_datetime"] = formattedDateTime;
    QJsonDocument jsonDoc(jsonObject);
    // Convert QJsonDocument to QByteArray
    QByteArray postData = jsonDoc.toJson();
    userInfoLoggerManager->post(request, postData);
}



QList<VideoData> ApiManager::parseVideoData(const QString &data) {
    QList<VideoData> resultList;

    // Remove the surrounding quotes and parse as a single string
    QString trimmedData = data.mid(1, data.length() - 2); // Remove leading "[\"", trailing "\"]"
    //qDebug()<<trimmedData;
    QStringList segments = trimmedData.split(",");

    //qDebug()<<"start data parsing == "<<segments;
    for (const QString &segment : segments) {
        QStringList parts = segment.trimmed().remove("'").split("|");
        //qDebug()<< segments << "--segment--";
        if (parts.size() == 4) {
            auto tempDurationFirstPart = parts[0].trimmed().split(":").first().toStdString();
            auto tempDurationSecondPart = parts[0].trimmed().split(":").last().toStdString();
            int durationNumber =  std::__cxx11::stoi(tempDurationSecondPart) - std::__cxx11::stoi(tempDurationFirstPart);
            QString duration = QString::number(durationNumber);
            QString key = parts[1].remove("\\x").remove("'").trimmed();
            QString iv = parts[2].remove("\\x").remove("'").trimmed();
            QString fileName = parts[3].split("/").last().trimmed();

            VideoData videoData = { duration, key, iv, fileName };
            resultList.append(videoData);
        } else {
            //qDebug() << "Invalid segment format:" <<  parts.size();
        }
    }
    //qDebug()<<"end data parsing";

    return resultList;
}

