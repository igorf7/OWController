#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "owdevice.h"
#include "ds1971.h"
#include "ds18b20.h"
#include "ds_other.h"
#include <QDialog>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

/**
 * @brief MainWindow Class Constructor
 * @param parent_object
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(QApplication::applicationName());

    QString filepath = "";

    /* Create USB Custom HID device */
#ifdef Q_OS_ANDROID
    hidDevice = new JniLayer(VID, PID);
    filepath = (getScreenDiagonal() > 7.4) ? ":qss/tablet.qss" : ":qss/phone.qss";

    connect(hidDevice, &JniLayer::deviceConnected,
            this, &MainWindow::onUsbConnected);
    connect(hidDevice, &JniLayer::deviceDisconnected,
            this, &MainWindow::onUsbDisconnected);
#else
    hidDevice = new CustomHid(VID, PID);
    filepath = ":qss/desktop.qss";
    this->window()->resize(252, 316);

    connect(hidDevice, &CustomHid::deviceConnected,
            this, &MainWindow::onUsbConnected);
    connect(hidDevice, &CustomHid::deviceDisconnected,
            this, &MainWindow::onUsbDisconnected);
#endif
    connect(ui->connectPushButton, SIGNAL(clicked()),
            this, SLOT(onConnectButtonClicked()));
    connect(ui->searchPushButton, SIGNAL(clicked()),
            this, SLOT(onSearchButtonClicked()));
    connect(ui->clockPushButton, SIGNAL(clicked()),
            this, SLOT(onClockButtonClicked()));
    connect(ui->deviceComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onDeviceComboBoxChanged(int)));

    QFile qssfile(filepath);
    if (qssfile.open(QFile::ReadOnly)) {
        QString StyleSheet = QLatin1String(qssfile.readAll());
        qApp->setStyleSheet(StyleSheet);
    }

    this->onStart();
}

/**
 * @brief MainWindow Class Destructor
 */
MainWindow::~MainWindow()
{
    if (isConnected) {
        this->onConnectButtonClicked();
    }

    delete hidDevice;
    delete ui;
}

/**
 * @brief MainWindow::Connection::onStart
 */
void MainWindow::onStart()
{
    this->onConnectButtonClicked();
}

/**
 * @brief onShowStatusBar
 * @param str
 * @param timeout
 */
void MainWindow::onShowStatusBar(const QString &str, int timeout)
{
    statusBar()->showMessage(str, timeout);
}

/**
 * @brief MainWindow::onConnectButtonClicked
 */
void MainWindow::onConnectButtonClicked()
{
    isShowClockEnabled = false;
    isOwSearchDone = false;

    if (!isConnected) {
        hidDevice->Connect();
    }
    else {
        hidDevice->Disconnect();
        if (usbPollingEvent != 0) {
            killTimer(usbPollingEvent);
            usbPollingEvent = 0;
            isUsbPollRunning = false;
        }
    }
}

/**
 * @brief MainWindow::onUsbConnected
 */
void MainWindow::onUsbConnected()
{
    isConnected = true;
    ui->connectPushButton->setIcon(QIcon(":/images/sw_on.png"));
    statusBar()->showMessage(tr("USB device connected"));

    if (!isUsbPollRunning) {
        isUsbPollRunning = true;
        secIntervalEvent = startTimer(1000);
        usbPollingEvent = startTimer(1);
    }
}

/**
 * @brief MainWindow::onUsbDisconnected
 */
void MainWindow::onUsbDisconnected()
{
    ui->connectPushButton->setIcon(QIcon(":/images/sw_off.png"));
    isConnected = false;
    this->deinitWidgets();
    statusBar()->showMessage(tr("USB device disconnected"));
}

/**
 * @brief MainWindow::timerEvent
 * @param event
 */
