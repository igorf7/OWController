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
    int secIntervalEvent = 0;
    int usbPollingEvent = 0;
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

    static const size_t USB_BUFF_SIZE = 65;
    static const int USB_POLLING_PERIOD = 10;

    QDialog *settingsWindow = nullptr;
    QSpinBox *writeFilePeriodSpinbox = nullptr;
    QSpinBox *pollingPeriodSpinbox = nullptr;

    CustomHid *hidDevice = nullptr;
    ClockView *clockWidget = nullptr;
    QVBoxLayout *deviceViewLayout = nullptr;
    QList<DeviceWidget*> deviceWidget;
    QList<quint64> allDeviceAddressList;
    QHash<quint64, int> selDevices;

    int timeStamp = 0;
    int secCounter = 0;
    int owPollingPeriod = 1; // 1 sec by default
    int writeFilePeriod = 60; // 60 sec by default

    bool isConnected = false;
    bool isUsbPollRunning = false;
    bool isShowClockEnabled = false;
    bool isOwSearchDone = false;
    bool isWriteFileEnabled = true;

    quint8 writedDevice = 0;
    char sep = ';';

    unsigned char rxUsbBuffer[USB_BUFF_SIZE];
    unsigned char txUsbBuffer[USB_BUFF_SIZE];

    void deinitWidgets();
    void createWidgetsLayout(int count);
    void handleReceivedPacket();
    void initDeviceComboBox();
    void deleteDeviceLayout();
    void writeCsvFile(float value, int index);
};
#endif // MAINWINDOW_H
