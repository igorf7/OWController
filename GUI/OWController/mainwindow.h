#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "customhid.h"
#include "usertypes.h"
#include "owdevice.h"
#include "ds1971.h"
#include "ds18b20.h"
#include "ds_other.h"
#include "clockview.h"
#include <QMainWindow>
#include <QLayout>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QMap>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    qint32 usbPollingEvent = 0;
    void timerEvent(QTimerEvent *event);

public slots:
    void onSendCommand(TOpcode opcode, quint8 *data, int data_len);
    void onShowStatusBar(const QString &str, int timeout);
    void onUsbConnected();
    void onUsbDisconnected();

private slots:
    void onConnectButtonClicked();
    void onSearchButtonClicked();
    void onClockButtonClicked();
    void onDeviceComboBoxChanged(int index);
    void onSettingsButtonClicked();
    void onCloseSettingsClicked();
    void onWriteFilePeriodChanged(int value);
    void onOWPollingPeriodChanged(int value);

private:
    Ui::MainWindow *ui;

    static const size_t USB_BUFF_SIZE = 64;

    const QString ProductString = "1-Wire Controller";

    QDialog *settingsWindow = nullptr;
    QCheckBox *writeFileCheckbox = nullptr;
    QSpinBox *writeFilePeriodSpinbox = nullptr;
    QSpinBox *pollingPeriodSpinbox = nullptr;

    CustomHid *hidDevice = nullptr;
    ClockView *clockWidget = nullptr;
    QVBoxLayout *deviceViewLayout = nullptr;
    QList<DeviceWidget*> deviceWidget;
    QList<quint64> allDeviceAddressList;
    QHash<quint64, int> selDevices;

    int timeStamp = 0;
    int milliSeconds = 0;
    int usbPollPeriod = 10;
    int writeFilePeriod;
    int owPollingPeriod;

    bool isConnected = false;
    bool isUsbPollRunning = false;
    bool isShowClockEnabled = false;
    bool isWriteFileEnabled = true;
    bool isOwSearchDone = false;

    unsigned char rxUsbBuffer[USB_BUFF_SIZE];
    unsigned char txUsbBuffer[USB_BUFF_SIZE];

    void startUsbPolling();
    void stopUsbPolling();
    void deinitWidgets();
    void createWidgetsLayout(int count);
    void handleReceivedPacket();
    void initDeviceComboBox();
    void deleteDeviceLayout();
    void readSettings();
    void writeSettings();
};
#endif // MAINWINDOW_H
