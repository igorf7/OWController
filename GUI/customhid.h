#ifndef CUSTOMHID_H
#define CUSTOMHID_H

#include "hid/inc/hidapi.h"
#include <QObject>

class CustomHid : public QObject
{
    Q_OBJECT

public:
    CustomHid(unsigned short vid = 0,
              unsigned short pid = 0);

    ~CustomHid();

    bool Connect();
    void Disconnect();
    void readFromDevice(QByteArray &rx_data);
    bool writeToDevice(const QByteArray &tx_data);

signals:
    void deviceConnected();
    void deviceDisconnected();

private:
    const QString ProductString = "OW Controller";
    static const int BUFF_SIZE = 64;
    static const unsigned short MAX_STR = 255;

    unsigned short VID, PID;
    wchar_t wstr[MAX_STR];

    hid_device *deviceHandle = NULL;

    void closeHidDevice();
};
#endif // CUSTOMHID_H
