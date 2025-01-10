#ifndef DS_OTHER_H
#define DS_OTHER_H

#include "devicewidget.h"
#include <QDialog>

namespace Ui {
class DS_OTHER;
}

class DS_OTHER : public DeviceWidget
{
    Q_OBJECT

public:
    explicit DS_OTHER(DeviceWidget *parent = nullptr);
    ~DS_OTHER();

    void showDeviceData(quint8 *data, int index) override;
    void setAddress(quint64 address) override;
    void setIndex(int index) override;
    void setWriteFilePeriod(bool enabled, int period) override;

private slots:
    void onSettingsButtonClicked();
    void onCopyButtonnClicked();
    void onCloseButtonClicked();

private:
    Ui::DS_OTHER *ui;

    QDialog *settingsWindow = nullptr;

    quint64 myAddress = 0;

    int myIndex = 0;
    int writeFilePeriod = 60;

    quint8 devFamilyCode = 0;

    bool isWriteFileEnabled = true;
};

#endif // DS_OTHER_H
