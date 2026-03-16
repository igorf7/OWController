#ifndef DS_OTHER_H
#define DS_OTHER_H

#include "devicewidget.h"
#include <QDialog>

class DS_OTHER : public DeviceWidget
{
    Q_OBJECT

public:
    explicit DS_OTHER(DeviceWidget *parent = nullptr);
    ~DS_OTHER();

    void showDeviceData(quint8 *data, int index) override;

private slots:
    void onSettingsButtonClicked() override;
    void onCopyButtonnClicked();
    void onCloseButtonClicked();

private:
    QDialog *settingsWindow = nullptr;
};

#endif // DS_OTHER_H
