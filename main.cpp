#include "mainwindow.h"
#include "loginpage.h"
#include "loginotpscreen.h"
#include "onboardingwrapper.h"

#include <QApplication>
#include <QTimer>
#include <QString>

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




int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // if (argc != 3)
    // {
    //     std::cerr << "Usage: " << argv[0] << " <folder_path>" << std::endl;
    //     return 1;
    // }

    // std::string folderPath = argv[1];

    // if (!isValidPath(folderPath))
    // {
    //     std::cerr << "Invalid or inaccessible folder path." << std::endl;
    //     return 1;
    // }

    // // If the path is valid, proceed with further operations
    // std::cout << "Folder path is valid: " << folderPath << std::endl;

    // // Example operation: Listing the contents of the directory
    // try
    // {
    //     for (const auto& entry : fs::directory_iterator(folderPath))
    //     {
    //         std::cout << entry.path() << std::endl;
    //     }
    // }
    // catch (const fs::filesystem_error& e)
    // {
    //     std::cerr << "Filesystem error during directory iteration: " << e.what() << std::endl;
    //     return 1;
    // }
    // catch (const std::exception& e)
    // {
    //     std::cerr << "General error during directory iteration: " << e.what() << std::endl;
    //     return 1;
    // }

    // const QString token = argv[2];
    // const QString course_id = argv[3];
    // const QString video_id = argv[4];


    const QString token = "L0ttEO2cTeo5nyojuXVe3LxH9kNSkK";
    const QString course_id = "66863639da8f59703445d9f2";
    const QString video_id = "6686845dda8f5976e0d41568";


     //QString filePath = QString::fromStdString(folderPath);
    QString filePath = "C:\\Users\\BharatCaller\\AppData\\Local\\Packages\\com.companyname.vidsafeproject_9zz4h110yvjzm\\LocalState\\VidSafeExtracted\\66868469da8f5976e34f87bc";

    MainWindow w(filePath,token,course_id,video_id,nullptr);
    w.show();
    return a.exec();
}


