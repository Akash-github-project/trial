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


//#define LOCAL true;

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
    QString homeDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);

    // Define your log file name
    QString logFileName = homeDir + "/vidsafe.log";
    static QFile logFile(logFileName);
    if (!logFile.isOpen()) {
        logFile.open(QIODevice::Append | QIODevice::Text);
    }

    QTextStream out(&logFile);
    out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz ") << " ";

    switch (type) {
    case QtDebugMsg:
        out << "DEBUG: ";
        break;
    case QtInfoMsg:
        out << "INFO: ";
        break;
    case QtWarningMsg:
        out << "WARNING: ";
        break;
    case QtCriticalMsg:
        out << "CRITICAL: ";
        break;
    case QtFatalMsg:
        out << "FATAL: ";
        break;
    }
    out << msg << Qt::endl;
}

QString accessNamedPipes(){
    QFile pipe("\\\\.\\pipe\\VideoPlayerStream");
    if (!pipe.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open pipe.";
        return QString("");
    }

    QTextStream stream(&pipe);
    QString message = stream.readLine();
    qDebug() << "Message from pipe:" << message;
    return message;
}



int main(int argc, char *argv[])
{
    //qInstallMessageHandler(customMessageHandler);
    QApplication a(argc, argv);

    qDebug()<<"starting the application";

  #ifndef LOCAL
    QString returnValue = accessNamedPipes();
    QStringList listOfArgs = returnValue.split("#");

    if(listOfArgs.length() != 7) {
        qDebug()<<"immproper number of arguments";
        return 1;
    }

    //qDebug()<<argv[1] << "," <<argv[2] << ", " << argv[3] << "," << argv[4] ;
    std::string folderPath = listOfArgs[0].toStdString();

    if (!isValidPath(folderPath))
    {
        qDebug() << "Invalid or inaccessible folder path." << "count --" << argc;
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
        qDebug() << "Filesystem error during directory iteration: " << e.what() ;
        return 1;
    }
    catch (const std::exception& e)
    {
        qDebug() << "General error during directory iteration: " << e.what();
        return 1;
    }

    const QString video_id = listOfArgs[1];
    const QString token = listOfArgs[2];
    const QString course_id = listOfArgs[3];
    const QString identifier = listOfArgs[4];
    const QString video_item_id = listOfArgs[5];
    const QString deviceId = listOfArgs[6];

    // qDebug()<<"video_id " << listOfArgs[1];
    // qDebug()<<"token " << listOfArgs[2];
    // qDebug()<<"course_id " << listOfArgs[3];
    // qDebug()<<"identifier " << listOfArgs[4];
    // qDebug()<<"video_item_id " << listOfArgs[5];
    // qDebug()<<"deviceId " << listOfArgs[6];


    QString filePath = QString::fromStdString(folderPath);
  #endif

  #ifdef LOCAL
     const QString token = "TRhIZOqfVNqcslht9uRFr6PozcqlnL";
     const QString course_id = "6739d7a9da8f59008ee76fb1";
     const QString video_item_id = "6739d829da8f590090f6e1ed";
     const QString deviceId = "/98BZXN2/CNWSC00891007A/";

     const QString video_id = "6739d829da8f590090f6e1ec";
     QString filePath = "C:\\Users\\BharatCaller\\AppData\\Local\\VidSafe\\in.vidsafe.vajiram.test\\Data\\VidSafeExtracted\\8709031440\\6739d7a9da8f59008ee76fb1\\6739d829da8f590090f6e1ed.zip";
     const QString identifier = "8709031440";
  #endif

    //std::setenv("QT_MULTIMEDIA_PREFERRED_PLUGINS", "windowsmediafoundation", 1);
    MainWindow w(filePath,token,course_id,video_id,video_item_id,identifier,deviceId,nullptr);
    w.show();
    return a.exec();
}


