#include "apimanager.h"

#include <EncryptionHandler.h>
#include <TextDecryptor.h>
#include <openssl/ec.h>
#include <openssl/err.h>

#include <QFile>
#include <QProcess>
#include <QSslKey>
#include <QString>

// thas just mobile
ApiManager::ApiManager(QString credentials, QString token, QString deviceId,
                       QObject* parent)
    : QObject{parent} {
  manager = new QNetworkAccessManager(this);
  // api manager setup

  userInfoLoggerManager = new QNetworkAccessManager(this);
  connect(manager, &QNetworkAccessManager::finished, this,
          &ApiManager::onFinished);
  connect(userInfoLoggerManager, &QNetworkAccessManager::finished, this,
          &ApiManager::onSubmitUserInfo);

  QObject::connect(manager, &QNetworkAccessManager::sslErrors,
                   [](QNetworkReply* reply, const QList<QSslError>& errors) {
                     qWarning() << "SSL" << errors.length();
                     for (const QSslError& error : errors) {
                       qWarning() << "SSL Error:" << error.errorString();
                     }
                     // For testing purposes only; in production, never ignore
                     // SSL errors
                     reply->ignoreSslErrors();
                   });

  QObject::connect(userInfoLoggerManager, &QNetworkAccessManager::sslErrors,
                   [](QNetworkReply* reply, const QList<QSslError>& errors) {
                     qWarning() << "SSL" << errors.length();
                     for (const QSslError& error : errors) {
                       qWarning()
                           << "SSL Error user logger: " << error.errorString();
                     }
                     // For testing purposes only; in production, never ignore
                     // SSL errors
                     reply->ignoreSslErrors();
                   });

  bsonObjectGenerator = new BSONObjectID(this);
  this->credentials = credentials;
  this->token = token;
  this->deviceId = deviceId;
}

ApiManager::ApiManager(QObject* parent) : QObject{parent} {}

QString ApiManager::getMotherboardSerialNumber() {
  /*    QString serialNumber;

  #ifdef Q_OS_WIN
          // Use wmic on Windows
      QString command("powershell -Command \"wmic baseboard get
  serialnumber\""); #else
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
      if (process.exitStatus() == QProcess::NormalExit && process.exitCode() ==
  0) { QString output = QString::fromUtf8(process.readAllStandardOutput());
          // Output the raw command output for debugging
          qDebug() << "Raw output:" << output;
          // Split the output into lines and remove any empty lines
          QStringList lines = output.split('\n'); // Split by newline
          lines.removeAll(QString()); // Remove empty strings
          // The second line (index 1) should contain the serial number
          if (lines.size() > 1) {
              serialNumber = lines[1].trimmed();  // Get the second line and
  trim whitespace
          }
      } else {
          //qDebug() << "Process error or command failed with exit code:" <<
  process.exitCode() << ", Error:" << process.errorString();
      }
      //qDebug() << "Serial number:" << serialNumber;
      return serialNumber*/
  ;
  return this->deviceId;
}

// add ssl

QSslConfiguration ApiManager::getSslConfig() {
  // test server
  // Decode the base64-encoded PFX data
  QByteArray password =
      QByteArray::fromBase64(QString::fromUtf8(prod_cert_pass).toUtf8());
  QByteArray pfxData =
      QByteArray::fromBase64(QString::fromUtf8(prod_cert).toUtf8());
  QBuffer* newBuffer = new QBuffer();

  newBuffer->setData(pfxData);

  // Open the buffer for reading
  if (!newBuffer->open(QIODevice::ReadOnly)) {
    qWarning() << "Failed to open buffer for reading";
  }

  // Variables to hold the extracted information
  QSslKey privateKey;
  QSslCertificate certificate;
  QList<QSslCertificate> caCertificates;

  // Import the PKCS#12 data
  if (!QSslCertificate::importPkcs12(newBuffer, &privateKey, &certificate,
                                     &caCertificates, password)) {
    qWarning() << "Failed to import PKCS#12 data.";
    return QSslConfiguration();
  }

  // Create and configure the SSL settings
  QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
  sslConfig.setLocalCertificate(certificate);
  sslConfig.setPrivateKey(privateKey);
  sslConfig.setCaCertificates(caCertificates);
  sslConfig.setPeerVerifyMode(QSslSocket::VerifyPeer);
  sslConfig.setProtocol(
      QSsl::TlsV1_2OrLater);  // Ensure a secure protocol is used

  return sslConfig;
}

