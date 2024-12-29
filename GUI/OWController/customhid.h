#ifndef CUSTOMHID_H
#define CUSTOMHID_H

#include "hidapi/hidapi.h"
#include <QObject>

class CustomHid : public QObject
{
    Q_OBJECT

public:
    CustomHid(unsigned short vid = 0,
              unsigned short pid = 0);

    ~CustomHid();

    bool Connect(const QString &product_string);
    void Disconnect();
    void setVid(unsigned short vid);
    void setPid(unsigned short pid);
    int Read(unsigned char *buff, size_t len);
    int Write(unsigned char *buff, size_t len, bool feat_mode);
    unsigned short getVid();
    unsigned short getPid();
    wchar_t* getProductString();
    wchar_t* getManufacturerString();
    wchar_t* getSerialNumber();

private:
    static const unsigned short MAX_STR = 255;

    unsigned short VID, PID;
    wchar_t wstr[MAX_STR];

    hid_device *deviceHandle = nullptr;
    hid_device_info *deviceInfo = nullptr;

    int opResult;
};
#endif // CUSTOMHID_H
