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

    bool Connect();
    void Disconnect();
    void readFromDevice(QByteArray &array);
    bool writeToDevice(const QByteArray &data);

signals:
    void finished();

private:
    static const int RX_DATA_SIZE = 64;

    unsigned short VID, PID;

    void closeHidDevice();
    void initUsbBridge();
    bool openHidDevice(int vid, int pid);
};
#endif // JNILAYER_H