void ApiManager::GetKeysForChunk(QString testToken, QString courseId,
                                 QString videoId, QString courseItemId) {
  QNetworkRequest request(QUrl(url + "/api/generate-video-metadata/"));
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

  key = gen.generateECKeyPair();
  this->clientPublicKey = gen.extractPublicKeyPEM(key);

  QString bearer = "Bearer ";
  QString tk = testToken;
  QString token = bearer + tk;
  // qDebug()<<"debug:: latin " <<token.toLatin1();
  request.setRawHeader("Authorization", token.toLatin1());
  request.setRawHeader("D", getMotherboardSerialNumber().toLatin1());
  request.setRawHeader("S", "WC");
  request.setRawHeader("V", appVersion.toLatin1());
  request.setRawHeader("Accept", "application/json; version=1.0");
  request.setSslConfiguration(getSslConfig());
  //
  QJsonObject json;
  json["course_id"] = courseId;
  json["p"] = QString::fromStdString(clientPublicKey.toHex().toStdString());
   qDebug()<<"debug:: couseId" << courseId;
  json["video_id"] = videoId;
  json["course_item_id"] = courseItemId;
  qDebug()<<"debug:: videoId" << videoId;
  QJsonDocument jsonDoc(json);

  qDebug()<<jsonDoc.toJson();
  // Convert QJsonDocument to QByteArray
  QByteArray postData = jsonDoc.toJson();

  manager->post(request, postData);
}

