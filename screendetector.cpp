#include "screendetector.h"
// #define TEST 0

ScreenDetector::ScreenDetector(QObject *parent)
    : QObject{parent}
{ }

ScreenDetector::~ScreenDetector(){ }

int ScreenDetector::checkDisiplays(){

    UINT32 numPathArrayElements = 0;
    UINT32 numModeInfoArrayElements = 0;
    LONG result = GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &numPathArrayElements, &numModeInfoArrayElements);
    if (result != ERROR_SUCCESS)
    {
        qDebug() << "GetDisplayConfigBufferSizes failed with error:" << result;
        return 0;
    }

    std::vector<DISPLAYCONFIG_PATH_INFO> pathInfoArray(numPathArrayElements);
    std::vector<DISPLAYCONFIG_MODE_INFO> modeInfoArray(numModeInfoArrayElements);

    result = QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &numPathArrayElements, pathInfoArray.data(),
                                &numModeInfoArrayElements, modeInfoArray.data(), nullptr);
    if (result != ERROR_SUCCESS)
    {
        qDebug() << "QueryDisplayConfig failed with error:" << result;
        return 0;
    }

    for (UINT32 i = 0; i < numPathArrayElements; i++)
    {
        DISPLAYCONFIG_PATH_INFO& pathInfo = pathInfoArray[i];
        DISPLAYCONFIG_TARGET_DEVICE_NAME deviceName = {};
        deviceName.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
        deviceName.header.size = sizeof(deviceName);
        deviceName.header.adapterId = pathInfo.targetInfo.adapterId;
        deviceName.header.id = pathInfo.targetInfo.id;

        result = DisplayConfigGetDeviceInfo(&deviceName.header);
        if (result != ERROR_SUCCESS)
        {
            qDebug() << "DisplayConfigGetDeviceInfo failed with error:" << result;
            continue;
        }

        DISPLAYCONFIG_VIDEO_OUTPUT_TECHNOLOGY outputTech = pathInfo.targetInfo.outputTechnology;
        QString deviceFriendlyName = QString::fromWCharArray(deviceName.monitorFriendlyDeviceName);

        switch (outputTech)
        {
        case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_HDMI:
            qDebug() << "HDMI display connected:" << deviceFriendlyName;
            break;
        case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DISPLAYPORT_EXTERNAL:
            qDebug() << "DisplayPort (possibly USB-C) display connected:" << deviceFriendlyName;
            break;
        case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_MIRACAST:
            qDebug() << "Wireless (Miracast) display connected:" << deviceFriendlyName;
            break;
        // Handle other output technologies as needed
        default:
            qDebug() << "Other display connection detected:" << deviceFriendlyName;
            break;
        }
    }
    return 0;
}

int ScreenDetector::getMonitorInfoFromDeviceManager()
{
    int monitorCount = 0;
    const GUID GUID_MONITOR_CLASS = {0x4d36e96e, 0xe325, 0x11ce,{0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18}};
    HDEVINFO hDevInfo = SetupDiGetClassDevs(
        &GUID_MONITOR_CLASS,
        NULL,
        NULL,
        DIGCF_PRESENT
        );

    if (hDevInfo == INVALID_HANDLE_VALUE)
    {
        qDebug() << "SetupDiGetClassDevs failed.";
        return 0;
    }

    DWORD i = 0;
    SP_DEVINFO_DATA devInfoData;
    devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

    while (SetupDiEnumDeviceInfo(hDevInfo, i, &devInfoData))
    {
        TCHAR buffer[1024];
        DWORD requiredSize = 0;

        // Device Description
        if (SetupDiGetDeviceRegistryProperty(
                hDevInfo,
                &devInfoData,
                SPDRP_DEVICEDESC,
                NULL,
                (PBYTE)buffer,
                sizeof(buffer),
                &requiredSize))
        {
            QString deviceDescription = QString::fromWCharArray(buffer);
            qDebug() << "Device Description:" << deviceDescription;
        }

        // Hardware IDs
        if (SetupDiGetDeviceRegistryProperty(
                hDevInfo,
                &devInfoData,
                SPDRP_HARDWAREID,
                NULL,
                (PBYTE)buffer,
                sizeof(buffer),
                &requiredSize))
        {
            QString hardwareID = QString::fromWCharArray(buffer);
            qDebug() << "Hardware ID:" << hardwareID;
        }

        // Device Instance ID
        TCHAR instanceId[MAX_DEVICE_ID_LEN];
        if (CM_Get_Device_ID(devInfoData.DevInst, instanceId, MAX_DEVICE_ID_LEN, 0) == CR_SUCCESS)
        {
            QString deviceInstanceID = QString::fromWCharArray(instanceId);
            qDebug() << "Device Instance ID:" << deviceInstanceID;
            monitorCount += 1;
        }

        qDebug() << "----------------------------------------";

        i++;
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);
#ifdef TEST
    return 1;
#else
    return monitorCount;
#endif

}



