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
        this->onSearchButtonClicked();
    }
}

/**
 * @brief MainWindow::onSearchButtonClicked
 */
void MainWindow::onSearchButtonClicked()
{
    allDeviceAddressList.clear();

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
        count = selDevices.size();
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

    QStringList ComboBoxItems;

    for (auto i = 0; i < allDeviceAddressList.size(); ++i) {
        quint8 dev_family = allDeviceAddressList.at(i) & 0xFF;
        QString name = OWDevice::getName(dev_family);
        if (!ComboBoxItems.contains(name)) {
            ComboBoxItems.append(name);
        }
    }
    ui->deviceComboBox->addItems(ComboBoxItems);
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

    isShowClockEnabled = false;
    selDevices.clear();

    int dev_num = 0;
    quint8 dev_family = OWDevice::getFamily(ui->deviceComboBox->currentText());

    for (int i = 0; i < allDeviceAddressList.size(); ++i) {
        if (dev_family == (allDeviceAddressList.at(i) & 0xFF)) {
            selDevices.insert(allDeviceAddressList.at(i), dev_num++);
        }
    }

    ui->deviceCountLabel->setText(QString::number(selDevices.size()));

    this->createWidgetsLayout(selDevices.size());
    this->sendOneWireCommand(eReadCmd, &dev_family, sizeof(dev_family));
}

/**
 * @brief MainWindow::createWidgetsLayout
 */
void MainWindow::createWidgetsLayout(int count)
{
    if (count < 1) return;

    deviceWidget.clear();

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
        deviceWidget << new ClockView;
        deviceViewLayout->addWidget(deviceWidget.at(0));
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
            connect(deviceWidget.at(i), &CardView::sendCommand,
                    this, &MainWindow::sendOneWireCommand);
            deviceViewLayout->addWidget(deviceWidget.at(i));
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
    quint64 dev_addr = *((quint64*)rx_packet->data);

    if (rx_packet->rep_id == eRepId_4)
    {
        switch (rx_packet->opcode)
        {
        case eSearchCmd:
            break;
        case eEnumerate:
            allDeviceAddressList.append(dev_addr);
            break;
        case eEnumerateDone:
            this->initDeviceComboBox();
            if ((allDeviceAddressList.size() == 0) && (!isShowClockEnabled)) {
                this->onClockButtonClicked();
            }
            statusBar()->showMessage(tr("Обнаружено 1-Wre устройств: ") +
                                         QString::number(allDeviceAddressList.size()), 5000);
            break;
        case eReadCmd:
            if (!isShowClockEnabled && !deviceWidget.isEmpty()) {
                int index = selDevices.value(dev_addr);
                deviceWidget.at(index)->showDeviceData(rx_packet->data, index + 1);
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
                    clockView = (ClockView*)deviceWidget.at(0);
                    clockView->showDeviceData(timeStamp);
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
 * @brief MainWindow::deinitWidgets
 */
void MainWindow::deinitWidgets()
{
    for (auto &it : deviceWidget) {
        if (it != nullptr) it->close();
    }
    ui->deviceCountLabel->setText("0");
    ui->deviceComboBox->clear();
    ui->deviceComboBox->setCurrentIndex(-1);
    ui->searchPushButton->setEnabled(false);
    ui->clockPushButton->setEnabled(false);
}
