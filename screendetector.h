#ifndef SCREENDETECTOR_H
#define SCREENDETECTOR_H

#include <windows.h>
#include <QObject>
#include <iostream>
#include <QDebug>
#include <QString>
#include <setupapi.h>
#include <cfgmgr32.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "cfgmgr32.lib")

class ScreenDetector : public QObject
{
    Q_OBJECT
public:
    // struct DisplayInfo {
    //     QString adapterName;
    //     QString outputName;
    //     QString description;
    //     bool isPhysical;
    // };
    explicit ScreenDetector(QObject *parent = nullptr);
    ~ScreenDetector();
    // IDXGIFactory* m_pFactory;
    // QList<IDXGIAdapter*> m_adapters;
    // QList<IDXGIOutput*> m_outputs;
    // QList<DisplayInfo> m_displays;


    int checkDisiplays();
    // void enumerateDisplays();
    int getMonitorInfoFromDeviceManager();
signals:
};

#endif // SCREENDETECTOR_H
