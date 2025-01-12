#ifndef DS18B20_H
#define DS18B20_H

#include "devicewidget.h"
#include <QDialog>
#include <QLineEdit>

namespace Ui {
class DS18B20;
}

class DS18B20 : public DeviceWidget
{
    Q_OBJECT

public:
    explicit DS18B20(DeviceWidget *parent = nullptr);
    ~DS18B20();

    void showDeviceData(quint8 *data, int index) override;
    void setAddress(quint64 address) override;
    void setIndex(int index) override;
    void setWriteFilePeriod(int period) override;

protected:
    qint32 secIntervalEvent = 0;
    void timerEvent(QTimerEvent *event) override;

private slots:
    void onSettingsButtonClicked();
    void onCopyButtonnClicked();
    void onCloseButtonClicked();
    void onWriteButtonClicked();
    void onReadButtonClicked();

private:
    Ui::DS18B20 *ui;

    const quint8 devFamilyCode = 0x28;

    QDialog *settingsWindow = nullptr;
    QLineEdit *almHighLineEdit = nullptr;
    QLineEdit *almLowLineEdit = nullptr;
    QLineEdit *resolutionLineEdit = nullptr;
    quint64 myAddress = 0;
    int myIndex = 0;
    int secCounter = 0;
    int writeFilePeriod = 60; // 60 sec by default

    float temperValue = 0.0f;

    qint8 devAlarmHigh = 0;
    qint8 devAlarmLow = 0;
    quint8 devResolution = 0;

    char column_sep = ';';

    bool isWriteFileRequired = false;

    void writeCsvFile(float value, int index);
};

#endif // DS18B20_H
