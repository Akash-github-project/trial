cmake_minimum_required(VERSION 3.5)

project(VidSafePlayer VERSION 0.1 LANGUAGES CXX)
#cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
#find_package(ZLIB REQUIRED)

FIND_PACKAGE( Qt6Multimedia REQUIRED )
FIND_PACKAGE( Qt6MultimediaWidgets REQUIRED )
find_package(Qt6UiTools REQUIRED)
SET(QT_USE_QTMULTIMEDIA TRUE)
SET(QT_USE_QTMULTIMEDIAWIDGETS TRUE)
find_package(Qt6 REQUIRED COMPONENTS Multimedia)
find_package(Qt6 REQUIRED COMPONENTS MultimediaWidgets)
find_package(Qt6 REQUIRED COMPONENTS UiTools )
find_package(Qt6 REQUIRED COMPONENTS Network )
find_package(Qt6 REQUIRED COMPONENTS Quick )


add_library(cryptopp STATIC IMPORTED)
add_library(openssl STATIC IMPORTED)
add_library(crypto STATIC IMPORTED)
add_library(libz STATIC IMPORTED)
add_library(libSoundTouch STATIC IMPORTED)
#set_target_properties(LibCrypto PROPERTIES IMPORTED_LOCATION "libs/lib")
set_target_properties( # Specifies the target library.
                       cryptopp
                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION
                       # Provides the path to the library you want to import.
                       ${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libcryptopp.a )
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/include/cryptopp/)

set_target_properties( # Specifies the target library.
                       libz
                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION
                       # Provides the path to the library you want to import.
                       ${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libz.a )
#openssl
set_target_properties( # Specifies the target library.
                       openssl
                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION
                       # Provides the path to the library you want to import.
                       ${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libssl.a )

set_target_properties( # Specifies the target library.
                        crypto
                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION
                       # Provides the path to the library you want to import.
                       ${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libcrypto.a)

set_target_properties( # Specifies the target library.
                        libSoundTouch
                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION
                       # Provides the path to the library you want to import.
                       ${CMAKE_CURRENT_SOURCE_DIR}/libs/lib/libSoundTouch.a)

#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/include/openssl/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/include/)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})


#QT6_USE_MODULES (${EXECUTABLE_NAME} Multimedia MultimediaWidgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(VidSafePlayer
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
        aes_crypto.h aes_crypto.cpp
        seekbarprogresscontroller.h seekbarprogresscontroller.cpp
        reddotrecording.h reddotrecording.cpp
        screenflashlayer.h screenflashlayer.cpp
        patternemitter.h patternemitter.cpp
        loginpage.h loginpage.cpp loginpage.ui
        actionnavigationbutton.h actionnavigationbutton.cpp
        clickableqframe.h clickableqframe.cpp
        loginotpscreen.h loginotpscreen.cpp loginotpscreen.ui
        onboardingwrapper.h onboardingwrapper.cpp onboardingwrapper.ui
        registerdeviceconfirmation.h registerdeviceconfirmation.cpp registerdeviceconfirmation.ui
        mainhomescreen.h mainhomescreen.cpp mainhomescreen.ui
        apimanager.h apimanager.cpp
        piracy_warning_modal.ui
        warningdialog.h warningdialog.cpp warningdialog.ui
        customseekbar.h customseekbar.cpp
        playercontrollerwidget.h playercontrollerwidget.cpp
        customcontrolswidget.h customcontrolswidget.cpp
        nointernetdialog.h nointernetdialog.cpp nointernetdialog.ui
        userplaybacktimertracker.h userplaybacktimertracker.cpp
        screendetector.h screendetector.cpp
        customuiloader.h customuiloader.cpp
        mizushirushihandora.h mizushirushihandora.cpp
        sharekeygenerator.h sharekeygenerator.cpp
        textdecryptor.h textdecryptor.cpp
        bsonobjectid.h bsonobjectid.cpp
        customgraphicstextitem.h customgraphicstextitem.cpp
        customaudiodevice.h customaudiodevice.cpp

        # securememory.h securememory.cpp

        #fullscreenviews.h fullscreenviews.cpp

    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET video_player_testing APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(VidSafePlayer SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(VidSafePlayer
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(VidSafePlayer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(VidSafePlayer PRIVATE Qt${QT_VERSION_MAJOR}::Multimedia)
target_link_libraries(VidSafePlayer PRIVATE Qt${QT_VERSION_MAJOR}::MultimediaWidgets)
target_link_libraries(VidSafePlayer PRIVATE Qt${QT_VERSION_MAJOR}::Network)
target_link_libraries(VidSafePlayer PRIVATE Qt${QT_VERSION_MAJOR}::UiTools)
target_link_libraries(VidSafePlayer PRIVATE Qt${QT_VERSION_MAJOR}::Quick)
target_link_libraries(VidSafePlayer PRIVATE cryptopp)
target_link_libraries(VidSafePlayer PRIVATE openssl)
target_link_libraries(VidSafePlayer PRIVATE crypto)
target_link_libraries(VidSafePlayer PRIVATE libz)
target_link_libraries(VidSafePlayer PRIVATE libSoundTouch)
#add_definitions(/EHsc)  # Optional for exception handling

if(WIN32)
  include_directories(<Windows.h>)  # Include for SetWindowDisplayAffinity
  target_link_libraries(VidSafePlayer PRIVATE setupapi)
  target_link_libraries(VidSafePlayer PRIVATE user32)
  target_link_libraries(VidSafePlayer PRIVATE wbemuuid)
  target_link_libraries(VidSafePlayer PRIVATE dxgi)
  target_link_libraries(VidSafePlayer PRIVATE d3d11)
  target_link_libraries(VidSafePlayer PRIVATE cfgmgr32)

endif(WIN32)
# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.6.3)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.player.vidsafeplayer)
endif()


set_target_properties(VidSafePlayer PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    OUTPUT_NAME "VajiramVidsafePlayer"
)

include(GNUInstallDirs)
install(TARGETS VidSafePlayer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(VidSafePlayer)
endif()
