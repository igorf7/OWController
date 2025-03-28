#ifndef CUSTOMHID_H
#define CUSTOMHID_H

#include "hidapi/hidapi.h"
#include "hidapi/libusb.h"
#include "hidapi/hidapi_libusb.h"
#include <QObject>

class CustomHid : public QObject
{
    Q_OBJECT

public:
    CustomHid(unsigned short vid = 0,
              unsigned short pid = 0);

    ~CustomHid();

    void Connect();
    void Disconnect();
    int Read(unsigned char *buff, size_t len);
    int Write(unsigned char *buff, size_t len);
    int getFeatureReport(unsigned char *buff, size_t len);
    int sendFeatureReport(unsigned char *buff, size_t len);

signals:
    void showStatusBar(const QString &str, int timeout);
    void deviceConnected();
    void deviceDisconnected();

private:
    const QString ProductString = "OW Controller";
    static const unsigned short MAX_STR = 255;

    unsigned short VID, PID;
    wchar_t wstr[MAX_STR];

    hid_device *deviceHandle = NULL;

    int findUsbDevice();
    void closeHidDevice();
};
#endif // CUSTOMHID_H