void ApiManager::sendErrorInfo(QJsonDocument errorData, QString url) {
  try {
    QString errorCode = errorData.object().value("code").toString();
    QJsonArray errorMessage =
        errorData.object().value("error_description").toArray();

    QNetworkRequest request(QUrl(url + "/api/record-error/"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    key = gen.generateECKeyPair();
    this->clientPublicKey = gen.extractPublicKeyPEM(key);

    QString bearer = "Bearer ";
    QString bearerToken = bearer + this->token;
    // qDebug()<<"debug:: latin " <<token.toLatin1();
    request.setRawHeader("Authorization", bearerToken.toLatin1());
    request.setRawHeader("D", getMotherboardSerialNumber().toLatin1());
    request.setRawHeader("S", "WC");
    request.setRawHeader("V", appVersion.toLatin1());
    request.setRawHeader("Accept", "application/json; version=1.0");
    //
    request.setSslConfiguration(getSslConfig());
    QJsonObject json;
    json["mobile"] = this->credentials;
    json["description"] = errorMessage.at(0).toString();
    json["error"] = errorCode;
    json["data"] = url;
    QJsonDocument jsonDoc(json);

    // Convert QJsonDocument to QByteArray
    QByteArray postData = jsonDoc.toJson();
    manager->post(request, postData);
  } catch (const std::exception& e) {
  }
}

QByteArray ApiManager::getPublicKeySha256(const QSslCertificate &cert)
{
    // Extract the public key in DER (ASN.1) format
    QSslKey pubKey = cert.publicKey();
    QByteArray spki = pubKey.toDer();  // DER = SubjectPublicKeyInfo structure

    // Hash using SHA-256
    QByteArray hash = QCryptographicHash::hash(spki, QCryptographicHash::Sha256);

    return hash;
}


void ApiManager::onSubmitUserInfo(QNetworkReply* reply) {
  // qDebug()<<"in submitUserInfo";
  // qDebug()<<"in submitUserInfo data" << reply->url().toString();
  int statusCode =
      reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

  const QSslCertificate serverCert =
      reply->sslConfiguration().peerCertificate();



 QString pinnedCert = loadPinnedCertFromBase64();

  qDebug()<<"server hex" << getPublicKeySha256(serverCert).toHex();
  if (getPublicKeySha256(serverCert).toHex() == pinnedCert) {
    reply->ignoreSslErrors();  // Accept if cert matches
  } else {
    // DailyLogger::instance().logger->info("Certificate mismatch. Aborting.") ;
    qWarning() << "Certificate mismatch. Aborting";
    emit noNetworkForTimer("No Internet connection.");
    return;
    // Do not call ignoreSslErrors() to block the request
  }

  if (reply->url().toString().contains("api/record-error")) {
    return;
  }

  if (reply->error() == QNetworkReply::NoError &&
      reply->url().toString().contains("api/log-activity")) {
    QByteArray response = reply->readAll();
    QJsonDocument document = QJsonDocument::fromJson(response, nullptr);
    qDebug() << "time send response" << document.object();
    this->requestId = "";
    if (document.object().contains("session_id")  && document.object().value("session_id").toString() != "") {
      sessionId = document.object().value("session_id").toString();
      return;
    }
    emit onUserTimeSentSuccess();
  } else {
    QByteArray response = reply->readAll();
    QJsonDocument document = QJsonDocument::fromJson(response, nullptr);
    // qWarning() <<"error :" << document.object();
    if (reply->error() != QNetworkReply::NoError && statusCode == 412) {
      QString errorMessage = handlePreconditionFailed(document);
      emit noNetworkForTimer(errorMessage);
    } else if (statusCode == 401) {
      emit noNetworkForTimer("Your session has expired.\nPlease log in again.");
    } else if (statusCode == 500) {
      sendErrorInfo(document, reply->url().toString());
      emit noNetworkForTimer("Server error" + QString::number(statusCode) +
                             ".\nPlease contact support.");
    } else if (statusCode == 443 || statusCode == 0) {
      qWarning() << "Error:" << reply->errorString();
      // qDebug() << "No network error from API manager 'onSubmitUserInfo'";
      emit noNetworkForTimer("No Internet connection.");
    } else {
      qWarning() << "Error:" << reply->errorString();
      // qDebug() << "No network error from API manager 'onSubmitUserInfo'";
      emit noNetworkForTimer("Server error" + QString::number(statusCode) +
                             ".\nPlease contact support.");
    }
  }
  reply->deleteLater();
}

QString ApiManager::handlePreconditionFailed(QJsonDocument errorData) {
  QString errorCode = errorData.object().value("code").toString();
  QJsonArray errorMessage =
      errorData.object().value("error_description").toArray();
  QString message =
      errorMessage.count() > 0
          ? "Error Code : " + errorCode + "\n" + errorMessage.at(0).toString()
          : "Error Code :" + errorCode + "\nPlease contact support";
  return message;
}

QString ApiManager::getVideoMetadata(QString spk, QString iv, QString text) {
  // EncryptionHandler  handler;
  TextDecryptor decryptor;
  QByteArray pmkey = QByteArray::fromHex(spk.toUtf8());
  // qDebug()<<"pem key" << pmkey.toStdString();

  EC_KEY* serverKey = gen.loadServerPublicKey(pmkey);
  QByteArray unCompressed = gen.publicKeyToUncompressedHex(serverKey);

  // qDebug()<<"uncompressed key" << unCompressed;

  const EC_GROUP* group = EC_KEY_get0_group(key);
  EC_POINT* server_point =
      gen.hexToECPoint(unCompressed, const_cast<EC_GROUP*>(group));
  /////////

  QByteArray sharedSecretArray = gen.deriveSharedSecret(key, server_point);
  // qDebug() << "Shared secret derived successfully on client" <<
  // sharedSecretArray.toHex();

  QByteArray info = "handshake data";  // Info for HKDF
  size_t keyLength = 32;               // Desired key ength (e.g., 256 bits)

  QByteArray derivedKeyValue;
  QByteArray salt;
  gen.hkdf(salt, sharedSecretArray, info, derivedKeyValue, keyLength);

  // qDebug() << "Derived Key:" << derivedKeyValue.toHex();
  return decryptor.decryptText(derivedKeyValue.toHex(), iv, text);
}

QJsonDocument ApiManager::jsonStringToDocument(const QString& jsonString) {
  // Initialize a QJsonParseError to capture any parsing errors
  QJsonParseError parseError;

  // Convert the JSON string to a QJsonDocument
  QJsonDocument jsonDoc =
      QJsonDocument::fromJson(jsonString.toUtf8(), &parseError);

  // Check for parsing errors
  if (parseError.error != QJsonParseError::NoError) {
    qWarning() << "Failed to parse JSON string:" << parseError.errorString();
    return QJsonDocument();  // Return an empty QJsonDocument if parsing fails
  }

  return jsonDoc;
}

void ApiManager::onFinished(QNetworkReply* reply) {
  int statusCode =
      reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

  const QSslCertificate serverCert =
      reply->sslConfiguration().peerCertificate();

  QString pinnedCert = loadPinnedCertFromBase64();
  qDebug()<< "server hex " <<  getPublicKeySha256(serverCert).toHex();
  if (getPublicKeySha256(serverCert).toHex() == pinnedCert) {
    reply->ignoreSslErrors();  // Accept if cert matches
  } else {
    // DailyLogger::instance().logger->info("Certificate mismatch. Aborting.") ;
    qWarning() << "Certificate mismatch. Aborting";
    return;
    // Do not call ignoreSslErrors() to block the request
  }

  if (reply->url().toString().contains("api/record-error")) {
    return;
  }

  if (reply->error() == QNetworkReply::NoError &&
      reply->url().toString().contains("api/generate-video-metadata")) {
    QByteArray responseData = reply->readAll();
    QJsonDocument document = QJsonDocument::fromJson(responseData, nullptr);

    QString metadata = document.object().value("metadata").toString();
    QString spk = document.object().value("spk").toString();
    QString iv = document.object().value("iv").toString();
    QString md = this->getVideoMetadata(spk, iv, metadata);

    QJsonDocument jmd = jsonStringToDocument(md);
    QJsonValue jmv = jmd.object().value("metadata");
    QList<VideoData> resultOfParsing = parseVideoData(jmv.toString());

    // Watermark config generation
    MizuConfig* config = new MizuConfig();
    config->log_activity_interval =
        jmd.object().value("log_activity_interval").toInt();

    // qWarning() << "json---------";
    // qWarning() << jmd.toJson();
    config->detect_external_display =
        jmd.object().value("detect_external_display").toBool(true);

    // come back here
    if (config->log_activity_interval == 0) {
      config->log_activity_interval = 60;
    }
    config->resume_at_seconds = jmd.object().value("resume_at_seconds").toInt();

    qDebug() << "----- log" << config->log_activity_interval << "log interval";

    // Small watermark (sm_wm) config
    config->smConfig = new SmMizu();
    qDebug() << "sm new ---- " << jmd.object().value("sm_wm").toString();
    auto smConfig = jmd.object().value("sm_wm").toObject();
    config->smConfig->font_size = smConfig.value("font_size").toInt();
    config->smConfig->transparent_start =
        smConfig.value("transparency_start").toInt();
    config->smConfig->transparent_end =
        smConfig.value("transparency_end").toInt();
    config->smConfig->count_per_frame =
        smConfig.value("count_per_frame").toInt();
    config->smConfig->interval = smConfig.value("interval").toInt();

    // Convert "colors" array to QStringList
    QJsonArray colorsArray = smConfig.value("colors").toArray();
    for (const QJsonValue& colorValue : colorsArray) {
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
    for (const QJsonValue& transparencyValue : transparencyArray) {
      config->flConfig->transparency.append(transparencyValue.toInt());
    }

    config->dflConfig = new DFlashMizu();
    auto dflConfig = jmd.object().value("d_flash_wm").toObject();
    config->dflConfig->chn_duration = dflConfig.value("chn_duration").toInt();
    config->dflConfig->color = dflConfig.value("color").toString();
    QJsonArray dTransparencyArray = dflConfig.value("transparency").toArray();
    for (const QJsonValue& transparencyValue : dTransparencyArray) {
      config->dflConfig->transparency.append(transparencyValue.toInt());
    }

    // qDebug() << "lgConfig" <<
    // QJsonDocument(lgConfig).toJson(QJsonDocument::Compact); qDebug() <<
    // "smConfig" << QJsonDocument(smConfig).toJson(QJsonDocument::Compact);
    emit onKeyFetchFinished(resultOfParsing, config,
                            jmd.object().value("duration").toInt());
  } else if (reply->error() != QNetworkReply::NoError &&
             reply->url().toString().contains("api/generate-video-metadata/")) {
    QByteArray response = reply->readAll();
    QJsonDocument document = QJsonDocument::fromJson(response, nullptr);
    // qWarning()<<document.object();
    if (statusCode == 412) {
      QString errorMessage = handlePreconditionFailed(document);
      emit noNetwork(errorMessage);
    } else if (statusCode == 401) {
      emit noNetwork("Your session has expired.\nPlease log in again.");
    } else if (statusCode == 500) {
      sendErrorInfo(document, reply->url().toString());
      emit noNetwork("Server Error " + QString::number(statusCode) +
                     ".\nPlease contact support\n at support@vidsafe.in");
    } else if (statusCode == 443 || statusCode == 0) {
      emit noNetworkForTimer("No Internet connection.");
    } else {
      qWarning() << "Error:" << reply->errorString();
      QString message = handlePreconditionFailed(document);
      qDebug() << "Netowrk error from API manager 'onSubmitUserInfo'"
               << statusCode << "error message" << message;

      emit noNetwork("Server Error " + QString::number(statusCode) +
                     ".\nPlease contact support");
      // emit noNetwork("No Internet connection");
    }
  } else {
    qWarning() << "Error:" << reply->errorString() << " " << statusCode;
    emit noNetwork("Server Error " + QString::number(statusCode) +
                   ".\nPlease contact support");
  }
  reply->deleteLater();
}

void ApiManager::sendUserWatchTime(QString token, QString courseId,
                                   QString courseItemId, QString videoId,
                                   qint64 playbackTime,
                                   qint64 currentPosition) {
  QNetworkRequest request(QUrl(url + "/api/log-activity/"));
  if (this->requestId.length() == 0) {
    this->requestId = QString::fromStdString(bsonObjectGenerator->generate());
  }  //

  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
  QString bearer = "Bearer ";
  QString tk = token;
  QString tokenString = bearer + tk;
  // qDebug()<<"debug:: latin " <<tokenString.toLatin1();
  qDebug() << "sesion id ----" << sessionId;
  request.setRawHeader("Authorization", tokenString.toLatin1());
  request.setRawHeader("D", getMotherboardSerialNumber().toLatin1());
  request.setRawHeader("S", "WC");
  request.setRawHeader("V", appVersion.toLatin1());
  request.setRawHeader("Accept", "application/json; version=1.0");
  request.setSslConfiguration(getSslConfig());
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

  qDebug() << "--------------------------------------------";
  // Create and format the activity datetime
  QDateTime dateTime = QDateTime::currentDateTime();  // Replace with the actual
                                                      // datetime if needed
  QString formattedDateTime = dateTime.toString("yyyy-MM-dd HH:mm:ss");
  jsonObject["activity_datetime"] = formattedDateTime;
  QJsonDocument jsonDoc(jsonObject);
  // Convert QJsonDocument to QByteArray
  QByteArray postData = jsonDoc.toJson();
  qDebug() << "sending user watch time";
  userInfoLoggerManager->post(request, postData);
}

QList<VideoData> ApiManager::parseVideoData(const QString& data) {
  QList<VideoData> resultList;

  // Remove the surrounding quotes and parse as a single string
  QString trimmedData =
      data.mid(1, data.length() - 2);  // Remove leading "[\"", trailing "\"]"
  // qDebug()<<trimmedData;
  QStringList segments = trimmedData.split(",");

  // qDebug()<<"start data parsing == "<<segments;
  for (const QString& segment : segments) {
    QStringList parts = segment.trimmed().remove("'").split("|");
    // qDebug()<< segments << "--segment--";
    if (parts.size() == 4) {
      auto tempDurationFirstPart =
          parts[0].trimmed().split(":").first().toStdString();
      auto tempDurationSecondPart =
          parts[0].trimmed().split(":").last().toStdString();
      int durationNumber = std::__cxx11::stoi(tempDurationSecondPart) -
                           std::__cxx11::stoi(tempDurationFirstPart);
      QString duration = QString::number(durationNumber);
      QString key = parts[1].remove("\\x").remove("'").trimmed();
      QString iv = parts[2].remove("\\x").remove("'").trimmed();
      QString fileName = parts[3].split("/").last().trimmed();

      VideoData videoData = {duration, key, iv, fileName};
      resultList.append(videoData);
    } else {
      // qDebug() << "Invalid segment format:" <<  parts.size();
    }
  }
  // qDebug()<<"end data parsing";

  return resultList;
}

void ApiManager::getSessionId(QString token, QString courseId,
                              QString courseItemId, QString videoId) {
  QNetworkRequest request(QUrl(url + "/api/log-activity/"));
  //
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
  QString bearer = "Bearer ";
  QString tk = token;
  QString tokenString = bearer + tk;
  // qDebug()<<"debug:: latin " <<tokenString.toLatin1();
  request.setRawHeader("Authorization", tokenString.toLatin1());
  request.setRawHeader("D", getMotherboardSerialNumber().toLatin1());
  request.setRawHeader("S", "WC");
  request.setRawHeader("V", appVersion.toLatin1());
  request.setRawHeader("Accept", "application/json; version=1.0");
  request.setSslConfiguration(getSslConfig());
  QJsonObject jsonObject;

  // Assign values to the QJsonObject
  jsonObject["activity_type"] = "VIDEO_OPEN";
  jsonObject["course_id"] = courseId;
  jsonObject["course_item_id"] = courseItemId;
  jsonObject["video_id"] = videoId;

  qDebug() << "--------------------------------------------";
  // Create and format the activity datetime
  QDateTime dateTime = QDateTime::currentDateTime();  // Replace with the actual
                                                      // datetime if needed
  QString formattedDateTime = dateTime.toString("yyyy-MM-dd HH:mm:ss");
  jsonObject["activity_datetime"] = formattedDateTime;
  QJsonDocument jsonDoc(jsonObject);
  // Convert QJsonDocument to QByteArray
  QByteArray postData = jsonDoc.toJson();
  userInfoLoggerManager->post(request, postData);
}

QString ApiManager::loadPinnedCertFromBase64() {
  // Decode the Base64 string into raw binary data
  // QByteArray certBytes =
  //     QByteArray::fromBase64(QString::fromUtf8(public_pin_cert).toUtf8());
  // // Create a certificate from the decoded bytes
  // QSslCertificate cert(certBytes,
  //                      QSsl::Der);  // Make sure you're using DER format
  // if (cert.isNull()) {
  //   qWarning() << "Failed to load certificate from base64";
  // }
  // return cert;

    QByteArray hash  = QByteArray::fromRawData(reinterpret_cast<const char*>(public_pin_cert_hash),sizeof(public_pin_cert_hash));
    QString hex = hash.toHex();

   qDebug()<<hex << " this is hex";

  return hex;
}