void MainWindow::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == secIntervalEvent) {
        secCounter++;
        if (!isOwSearchDone) {
            this->onSearchButtonClicked();
        }
        else {
            quint8 tx_data[2]; // contains the device familiy and 1-Wire polling period
            tx_data[0] = OWDevice::getFamily(ui->deviceComboBox->currentText());
            tx_data[1] = (quint8)owPollingPeriod;
            this->onSendCommand(eOwReadData, tx_data, sizeof(tx_data));
            if (secCounter >= writeFilePeriod) {
                secCounter = 0;
            }
        }
    }
    else if (event->timerId() == usbPollingEvent) {
        QByteArray rxData;
        hidDevice->readFromDevice(rxData);
        if (rxData.size() > 0) {
            this->handleReceivedPacket(rxData);
        }
    }
}

/**
 * @brief MainWindow::onSearchButtonClicked
 */
void MainWindow::onSearchButtonClicked()
{
    if (!isConnected) return;

    isOwSearchDone = false;
    allDeviceAddressList.clear();
    this->onSendCommand(eOwSearch, nullptr, 0);
}

/**
 * @brief MainWindow::onClockButtonClicked
 */
void MainWindow::onClockButtonClicked()
{
    int count;

    if (isShowClockEnabled) {
        isShowClockEnabled = false;
        count = selDevices.size();
    }
    else {
        isShowClockEnabled = true;
        count = 1;
    }

    this->createWidgetsLayout(count);
}

/**
 * @brief MainWindow::onSendCommand
 * @param opcode
 * @param dev_type
 */
void MainWindow::onSendCommand(TOpcode opcode, quint8 *data, int data_len)
{
    if (!isConnected) return;

    QByteArray txData;
    txData.append((quint8)eRepId_3);
    txData.append((quint8)opcode);
    txData.append((quint8)0);
    for (int i = 0; i < data_len; i++) {
        txData.append(data[i]);
    }
    hidDevice->writeToDevice(txData);
}

/**
 * @brief MainWindow::initDeviceComboBox
 */
void MainWindow::initDeviceComboBox()
{
    ui->deviceComboBox->clear();

    QStringList ComboBoxItems;

    for (int i = 0; i < allDeviceAddressList.size(); ++i) {
        quint8 dev_family = allDeviceAddressList.at(i) & 0xFF;
        QString name = OWDevice::getName(dev_family);
        if (!ComboBoxItems.contains(name)) {
            ComboBoxItems.append(name);
        }
    }
    if (!ComboBoxItems.isEmpty()) {
        ui->deviceComboBox->addItems(ComboBoxItems);
    }
}

/**
 * @brief onDeviceComboBoxChanged
 * @param index
 */
void MainWindow::onDeviceComboBoxChanged(int index)
{
    Q_UNUSED(index)

    if (!isConnected) return;

    isShowClockEnabled = false;
    selDevices.clear();

    quint8 dev_family = OWDevice::getFamily(ui->deviceComboBox->currentText());

    for (int i = 0, j = 0; i < allDeviceAddressList.size(); ++i) {
        if (dev_family == (allDeviceAddressList.at(i) & 0xFF)) {
            selDevices.insert(allDeviceAddressList.at(i), j++);
        }
    }

    statusBar()->showMessage(ui->deviceComboBox->currentText() +
                            " device found: " + QString::number(selDevices.size()));

    this->createWidgetsLayout(selDevices.size());
}

/**
 * @brief MainWindow::createWidgetsLayout
 */
