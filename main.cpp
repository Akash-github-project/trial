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
    // qInstallMessageHandler(customMessageHandler);
    QApplication a(argc, argv);

  #ifndef LOCAL
    QString returnValue = accessNamedPipes();
    QStringList listOfArgs = returnValue.split("#");

    if(listOfArgs.length() != 6) {
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
    QString filePath = QString::fromStdString(folderPath);
  #endif

  #ifdef LOCAL
     const QString token = "SU3MILNFFAn8tAni5EFwdRblvMnzSi";
     const QString course_id = "66863639da8f59703445d9f2d";

     const QString video_id = "6686845dda8f5976e0d41568";
     QString filePath = "C:/Users/BharatCaller/AppData/Roaming/VidSafe/com.companyname.vidsafeproject/Data/VidSafeExtracted/66868469da8f5976e34f87bc.zip";
     const QString identifier = "8709031440";
  #endif
    MainWindow w(filePath,token,course_id,video_id,video_item_id,identifier,nullptr);
    w.show();
    return a.exec();
}


