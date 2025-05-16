#include "mainwindow.h"
#include "loginpage.h"
#include "loginotpscreen.h"
#include "onboardingwrapper.h"

#include <QApplication>
#include <QTimer>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QtMessageHandler>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <windows.h>

//#define LOCAL ;

namespace fs = std::filesystem;
bool isValidPath(const std::string& path)
{
    try
    {
        fs::path p(path);
        // Check if the path exists and is a directory
        return fs::exists(p) && fs::is_directory(p);
    }
    catch (const fs::filesystem_error& e)
    {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
        return false;
    }
    catch (const std::exception& e)
    {
        std::cerr << "General error: " << e.what() << std::endl;
        return false;
    }
}

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // Define your log file name
    // Ensure logs directory exists
    QString logsDirPath = QCoreApplication::applicationDirPath() + "/logs";
    QDir logsDir(logsDirPath);
    if (!logsDir.exists()) {
        logsDir.mkpath(".");
    }

    // Define your log file name with logs folder
    QString logFileName = logsDirPath + "/" + QDateTime::currentDateTime().toString("yyyy-MM-dd") + "-vidsafe.txt";
    static QFile logFile(logFileName);

    if (!logFile.isOpen()) {
        logFile.open(QIODevice::Append | QIODevice::Text);
    }

    QTextStream out(&logFile);
    out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ") << " ";

    switch (type) {
#ifdef LOCAL
    case QtDebugMsg:
        out << "DEBUG: "<< msg << Qt::endl;
        break;
    case QtInfoMsg:
        out << "INFO: "<< msg << Qt::endl;
        break;
    case QtWarningMsg:
        out << "WARNING: " << msg << Qt::endl;
        break;
    case QtCriticalMsg:
        out << "CRITICAL: " << msg << Qt::endl;
        break;
    }
#endif
#ifndef LOCAL
    case QtWarningMsg:
        out << "LOG: ";
        out << msg << Qt::endl;
        break;
    }
#endif
}

QString accessNamedPipes(){
    QFile pipe("\\\\.\\pipe\\VideoPlayerStream");
    if (!pipe.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open pipe.";
        return QString("");
    }

    QTextStream stream(&pipe);
    QString message = stream.readLine();
    return message;
}



int main(int argc, char *argv[])
{
    qInstallMessageHandler(customMessageHandler);
    QApplication a(argc, argv);
    qWarning()<<"starting the application";

  #ifndef LOCAL
    QString returnValue = accessNamedPipes();
    QStringList listOfArgs = returnValue.split("|#|#|");

    if(listOfArgs.length() != 7) {
        qWarning()<<"immproper number of arguments";
        return 1;
    }

    //qDebug()<<argv[1] << "," <<argv[2] << ", " << argv[3] << "," << argv[4] ;
    std::string folderPath = listOfArgs[0].toStdString();

    if (!isValidPath(folderPath))
    {
        qWarning() << "Invalid or inaccessible folder path." << "count --" << argc;
        return 1;
    }

    // If the path is valid, proceed with further operations
    qDebug() << "Folder path is valid: " << folderPath;

    // Example operation: Listing the contents of the directory
    try
    {
        for (const auto& entry : fs::directory_iterator(folderPath))
        {
            qDebug() << entry.path() ;
        }
    }
    catch (const fs::filesystem_error& e)
    {
        qWarning() << "Filesystem error during directory iteration: " << e.what() ;
        return 1;
    }
    catch (const std::exception& e)
    {
        qWarning() << "General error during directory iteration: " << e.what();
        return 1;
    }

    const QString video_id = listOfArgs[1];
    const QString token = listOfArgs[2];
    const QString course_id = listOfArgs[3];
    const QString identifier = listOfArgs[4];
    const QString video_item_id = listOfArgs[5];
    const QString deviceId = listOfArgs[6];

    QString filePath = QString::fromStdString(folderPath);
  #endif

  #ifdef LOCAL
     const QString token = "7BPOSiooLNHRpU6fpKefKtOXaWvlqR";
     const QString course_id = "6746cd03da8f59733bddcdbe";
     const QString video_item_id = "6746d254da8f597723f09a21";
     const QString deviceId = "/98BZXN2/CNWSC00891007A/";

     const QString video_id = "6746d254da8f597723f09a20";
     QString filePath = "C:\\Users\\BharatCaller\\AppData\\Local\\VidSafe\\in.vidsafe.vajiram.test\\Data\\VidSafeExtracted\\8709031440\\6746cd03da8f59733bddcdbe\\6746d254da8f597723f09a21.zip";
     const QString identifier = "8709031440";
  #endif

    qWarning()<<"starting the app";
    MainWindow w(filePath,token,course_id,video_id,video_item_id,identifier,deviceId,nullptr);
    w.show();
    return a.exec();
}


