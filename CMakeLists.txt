cmake_minimum_required(VERSION 3.5)

project(video_player_testing VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

FIND_PACKAGE( Qt6Multimedia REQUIRED )
FIND_PACKAGE( Qt6MultimediaWidgets REQUIRED )
find_package(Qt6UiTools REQUIRED)
SET(QT_USE_QTMULTIMEDIA TRUE)
SET(QT_USE_QTMULTIMEDIAWIDGETS TRUE)
find_package(Qt6 REQUIRED COMPONENTS Multimedia)
find_package(Qt6 REQUIRED COMPONENTS MultimediaWidgets)
find_package(Qt6 REQUIRED COMPONENTS UiTools )

add_library(cryptopp STATIC IMPORTED)
#set_target_properties(LibCrypto PROPERTIES IMPORTED_LOCATION "libs/lib")
set_target_properties( # Specifies the target library.
                       cryptopp

                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION

                       # Provides the path to the library you want to import.
                       ${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libcryptopp.a )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/include/cryptopp/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})


#QT6_USE_MODULES (${EXECUTABLE_NAME} Multimedia MultimediaWidgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(video_player_testing
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        videoprogressbarcontroller.h videoprogressbarcontroller.cpp
        encryptionhandler.h encryptionhandler.cpp
        NewDecription.h
        circularbutton.h circularbutton.cpp
        resources.qrc
        controls_overlay.ui
        fullscreencontrolhoverhandler.h fullscreencontrolhoverhandler.cpp
        playbackratehandler.h playbackratehandler.cpp
        windoweventhandler.h windoweventhandler.cpp
        CustomGraphicsView.h
        CustomGraphicsView.cpp

    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET video_player_testing APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(video_player_testing SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(video_player_testing
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(video_player_testing PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(video_player_testing PRIVATE Qt${QT_VERSION_MAJOR}::Multimedia)
target_link_libraries(video_player_testing PRIVATE Qt${QT_VERSION_MAJOR}::MultimediaWidgets)
target_link_libraries(video_player_testing PRIVATE Qt${QT_VERSION_MAJOR}::UiTools)
target_link_libraries(video_player_testing PRIVATE cryptopp)
# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.6.3)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.video_player_testing)
endif()
set_target_properties(video_player_testing PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS video_player_testing
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(video_player_testing)
endif()