void MainWindow::createWidgetsLayout(int count)
{
    if (count < 1) return;

    if (!deviceWidget.isEmpty()) {
        deviceWidget.clear();
    }

    /* Delete previous layout if exist */
    this->deleteDeviceLayout();

    /* Create new layout */
    deviceViewLayout = new QVBoxLayout;

    if (isShowClockEnabled) {
        clockWidget = new ClockView;
        deviceViewLayout->addWidget(clockWidget);
    }
    else {
        quint8 device_family = OWDevice::getFamily(ui->deviceComboBox->currentText());
        for (int i = 0; i < count; i++)
        {
            switch (device_family)
            {
           case 0x14: // DS1971
               deviceWidget << new DS1971;
               break;

            case 0x28: // DS18B20
                deviceWidget << new DS18B20;
                break;

            default:  // any other device
                deviceWidget << new DS_OTHER;
                break;
            }

            if (deviceWidget.at(i) == nullptr) return;
            quint64 dev_address = selDevices.key(i);
            deviceWidget.at(i)->setAddress(dev_address);
            int index = selDevices.value(dev_address);
            deviceWidget.at(i)->setIndex(index + 1);
            connect(deviceWidget.at(i), &DeviceWidget::sendCommand,
                                  this, &MainWindow::onSendCommand);

            deviceViewLayout->addWidget(deviceWidget.at(i));
        }
    }

    ui->scrollAreaWidgetContents->setLayout(deviceViewLayout);
}

/**
 * @brief MainWindow::handleReceivedPacket
 */
void MainWindow::handleReceivedPacket(const QByteArray &received)
{
    TAppLayerPacket *rx_packet = (TAppLayerPacket*)received.data();
    quint64 dev_addr = *((quint64*)rx_packet->data);

    if (rx_packet->rep_id == eRepId_4)
    {
        switch (rx_packet->opcode)
        {
        case eOwSearch:
            break;

        case eOwEnumerate:
            allDeviceAddressList.append(dev_addr);
            break;

        case eOwEnumerateDone:
            isOwSearchDone = true;
            this->initDeviceComboBox();
            if ((allDeviceAddressList.size() == 0) && (!isShowClockEnabled)) {
                this->onClockButtonClicked();
            }
            else {
                statusBar()->showMessage(tr("Total 1-Wire devices found: ") +
                                        QString::number(allDeviceAddressList.size()));
            }
            break;

        case eOwReadData:
            if (!isShowClockEnabled && !deviceWidget.isEmpty()) {
                int index = selDevices.value(dev_addr);
                deviceWidget.at(index)->showDeviceData(rx_packet->data, index + 1);
            }
            break;

        case eOwWriteData:
            break;

        case eGetRtcData:
            if (isShowClockEnabled) {
                timeStamp = *((quint32*)rx_packet->data);
                quint64 unixtime = QDateTime::currentSecsSinceEpoch();
                qint64 dif = (qint64)(unixtime - timeStamp);
                if (qAbs(dif) > 2) {
                    timeStamp = unixtime + 1;
                    this->onSendCommand(eSyncRtc, (quint8*)&timeStamp, sizeof(timeStamp));
                }
                else {
                    clockWidget->showDateTime(timeStamp);
                }
            }
            break;

        default:
            break;
        }
    }
}

/**
 * @brief MainWindow::deleteDeviceLayout
 */
void MainWindow::deleteDeviceLayout()
{
    if (deviceViewLayout != nullptr) {
        while (QLayoutItem* item = deviceViewLayout->takeAt(0)) {
            delete item->widget();
            delete item;
        }
        delete deviceViewLayout;
        deviceViewLayout = nullptr;
    }
}

/**
 * @brief MainWindow::deinitWidgets
 */
void MainWindow::deinitWidgets()
{
    this->deleteDeviceLayout();
    deviceWidget.clear();
    ui->deviceComboBox->clear();
    ui->deviceComboBox->setCurrentIndex(-1);
}

/**
 * @brief MainWindow::getScreenDiagonal
 * @return
 */
qreal MainWindow::getScreenDiagonal()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    //qreal dpi = screen->logicalDotsPerInch();
    QSizeF size = screen->physicalSize(); // size in millimeters
    qreal widthInch = size.width() / 25.4;
    qreal heightInch = size.height() / 25.4; // size in inches
    // Diagonal in inches
    qreal diagonal = sqrt(widthInch * widthInch + heightInch * heightInch);
    return diagonal;
}
