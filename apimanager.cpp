#include "apimanager.h"

#include <EncryptionHandler.h>
#include <QFile>
#include <QSslKey>
#include <TextDecryptor.h>
#include <openssl/ec.h>
#include <openssl/err.h>
#include <QProcess>
#include <QString>


// thas just mobile
ApiManager::ApiManager(QString credentials,QString token,QString deviceId,QObject *parent)
    : QObject{parent}
{
    manager = new QNetworkAccessManager(this);
    // api manager setup

    userInfoLoggerManager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &ApiManager::onFinished);
    connect(userInfoLoggerManager,&QNetworkAccessManager::finished,this,&ApiManager::onSubmitUserInfo);

    QObject::connect(manager, &QNetworkAccessManager::sslErrors, [](QNetworkReply* reply, const QList<QSslError>& errors) {
        qWarning()<<"SSL" << errors.length();
        for (const QSslError& error : errors) {
            qWarning() << "SSL Error:" << error.errorString();
        }
        // For testing purposes only; in production, never ignore SSL errors
        reply->ignoreSslErrors();
    });

    QObject::connect(userInfoLoggerManager, &QNetworkAccessManager::sslErrors, [](QNetworkReply* reply, const QList<QSslError>& errors) {
        qWarning()<<"SSL" << errors.length();
        for (const QSslError& error : errors) {
            qWarning() << "SSL Error user logger: " << error.errorString();
        }
        // For testing purposes only; in production, never ignore SSL errors
        reply->ignoreSslErrors();
    });

    bsonObjectGenerator = new BSONObjectID(this);
    this->credentials = credentials;
    this->token = token;
    this->deviceId = deviceId;
}

ApiManager::ApiManager(QObject *parent)
    : QObject{parent}{  }


QString ApiManager::getMotherboardSerialNumber() {
/*    QString serialNumber;

#ifdef Q_OS_WIN
        // Use wmic on Windows
    QString command("powershell -Command \"wmic baseboard get serialnumber\"");
#else
        // Use dmidecode on Linux/macOS (might require installation)
    QString command("sudo dmidecode -t baseboard | grep Serial");
#endif
    QProcess process;
    process.start(command);
    // Wait for the command to finish
    if (!process.waitForFinished(5000)) { // Timeout after 3 seconds
        return QString(); // Return empty string
    }
    // Check if the process finished normally
    if (process.exitStatus() == QProcess::NormalExit && process.exitCode() == 0) {
        QString output = QString::fromUtf8(process.readAllStandardOutput());
        // Output the raw command output for debugging
        qDebug() << "Raw output:" << output;
        // Split the output into lines and remove any empty lines
        QStringList lines = output.split('\n'); // Split by newline
        lines.removeAll(QString()); // Remove empty strings
        // The second line (index 1) should contain the serial number
        if (lines.size() > 1) {
            serialNumber = lines[1].trimmed();  // Get the second line and trim whitespace
        }
    } else {
        //qDebug() << "Process error or command failed with exit code:" << process.exitCode() << ", Error:" << process.errorString();
    }
    //qDebug() << "Serial number:" << serialNumber;
    return serialNumber*/;
    return this->deviceId;
}



// add ssl

QSslConfiguration ApiManager::getSslConfig(){
    //QString base64Pfx = "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";
    QString base64Pfx = "";
    // Replace with actual Base64 encoded certificate stringd
    // Decode the base64-encoded PFX data
    //QString password = "#4546Vid";
    QString password = "";
    QByteArray pfxData = QByteArray::fromBase64(base64Pfx.toUtf8());
    QBuffer* newBuffer = new QBuffer();


    newBuffer->setData(pfxData);
    //qDebug() << " setting data";

    // Open the buffer for reading
    if (!newBuffer->open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open buffer for reading";
    }



    // Variables to hold the extracted information
    QSslKey privateKey;
    QSslCertificate certificate;
    QList<QSslCertificate> caCertificates;

    // Import the PKCS#12 data
    if (!QSslCertificate::importPkcs12(newBuffer, &privateKey, &certificate, &caCertificates, password.toUtf8())) {
        qWarning() << "Failed to import PKCS#12 data.";
        return QSslConfiguration();
    }

    // Create and configure the SSL settings
    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    sslConfig.setLocalCertificate(certificate);
    sslConfig.setPrivateKey(privateKey);
    sslConfig.setCaCertificates(caCertificates);
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyPeer);
    sslConfig.setProtocol(QSsl::TlsV1_2OrLater); // Ensure a secure protocol is used

    return sslConfig;
}

