#ifndef DS1971_H
#define DS1971_H

#include "cardview.h"
#include <QDialog>

namespace Ui {
class DS1971;
}

class DS1971 : public CardView
{
    Q_OBJECT

public:
    explicit DS1971(CardView *parent = nullptr);
    ~DS1971();

    void showDeviceData(quint8 *data);

private slots:
    void onSettingsButtonClicked();
    void onCloseButtonClicked();

private:
    Ui::DS1971 *ui;

    const quint8 devFamilyCode = 0x14;

    QDialog *settingsWindow = nullptr;

    quint64 deviceAddress = 0;

    quint8 devIndex = 0;
};

#endif // DS1971_H
