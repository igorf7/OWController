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
    qint32 pollingEvent = 0;
    void timerEvent(QTimerEvent *event);

public slots:
    void sendOneWireCommand(TOpcode opcode, quint8 *data, quint8 data_len);

private slots:
    void onConnectButtonClicked();
    void onSearchButtonClicked();
    void onClockButtonClicked();
    void onDeviceComboBoxChanged(int);

private:
    Ui::MainWindow *ui;

    static const size_t USB_BUFF_SIZE = 65;

    const QString ProductString = "1-Wire Controller";

    CustomHid *hidDevice = nullptr;

    QVBoxLayout *deviceViewLayout = nullptr;
    QList<QWidget*> devWidgetList;
    TDeviceMap devFoundMap;

    OWDevice  owDevice;

    ClockView *clockView;
    DS18B20   *ds18b20;
    DS1971    *ds1971;
    DS_OTHER  *dsOther;

    quint32 timeStamp;

    int selectedDeviceCount = 0;
    int totalDeviceCount = 0;
    int currDevNumber = 0;

    float deviceValue = 0.0;

    bool isConnected = false;
    bool isPollingRunning = false;
    bool isShowClockEnabled = false;

    unsigned char rxUsbBuffer[USB_BUFF_SIZE];
    unsigned char txUsbBuffer[USB_BUFF_SIZE];

    void startUsbPolling();
    void stopUsbPolling();
    void deinitWidgets();
    void createWidgetsLayout(int count);
    void handleReceivedPacket();
    void initDeviceComboBox();
    void showReceivedData(TAppLayerPacket *rx_packet);
};
#endif // MAINWINDOW_H