void ApiManager::GetKeysForChunk(QString testToken,QString courseId,QString videoId,QString courseItemId){
     ;
    QNetworkRequest request(QUrl(url + "/api/generate-video-metadata/"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    key =  gen.generateECKeyPair();
    this->clientPublicKey = gen.extractPublicKeyPEM(key);

    QString bearer = "Bearer ";
    QString tk = testToken;
    QString token = bearer + tk;
    //qDebug()<<"debug:: latin " <<token.toLatin1();
    request.setRawHeader("Authorization",token.toLatin1());
    request.setRawHeader("D",getMotherboardSerialNumber().toLatin1());
    request.setRawHeader("S","WC");
    request.setRawHeader("V",appVersion.toLatin1());
    request.setRawHeader("Accept","application/json; version=1.0");
    //request.setSslConfiguration(getSslConfig());
    //
    QJsonObject json;
    json["course_id"] = courseId;
    json["p"] = QString::fromStdString(clientPublicKey.toHex().toStdString());
    //qDebug()<<"debug:: " << courseId;
    json["video_id"] = videoId;
    json["course_item_id"] = courseItemId;
    //qDebug()<<"debug:: " << videoId;
    QJsonDocument jsonDoc(json);

    // Convert QJsonDocument to QByteArray
    QByteArray postData = jsonDoc.toJson();

    manager->post(request, postData);
}

void ApiManager::sendErrorInfo(QJsonDocument errorData,QString url){
    try {
        QString errorCode = errorData.object().value("code").toString();
        QJsonArray errorMessage = errorData.object().value("error_description").toArray();


        QNetworkRequest request(QUrl(url + "/api/record-error/"));
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        key =  gen.generateECKeyPair();
        this->clientPublicKey = gen.extractPublicKeyPEM(key);

        QString bearer = "Bearer ";
        QString bearerToken = bearer + this->token;
        //qDebug()<<"debug:: latin " <<token.toLatin1();
        request.setRawHeader("Authorization",bearerToken.toLatin1());
        request.setRawHeader("D",getMotherboardSerialNumber().toLatin1() );
        request.setRawHeader("S","WC");
        request.setRawHeader("V",appVersion.toLatin1());
        request.setRawHeader("Accept","application/json; version=1.0");
        //
         //request.setSslConfiguration(getSslConfig());
        QJsonObject json;
        json["mobile"] = this->credentials;
        json["description"] = errorMessage.at(0).toString();
        json["error"] = errorCode;
        json["data"] = url;
        QJsonDocument jsonDoc(json);

        // Convert QJsonDocument to QByteArray
        QByteArray postData = jsonDoc.toJson();
        manager->post(request, postData);
    }catch (const std::exception &e){

    }
}

void ApiManager::onSubmitUserInfo(QNetworkReply* reply){
    //qDebug()<<"in submitUserInfo";
    //qDebug()<<"in submitUserInfo data" << reply->url().toString();
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if(reply->url().toString().contains("api/record-error")){
        return;
    }

    if(reply->error() == QNetworkReply::NoError && reply->url().toString().contains("api/log-activity")){
        QByteArray response = reply->readAll();
        QJsonDocument document = QJsonDocument::fromJson(response,nullptr);
        qDebug()<<"time send response" << document.object();
        this->requestId = "";
        if(document.object().contains("session_id")){
            sessionId = document.object().value("session_id").toString();
            //qDebug()<<"session id" << sessionId;
            return;
        }
        emit onUserTimeSentSuccess();
    }else {

        QByteArray response = reply->readAll();
        QJsonDocument document = QJsonDocument::fromJson(response,nullptr);
        //qWarning() <<"error :" << document.object();
        if (reply->error() != QNetworkReply::NoError && statusCode == 412) {
            QString errorMessage = handlePreconditionFailed(document);
            emit noNetworkForTimer(errorMessage);
        }
        else if (statusCode == 401) {
            emit noNetworkForTimer("Your session has expired.\nPlease log in again.");
        }
        else if (statusCode == 500) {
            sendErrorInfo(document,reply->url().toString());
            emit noNetworkForTimer("Server error" + QString::number(statusCode) + ".\nPlease contact support.");
        }else if(statusCode == 443 || statusCode == 0){
             //qWarning() << "Error:" << reply->errorString();
             //qDebug() << "No network error from API manager 'onSubmitUserInfo'";
            emit noNetworkForTimer("No Internet connection.");
        }
        else {
            //qWarning() << "Error:" << reply->errorString();
             //qDebug() << "No network error from API manager 'onSubmitUserInfo'";
            emit noNetworkForTimer("Server error" + QString::number(statusCode) + ".\nPlease contact support.");
        }
    }
    reply->deleteLater();
}

QString ApiManager::handlePreconditionFailed(QJsonDocument errorData){
    QString errorCode = errorData.object().value("code").toString();
    QJsonArray errorMessage = errorData.object().value("error_description").toArray();
    QString message = errorMessage.count() > 0? "Error Code : " + errorCode + "\n" + errorMessage.at(0).toString():"Error Code :" + errorCode +  "\nPlease contact support";
    return message;
}

QString ApiManager::getVideoMetadata(QString spk,QString iv,QString text){
        //EncryptionHandler  handler;
        TextDecryptor decryptor;
        QByteArray pmkey = QByteArray::fromHex(spk.toUtf8());
        //qDebug()<<"pem key" << pmkey.toStdString();

        EC_KEY * serverKey = gen.loadServerPublicKey(pmkey);
        QByteArray unCompressed = gen.publicKeyToUncompressedHex(serverKey);

        //qDebug()<<"uncompressed key" << unCompressed;

        const EC_GROUP* group = EC_KEY_get0_group(key);
        EC_POINT* server_point = gen.hexToECPoint(unCompressed, const_cast<EC_GROUP*>(group));
        /////////

        QByteArray sharedSecretArray = gen.deriveSharedSecret(key, server_point);
        //qDebug() << "Shared secret derived successfully on client" << sharedSecretArray.toHex();

        QByteArray info = "handshake data"; // Info for HKDF
        size_t keyLength = 32; // Desired key ength (e.g., 256 bits)

        QByteArray derivedKeyValue ;
        QByteArray salt;
        gen.hkdf(salt,sharedSecretArray,info,derivedKeyValue,keyLength);

        //qDebug() << "Derived Key:" << derivedKeyValue.toHex();
        return decryptor.decryptText(derivedKeyValue.toHex(),iv,text);
}

QJsonDocument ApiManager::jsonStringToDocument(const QString& jsonString) {
    // Initialize a QJsonParseError to capture any parsing errors
    QJsonParseError parseError;

    // Convert the JSON string to a QJsonDocument
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8(), &parseError);

    // Check for parsing errors
    if (parseError.error != QJsonParseError::NoError) {
        //qWarning() << "Failed to parse JSON string:" << parseError.errorString();
        return QJsonDocument(); // Return an empty QJsonDocument if parsing fails
    }

    return jsonDoc;
}

void ApiManager::onFinished(QNetworkReply* reply) {

    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if(reply->url().toString().contains("api/record-error")){
        return;
    }



    if (reply->error() == QNetworkReply::NoError && reply->url().toString().contains("api/generate-video-metadata")) {
        QByteArray responseData = reply->readAll();
        QJsonDocument document = QJsonDocument::fromJson(responseData,nullptr);

        QString metadata = document.object().value("metadata").toString();
        QString spk = document.object().value("spk").toString();
        QString iv = document.object().value("iv").toString();
        QString md = this->getVideoMetadata(spk,iv,metadata);

        QJsonDocument jmd = jsonStringToDocument(md);
        QJsonValue jmv = jmd.object().value("metadata");
        QList<VideoData> resultOfParsing = parseVideoData(jmv.toString());


        // Watermark config generation
        MizuConfig *config = new MizuConfig();
        config->log_activity_interval = jmd.object().value("log_activity_interval").toInt();
        if(config->log_activity_interval == 0){
            config->log_activity_interval = 60;
        }
        config->resume_at_seconds = jmd.object().value("resume_at_seconds").toInt();

        qDebug()<<"----- log"<< config->log_activity_interval<< "log interval";

        // Small watermark (sm_wm) config
        config->smConfig = new SmMizu();
        qDebug()<<"sm new ---- " << jmd.object().value("sm_wm").toString();
        auto smConfig = jmd.object().value("sm_wm").toObject();
        config->smConfig->font_size = smConfig.value("font_size").toInt();
        config->smConfig->transparent_start = smConfig.value("transparency_start").toInt();
        config->smConfig->transparent_end = smConfig.value("transparency_end").toInt();
        config->smConfig->count_per_frame = smConfig.value("count_per_frame").toInt();
        config->smConfig->interval = smConfig.value("interval").toInt();

        // Convert "colors" array to QStringList
        QJsonArray colorsArray = smConfig.value("colors").toArray();
        for (const QJsonValue &colorValue : colorsArray) {
            config->smConfig->colors.append(colorValue.toString());
        }

        // Large watermark (lg_wm) config
        config->lgConfig = new LgMizu();
        auto lgConfig = jmd.object().value("lg_wm").toObject();
        config->lgConfig->angle = lgConfig.value("angle").toInt();
        config->lgConfig->font_size = lgConfig.value("font_size").toInt();
        config->lgConfig->transparency = lgConfig.value("transparency").toInt();
        config->lgConfig->width_percent = lgConfig.value("width_percent").toInt();
        config->lgConfig->color = lgConfig.value("color").toString();

        // flash wm

        config->flConfig = new FlashMizu();
        auto flConfig = jmd.object().value("flash_wm").toObject();
        config->flConfig->chn_duration = flConfig.value("chn_duration").toInt();
        config->flConfig->color = flConfig.value("color").toString();
        QJsonArray transparencyArray = flConfig.value("transparency").toArray();
        for (const QJsonValue &transparencyValue : transparencyArray) {
            config->flConfig->transparency.append(transparencyValue.toInt());
        }

        config->dflConfig = new DFlashMizu();
        auto dflConfig = jmd.object().value("d_flash_wm").toObject();
        config->dflConfig->chn_duration = dflConfig.value("chn_duration").toInt();
        config->dflConfig->color = dflConfig.value("color").toString();
        QJsonArray dTransparencyArray = dflConfig.value("transparency").toArray();
        for (const QJsonValue &transparencyValue : dTransparencyArray) {
            config->dflConfig->transparency.append(transparencyValue.toInt());
        }

        // qDebug() << "lgConfig" << QJsonDocument(lgConfig).toJson(QJsonDocument::Compact);
        // qDebug() << "smConfig" << QJsonDocument(smConfig).toJson(QJsonDocument::Compact);
        emit onKeyFetchFinished(resultOfParsing,config,jmd.object().value("duration").toInt());
    } else if(reply->error() != QNetworkReply::NoError && reply->url().toString().contains("api/generate-video-metadata/")){
        QByteArray response = reply->readAll();
        QJsonDocument document = QJsonDocument::fromJson(response,nullptr);
        //qWarning()<<document.object();
        if(statusCode == 412){
            QString errorMessage =  handlePreconditionFailed(document);
            emit noNetwork(errorMessage);
        }else if(statusCode == 401){
            emit noNetwork("Your session has expired.\nPlease log in again.");
        } else if(statusCode == 500){
            sendErrorInfo(document,reply->url().toString());
            emit noNetwork("Server Error " + QString::number(statusCode) + ".\nPlease contact support\n at support@vidsafe.in" );
        }else if(statusCode == 443 || statusCode == 0){
            emit noNetworkForTimer("No Internet connection.");
        }
        else {
            //qWarning() << "Error:" << reply->errorString();
            QString message = handlePreconditionFailed(document);
            qDebug()<<"no netowrk error from API manager 'onSubmitUserInfo'" << statusCode <<"error message" << message;

            emit noNetwork("Server Error " + QString::number(statusCode) + ".\nPlease contact support");
            // emit noNetwork("No Internet connection");
        }
    }else {
        //qWarning() << "Error:" << reply->errorString() << " " << statusCode;
        emit noNetwork("Server Error " + QString::number(statusCode) + ".\nPlease contact support");
    }
    reply->deleteLater();
}





void ApiManager::sendUserWatchTime(QString token, QString courseId,QString courseItemId,QString videoId,qint64 playbackTime,qint64 currentPosition){
    QNetworkRequest request(QUrl(url + "/api/log-activity/"));
    if(this->requestId.length() == 0) {
        this->requestId = QString::fromStdString(bsonObjectGenerator->generate());
    } //

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QString bearer = "Bearer ";
    QString tk = token;
    QString tokenString = bearer + tk;
    //qDebug()<<"debug:: latin " <<tokenString.toLatin1();
    qDebug()<<"sesion id ----" <<sessionId;
    request.setRawHeader("Authorization",tokenString.toLatin1());
    request.setRawHeader("D",getMotherboardSerialNumber().toLatin1());
    request.setRawHeader("S","WC");
    request.setRawHeader("V",appVersion.toLatin1());
    request.setRawHeader("Accept","application/json; version=1.0");
    //request.setSslConfiguration(getSslConfig());
    QJsonObject jsonObject;

    // Assign values to the QJsonObject
    jsonObject["activity_type"] = "VIDEO_WATCH";
    jsonObject["course_id"] = courseId;
    jsonObject["course_item_id"] = courseItemId;
    jsonObject["video_id"] = videoId;
    jsonObject["video_watch_duration"] = playbackTime;
    jsonObject["session_id"] = sessionId;
    jsonObject["current_seek_time"] = currentPosition;
    jsonObject["id"] = this->requestId;

    qDebug()<< "--------------------------------------------";
    // Create and format the activity datetime
    QDateTime dateTime = QDateTime::currentDateTime(); // Replace with the actual datetime if needed
    QString formattedDateTime = dateTime.toString("yyyy-MM-dd HH:mm:ss");
    jsonObject["activity_datetime"] = formattedDateTime;
    QJsonDocument jsonDoc(jsonObject);
    // Convert QJsonDocument to QByteArray
    QByteArray postData = jsonDoc.toJson();
    qDebug()<<"sending user watch time";
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


void ApiManager::getSessionId(QString token, QString courseId,QString courseItemId,QString videoId){
    QNetworkRequest request(QUrl(url + "/api/log-activity/"));
    //
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QString bearer = "Bearer ";
    QString tk = token;
    QString tokenString = bearer + tk;
    //qDebug()<<"debug:: latin " <<tokenString.toLatin1();
    request.setRawHeader("Authorization",tokenString.toLatin1());
    request.setRawHeader("D",getMotherboardSerialNumber().toLatin1());
    request.setRawHeader("S","WC");
    request.setRawHeader("V",appVersion.toLatin1());
    request.setRawHeader("Accept","application/json; version=1.0");
    //request.setSslConfiguration(getSslConfig());
    QJsonObject jsonObject;

    // Assign values to the QJsonObject
    jsonObject["activity_type"] = "VIDEO_OPEN";
    jsonObject["course_id"] = courseId;
    jsonObject["course_item_id"] = courseItemId;
    jsonObject["video_id"] = videoId;

    qDebug()<< "--------------------------------------------";
    // Create and format the activity datetime
    QDateTime dateTime = QDateTime::currentDateTime(); // Replace with the actual datetime if needed
    QString formattedDateTime = dateTime.toString("yyyy-MM-dd HH:mm:ss");
    jsonObject["activity_datetime"] = formattedDateTime;
    QJsonDocument jsonDoc(jsonObject);
    // Convert QJsonDocument to QByteArray
    QByteArray postData = jsonDoc.toJson();
    userInfoLoggerManager->post(request, postData);
}

