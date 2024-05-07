#include "mainwindow.h"

#include <QApplication>
#include <QTimer>
#include <d3d11.h>


ID3D11Device* g_pd3dDevice = nullptr;
ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;

// DirectX initialization function
bool initializeDirectX()
{
    HRESULT hr = S_OK;

    // Create DXGI Factory
    IDXGIFactory* dxgiFactory = nullptr;
    hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&dxgiFactory);
    if (FAILED(hr))
    {
        qDebug() << "Failed to create DXGI Factory";
        return false;
    }

    // Enumerate adapters
    IDXGIAdapter* dxgiAdapter = nullptr;
    hr = dxgiFactory->EnumAdapters(0, &dxgiAdapter);
    if (FAILED(hr))
    {
        qDebug() << "Failed to enumerate adapters";
        dxgiFactory->Release();
        return false;
    }

    // Create device and device context
    hr = D3D11CreateDevice(
        dxgiAdapter,
        D3D_DRIVER_TYPE_UNKNOWN,
        nullptr,
        0,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &g_pd3dDevice,
        nullptr,
        &g_pd3dDeviceContext);

    dxgiAdapter->Release();
    dxgiFactory->Release();

    if (FAILED(hr))
    {
        qDebug() << "Failed to create DirectX 11 device";
        return false;
    }

    return true;
}

// Check device removal reason
void checkDeviceRemovedReason()
{
    if (g_pd3dDevice)
    {
        HRESULT hr = g_pd3dDevice->GetDeviceRemovedReason();
        if (hr == DXGI_ERROR_DEVICE_REMOVED)
        {
            qDebug() << "Device removed!";
            // Handle device removed scenario
            // e.g., recreate device, release resources, etc.
        }
    } else {
            qDebug() << "no reason!";
    }
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    /////////
     if (!initializeDirectX())
    {
        // DirectX initialization failed, handle error
        return -1;
    }

    // Create a timer to periodically check device removed reason
    QTimer timer;
    timer.setInterval(1000); // Check every second
    QObject::connect(&timer, &QTimer::timeout, [&]() {
        // Check device removed reason
        checkDeviceRemovedReason();
    });
    timer.start();
    ///
    return a.exec();

}
