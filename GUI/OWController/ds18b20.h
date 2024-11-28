#ifndef DS18B20_H
#define DS18B20_H

#include "cardview.h"
#include <QDialog>
#include <QLineEdit>

namespace Ui {
class DS18B20;
}

class DS18B20 : public CardView
{
    Q_OBJECT

public:
    explicit DS18B20(CardView *parent = nullptr);
    ~DS18B20();

    void showDeviceData(quint8 *data);
    void setAddress(quint64 address);
    void setIndex(int index);

private slots:
    void onSettingsButtonClicked();
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

    float deviceData = 0.0f;

    qint8 devAlarmHigh = 0;
    qint8 devAlarmLow = 0;
    quint8 devResolution = 0;
};

#endif // DS18B20_H
