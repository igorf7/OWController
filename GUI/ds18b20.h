#ifndef DS18B20_H
#define DS18B20_H

#include <QDialog>
#include <QLineEdit>
#include "devicewidget.h"

class DS18B20 : public DeviceWidget
{
    Q_OBJECT

public:
    explicit DS18B20(DeviceWidget *parent = nullptr);
    ~DS18B20();

    void showDeviceData(quint8 *data, int index) override;

private slots:
    void onSettingsButtonClicked() override;
    void onCopyButtonnClicked();
    void onCloseButtonClicked();
    void onWriteButtonClicked();
    void onReadButtonClicked();

private:
    const quint8 devFamilyCode = 0x28;

    QDialog *settingsWindow = nullptr;
    QLineEdit *almHighLineEdit = nullptr;
    QLineEdit *almLowLineEdit = nullptr;
    QLineEdit *resolutionLineEdit = nullptr;

    float temperValue = 0.0f;

    qint8 devAlarmHigh = 0;
    qint8 devAlarmLow = 0;
    quint8 devResolution = 0;
};

#endif // DS18B20_H
