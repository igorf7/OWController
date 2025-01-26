#ifndef DEVICEWIDGET_H
#define DEVICEWIDGET_H

#include "cardview.h"

namespace Ui {
class DeviceWidget;
}

class DeviceWidget : public CardView
{
    Q_OBJECT

public:
    explicit DeviceWidget(CardView *parent = nullptr);
    ~DeviceWidget();

    virtual void showDeviceData(quint8 *data, int index) = 0;

    void setPointName(const QString &poitName);
    void setParameterName(const QString &prmName);
    void setParameterValue(const QString &prmValue);
    void setPrmValueLabelStyle(const QString &prmStyle);
    void setPointNameLabelFontSize(int size);
    void setPrmNameLabelFontSize(int size);
    void setPrmValueLabelFontSize(int size);

    void setAddress(quint64 address) {myAddress = address;}
    void setIndex(int index) {myIndex = index;}
    quint64 getDeviceAddress() {return myAddress;}
    int getDeviceIndex() {return myIndex;}

signals:
    void sendCommand(TOpcode opcode, quint8 *data, quint8 data_len);

private slots:
    virtual void onSettingsButtonClicked() = 0;

private:
    Ui::DeviceWidget *ui;
    QFont deviceFont;

    quint64 myAddress = 0;
    int myIndex = 0;
};

#endif // DEVICEWIDGET_H
