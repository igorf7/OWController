#ifndef JNILAYER_H
#define JNILAYER_H

#include <QObject>

class JniLayer : public QObject
{
    Q_OBJECT

public:
    JniLayer(unsigned short vid = 0,
			unsigned short pid = 0);

    ~JniLayer();

    void Connect();
    void Disconnect();
    void readFromDevice(QByteArray &array);
    bool writeToDevice(const QByteArray &data);

signals:
    void deviceConnected();
    void deviceDisconnected();

private:
    static const int RX_DATA_SIZE = 64;

    unsigned short VID, PID;

    void closeHidDevice();
    void initUsbBridge();
    bool openHidDevice(int vid, int pid);
};
#endif // JNILAYER_H
