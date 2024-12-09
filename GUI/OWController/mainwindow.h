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
    qint32 usbPollingPeriod = 0;
    qint32 secCounterEvent = 0;
    void timerEvent(QTimerEvent *event);

public slots:
    void onSendCommand(TOpcode opcode, quint8 *data, int data_len);

private slots:
    void onConnectButtonClicked();
    void onSearchButtonClicked();
    void onClockButtonClicked();
    void onDeviceComboBoxChanged(int index);
    void onSettingsButtonClicked();
    void onCloseSettingsClicked();
    void onWriteFileCheckboxToggled(bool checked);
    void onWriteFilePeriodChanged(int value);
    void onOWPollingPeriodChanged(int value);

private:
    Ui::MainWindow *ui;

    static const size_t USB_BUFF_SIZE = 65;

    const QString ProductString = "1-Wire Controller";

    QDialog *settingsWindow = nullptr;
    CustomHid *hidDevice = nullptr;
    ClockView *clockWidget = nullptr;
    QVBoxLayout *deviceViewLayout = nullptr;
    QList<DeviceWidget*> deviceWidget;
    QList<quint64> allDeviceAddressList;
    QHash<quint64, int> selDevices;

    quint32 timeStamp;
    int secCounter = 0;
    int writeFilePeriod;
    int owPollingPeriod;
    int owPrevPollingPeriod;

    bool isConnected = false;
    bool isPollingRunning = false;
    bool isShowClockEnabled = false;
    bool isWriteFileEnabled = true;

    quint8 writedDevice = 0;

    char column_sep = ';';

    unsigned char rxUsbBuffer[USB_BUFF_SIZE];
    unsigned char txUsbBuffer[USB_BUFF_SIZE];

    void startUsbPolling();
    void stopUsbPolling();
    void deinitWidgets();
    void createWidgetsLayout(int count);
    void handleReceivedPacket();
    void initDeviceComboBox();
    void deleteDeviceLayout();
    void writeCsvFile(float value, int index);
    void readSettings();
    void writeSettings();
};
#endif // MAINWINDOW_H
