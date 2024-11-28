#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

using namespace std;

/**
 * @brief MainWindow Class Constructor
 * @param parent_object
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(QApplication::applicationName());

    /* Create USB Custom HID device */
    hidDevice = new CustomHid(0x0483, 0x5711);

    /* Connecting signals to slots */
    connect(ui->connectPushButton, SIGNAL(clicked()),
            this, SLOT(onConnectButtonClicked()));
    connect(ui->searchPushButton, SIGNAL(clicked()),
            this, SLOT(onSearchButtonClicked()));
    connect(ui->clockPushButton, SIGNAL(clicked()),
            this, SLOT(onClockButtonClicked()));
    connect(ui->deviceComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onDeviceComboBoxChanged(int)));

    /* Activate status bar */
    QFont font;
    font.setItalic(true);
    statusBar()->setFont(font);

    /* Attempting to connect a USB device */
    onConnectButtonClicked();
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
 * @brief MainWindow::onConnectButtonClicked
 */
void MainWindow::onConnectButtonClicked()
{
    isShowClockEnabled = false;

    if (isConnected) {
        this->stopUsbPolling();
        hidDevice->Disconnect();
        isConnected = false;
        this->deinitWidgets();

        ui->connectPushButton->setIcon(QIcon(":/images/switch-off.png"));
        statusBar()->showMessage(tr("USB порт закрыт"));
    }
    else {
        isConnected = hidDevice->Connect(ProductString);

        if (!isConnected) {
            this->deinitWidgets();
            ui->connectPushButton->setIcon(QIcon(":/images/switch-off.png"));
            statusBar()->showMessage(tr("USB устройство не обнаружено"));
            return;
        }

        ui->searchPushButton->setEnabled(true);
        ui->clockPushButton->setEnabled(true);
        ui->connectPushButton->setIcon(QIcon(":/images/switch-on.png"));
        statusBar()->showMessage(tr("USB порт открыт"));
        this->startUsbPolling();
        //this->onClockButtonClicked();
        this->onSearchButtonClicked();
    }
}

/**
 * @brief MainWindow::onSearchButtonClicked
 */
void MainWindow::onSearchButtonClicked()
{
    devFoundMap.clear();
    owDevice.clearAddressList();
    ui->searchPushButton->setEnabled(false);
    this->sendOneWireCommand(eSearchCmd, nullptr, 0);
}

/**
 * @brief MainWindow::onClockButtonClicked
 */
void MainWindow::onClockButtonClicked()
{
    int count;

    if (isShowClockEnabled) {
        isShowClockEnabled = false;
        count = selectedDeviceCount;
    }
    else {
        isShowClockEnabled = true;
        count = 1;
    }

    this->createWidgetsLayout(count);
}

/**
 * @brief MainWindow::sendOneWireCommand
 * @param opcode
 * @param dev_type
 */
void MainWindow::sendOneWireCommand(TOpcode opcode, quint8 *data, quint8 data_len)
{
    if (!isConnected) return;

    TAppLayerPacket *tx_packet = (TAppLayerPacket*)txUsbBuffer;

    tx_packet->rep_id = eRepId_3;
    tx_packet->opcode = opcode;
    tx_packet->size = 0;
    for (auto i = 0; i < data_len; i++) {
        tx_packet->data[i] = data[i];
    }

    int len = hidDevice->Write(txUsbBuffer, USB_BUFF_SIZE, false);
    if (len < 0) { // Lost connection
        if (isConnected) this->onConnectButtonClicked();
    }
}

/**
 * @brief MainWindow::startUsbPolling
 */
void MainWindow::startUsbPolling()
{
    if (!isConnected) return;

    if (!isPollingRunning) {
        isPollingRunning = true;
        pollingEvent = startTimer(50);
    }
}

/**
 * @brief MainWindow::stopUsbPolling
 */
void MainWindow::stopUsbPolling()
{
    killTimer(pollingEvent);
    pollingEvent = 0;
    isPollingRunning = false;
}

/**
 * @brief MainWindow::timerEvent
 * @param event
 */
void MainWindow::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == pollingEvent) {
        int len = hidDevice->Read(rxUsbBuffer, USB_BUFF_SIZE);
        if (len < 0) { // Lost connection
            if (isConnected) this->onConnectButtonClicked();
            return;
        }
        else if (len != 0) {
            handleReceivedPacket();
        }
    }
}

/**
 * @brief MainWindow::initDeviceComboBox
 */
void MainWindow::initDeviceComboBox()
{
    ui->deviceComboBox->clear();

    owDevice.getFound(devFoundMap);

    QMapIterator<QString, quint8> it(devFoundMap);

    while (it.hasNext()) {
        it.next();
        ui->deviceComboBox->addItem(it.key());
    }

    ui->searchPushButton->setEnabled(true);
}

/**
 * @brief onDeviceComboBoxChanged
 * @param index
 */
void MainWindow::onDeviceComboBoxChanged(int index)
{
    Q_UNUSED(index)

    if (!isConnected) return;

    currDevNumber = 0;
    isShowClockEnabled = false;

    selectedDeviceCount = devFoundMap.value(ui->deviceComboBox->currentText());
    ui->deviceCountLabel->setText(QString::number(selectedDeviceCount));
    this->createWidgetsLayout(selectedDeviceCount);

    quint8 dev_family = OWDevice::getFamily(ui->deviceComboBox->currentText());
    this->sendOneWireCommand(eReadCmd, &dev_family, sizeof(dev_family));
}

