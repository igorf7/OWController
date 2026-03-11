#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLayout>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QMap>

#include "devicewidget.h"
#include "clockview.h"

#ifdef Q_OS_ANDROID
#include "jnilayer.h"
#else
#include "customhid.h"
#endif

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
    void onStart();
    void onConnectButtonClicked();
    void onSearchButtonClicked();
    void onClockButtonClicked();
    void onDeviceComboBoxChanged(int index);

private:
    Ui::MainWindow *ui;

    static const int USB_POLLING_PERIOD = 10;
    static const quint16 VID = 0x0483;
    static const quint16 PID = 0xdf11;

    QDialog *settingsWindow = nullptr;
    QSpinBox *writeFilePeriodSpinbox = nullptr;
    QSpinBox *pollingPeriodSpinbox = nullptr;

#ifdef Q_OS_ANDROID
    JniLayer *hidDevice = nullptr;
#else
    CustomHid *hidDevice = nullptr;
#endif

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

    void deinitWidgets();
    void createWidgetsLayout(int count);
    void handleReceivedPacket(const QByteArray &received);
    void initDeviceComboBox();
    void deleteDeviceLayout();
    qreal getScreenDiagonal();
};
#endif // MAINWINDOW_H
