#include "apimanager.h"

ApiManager::ApiManager(QObject *parent)
    : QObject{parent}
{

    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &ApiManager::onFinished);
}

void ApiManager::GetKeysForChunk(QString testToken,QString courseId,QString videoId){

    QNetworkRequest request(QUrl("https://test-server.vidsafe.in/api/generate-video-metadata/"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QString bearer = "Bearer ";
    QString tk = testToken;
    QString token = bearer + tk;
    qDebug()<<"debug:: latin " <<token.toLatin1();
    request.setRawHeader("Authorization",token.toLatin1());
    QJsonObject json;
    json["course_id"] = courseId;
    qDebug()<<"debug:: " << courseId;
    json["video_id"] = videoId;
    qDebug()<<"debug:: " << videoId;
    QJsonDocument jsonDoc(json);

    // Convert QJsonDocument to QByteArray
    QByteArray postData = jsonDoc.toJson();

     manager->post(request, postData);
}


void ApiManager::onFinished(QNetworkReply* reply) {
    if (reply->error() == QNetworkReply::NoError) {
        //QByteArray response = reply->readAll();
        QByteArray responseData = reply->readAll();
        QJsonDocument document = QJsonDocument::fromJson(responseData,nullptr);

        //document
        //qDebug()<<object[""].toString();
        QJsonValue value = document.object().value("video_metadata");
        QJsonValue keys = value.toObject().value("metadata");
        //qDebug()<<"hello---"<<keys.toString();
        QList<VideoData> resultOfParsing = parseVideoData(keys.toString());
        qDebug()<<"Api request done";
        emit onKeyFetchFinished(resultOfParsing);
    } else {
        qDebug() << "Error:" << reply->errorString();
    }
    reply->deleteLater();
}



QList<VideoData> ApiManager::parseVideoData(const QString &data) {
    QList<VideoData> resultList;

    // Remove the surrounding quotes and parse as a single string
    QString trimmedData = data.mid(1, data.length() - 2); // Remove leading "[\"", trailing "\"]"
    qDebug()<<trimmedData;
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
            qDebug() << "Invalid segment format:" <<  parts.size();
        }
    }
    qDebug()<<"end data parsing";

    return resultList;
}