/**
 * @brief MainWindow::createWidgetsLayout
 */
void MainWindow::createWidgetsLayout(int count)
{
    if (count < 1) return;

    devWidgetList.clear();

    /* Delete previous layout if exist */
    if (deviceViewLayout != nullptr) {
        while (QLayoutItem* item = deviceViewLayout->takeAt(0)) {
            delete item->widget();
            delete item;
        }
        delete deviceViewLayout;
        deviceViewLayout = nullptr;
    }

    /* Create new layout */
    deviceViewLayout = new QVBoxLayout;

    if (isShowClockEnabled) {
        devWidgetList << new ClockView;
        deviceViewLayout->addWidget(devWidgetList.at(0));
    }
    else {
        quint8 device_family = OWDevice::getFamily(ui->deviceComboBox->currentText());

        for (int i = 0; i < count; i++)
        {
            switch (device_family)
            {
            case 0x14: // DS1971
                ds1971 = new DS1971;
                connect(ds1971, &DS1971::sendCommand, this, &MainWindow::sendOneWireCommand);
                devWidgetList << ds1971;
                break;

            // case 0x10: // DS18S20
            //     break;

            case 0x28: // DS18B20
                ds18b20 = new DS18B20;
                connect(ds18b20, &DS18B20::sendCommand, this, &MainWindow::sendOneWireCommand);
                devWidgetList << ds18b20;
                break;

            default:  // any other device
                devWidgetList << new DS_OTHER;
                break;
            }

            if (devWidgetList.at(i) == nullptr) return;
            deviceViewLayout->addWidget(devWidgetList.at(i));
        }
    }
    ui->scrollAreaWidgetContents->setLayout(deviceViewLayout);
}

/**
 * @brief MainWindow::handleReceivedPacket
 */
void MainWindow::handleReceivedPacket()
{
    TAppLayerPacket *rx_packet = (TAppLayerPacket*)rxUsbBuffer;
    quint64 dev_addr = 0;

    if (rx_packet->rep_id == eRepId_4)
    {
        switch (rx_packet->opcode)
        {
        case eSearchCmd:
            break;
        case eEnumerate:
            dev_addr = *((quint64*)rx_packet->data);
            owDevice.addAddress(dev_addr);
            break;
        case eEnumerateDone:
            this->initDeviceComboBox();
            totalDeviceCount = owDevice.getCount();

            if ((totalDeviceCount == 0) && (!isShowClockEnabled)) {
                this->onClockButtonClicked();
            }
            statusBar()->showMessage(tr("Обнаружено 1-Wre устройств: ") +
                                         QString::number(totalDeviceCount), 5000);
            break;
        case eReadCmd:
            if (!isShowClockEnabled) {
                this->showReceivedData(rx_packet);
            }
            break;
        case eWriteCmd:
            break;
        case eGetRtcCmd:
            if (isShowClockEnabled) {
                timeStamp = *((quint32*)rx_packet->data);
                if (qAbs(QDateTime::currentSecsSinceEpoch() - timeStamp) > 1) {
                    timeStamp = QDateTime::currentSecsSinceEpoch() + 1;
                    this->sendOneWireCommand(eSyncRtcCmd, (quint8*)&timeStamp, sizeof(timeStamp));
                }
                else {
                    clockView = (ClockView*)devWidgetList.at(0);
                    clockView->showDateTime(timeStamp);
                }
            }
            break;
        default:
            // wrong command
            break;
        }
    }
}

/**
 * @brief MainWindow::showReceivedData
 * @param rx_packet
 */
void MainWindow::showReceivedData(TAppLayerPacket *rx_packet)
{
    quint64 device_address = *((quint64*)rx_packet->data);
    quint8  device_family = (device_address & 0xFF);

    if (device_family != OWDevice::getFamily(ui->deviceComboBox->currentText())) return;

    switch (device_family)
    {
    case 0x14:  // DS1971
        ds1971 = (DS1971*)devWidgetList.at(currDevNumber++);
        ds1971->showDeviceData(rx_packet->data, currDevNumber);
        break;

    // case 0x10:  // DS18S20
    //     break;

    case 0x28:  // DS18B20
        ds18b20 = (DS18B20*)devWidgetList.at(currDevNumber++);
        ds18b20->showDeviceData(rx_packet->data, currDevNumber);
        break;

    default:    // Any other device
        dsOther = (DS_OTHER*)devWidgetList.at(currDevNumber++);
        dsOther->showDeviceData(rx_packet->data, currDevNumber);
        break;
    }

    if (currDevNumber >= selectedDeviceCount) currDevNumber = 0;
}

/**
 * @brief MainWindow::deinitWidgets
 */
void MainWindow::deinitWidgets()
{
    for (auto &it : devWidgetList) {
        if (it != nullptr) it->close();
    }
    ui->deviceCountLabel->setText("0");
    ui->deviceComboBox->clear();
    ui->deviceComboBox->setCurrentIndex(-1);
    ui->searchPushButton->setEnabled(false);
    ui->clockPushButton->setEnabled(false);
}
