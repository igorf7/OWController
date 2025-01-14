#ifndef DEVICEWIDGET_H
#define DEVICEWIDGET_H

#include "cardview.h"

class DeviceWidget : public CardView
{
    Q_OBJECT

public:
    explicit DeviceWidget(CardView *parent = nullptr);
    ~DeviceWidget();

    virtual void showDeviceData(quint8 *data, int index) = 0;
    virtual void setAddress(quint64 address) = 0;
    virtual void setIndex(int index) = 0;

signals:
    void sendCommand(TOpcode opcode, quint8 *data, quint8 data_len);
};

#endif // DEVICEWIDGET_H
