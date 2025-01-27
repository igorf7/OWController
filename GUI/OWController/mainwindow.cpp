#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cardview.h"
#include <QDialog>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

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

#ifdef __ANDROID__
    QSize size(38, 38);
    ui->searchPushButton->setMinimumSize(size);
    ui->clockPushButton->setMinimumSize(size);
    ui->settingsPushButton->setMinimumSize(size);
    ui->connectPushButton->setMinimumSize(size);
    ui->searchPushButton->setMaximumSize(size);
    ui->clockPushButton->setMaximumSize(size);
    ui->settingsPushButton->setMaximumSize(size);
    ui->connectPushButton->setMaximumSize(size);
    ui->deviceComboBox->setMinimumSize(size);
#endif

    /* Create USB Custom HID device */
    hidDevice = new CustomHid(0x0483, 0x5711);

    /* Connecting signals to slots */
    connect(hidDevice, &CustomHid::showStatusBar,
            this, &MainWindow::onShowStatusBar);
    connect(hidDevice, &CustomHid::deviceConnected,
            this, &MainWindow::onUsbConnected);
    connect(hidDevice, &CustomHid::deviceDisconnected,
            this, &MainWindow::onUsbDisconnected);
    connect(ui->connectPushButton, SIGNAL(clicked()),
            this, SLOT(onConnectButtonClicked()));
    connect(ui->searchPushButton, SIGNAL(clicked()),
            this, SLOT(onSearchButtonClicked()));
    connect(ui->clockPushButton, SIGNAL(clicked()),
            this, SLOT(onClockButtonClicked()));
    connect(ui->deviceComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onDeviceComboBoxChanged(int)));
    connect(ui->settingsPushButton, SIGNAL(clicked()),
            this, SLOT(onSettingsButtonClicked()));

    /* Activate status bar */
    QFont font;
    font.setItalic(true);
    statusBar()->setFont(font);

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
                writedDevice = 0;
            }
        }
    }
    else if (event->timerId() == usbPollingEvent) {
        int len = hidDevice->Read(rxUsbBuffer, USB_BUFF_SIZE);
        if (len < 0) { // Lost connection
            //if (isConnected) this->onConnectButtonClicked();
            statusBar()->showMessage(tr("USB Receive Error"));
            return;
        }
        else if (len != 0) {
            this->handleReceivedPacket();
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
 * @brief MainWindow::onSettingsButtonClicked
 */
void MainWindow::onSettingsButtonClicked()
{
    settingsWindow = new QDialog;

    settingsWindow->setWindowFlags((settingsWindow->windowFlags())
                                   & (~Qt::WindowContextHelpButtonHint));

    QPixmap pm = QPixmap(1, 1);
    pm.fill(QColor(0, 0, 0, 0));
    settingsWindow->setWindowIcon(QIcon(pm));

    settingsWindow->resize(this->window()->width(), this->window()->height()/2);
    settingsWindow->setModal(true);

    settingsWindow->setAttribute(Qt::WA_DeleteOnClose);

    QVBoxLayout *vdlgLayout = new QVBoxLayout;
    writeFilePeriodSpinbox = new QSpinBox;
    writeFilePeriodSpinbox->setRange(1, 300);
    writeFilePeriodSpinbox->setSingleStep(1);
    writeFilePeriodSpinbox->setValue(writeFilePeriod);
    writeFilePeriodSpinbox->setSuffix(tr(" sec"));
    pollingPeriodSpinbox = new QSpinBox;
    pollingPeriodSpinbox->setRange(1, 60);
    pollingPeriodSpinbox->setSingleStep(1);
    pollingPeriodSpinbox->setValue(owPollingPeriod);
    pollingPeriodSpinbox->setSuffix(tr(" sec"));
    QLabel *pollingPeriodLabel = new QLabel(tr("1-Wire Bus Polling Period"));
    QLabel *writePeriodLabel = new QLabel(tr("Write CSV file period"));
    QPushButton *closeButton = new QPushButton(tr("Save and Close"));
    closeButton->setMinimumHeight(32);

    QHBoxLayout *h1Layout = new QHBoxLayout;
    h1Layout->addWidget(pollingPeriodLabel);
    h1Layout->addWidget(pollingPeriodSpinbox);
    h1Layout->addStretch();

    QHBoxLayout *h2Layout = new QHBoxLayout;
    h2Layout->addWidget(writePeriodLabel);
    h2Layout->addWidget(writeFilePeriodSpinbox);

    vdlgLayout->addLayout(h1Layout);
    vdlgLayout->addLayout(h2Layout);
    vdlgLayout->addWidget(closeButton);
    settingsWindow->setLayout(vdlgLayout);

    connect(closeButton, SIGNAL(clicked()),
            this, SLOT(onCloseSettingsClicked()));
    connect(writeFilePeriodSpinbox, SIGNAL(valueChanged(int)),
            this, SLOT(onWriteFilePeriodChanged(int)));
    connect(pollingPeriodSpinbox, SIGNAL(valueChanged(int)),
            this, SLOT(onOWPollingPeriodChanged(int)));

    settingsWindow->show();
}

/**
 * @brief MainWindow::onWriteFilePeriodChanged
 * @param value
 */
void MainWindow::onWriteFilePeriodChanged(int value)
{    
    if (value < pollingPeriodSpinbox->value()) {
        writeFilePeriodSpinbox->setValue(pollingPeriodSpinbox->value());
    }
}

/**
 * @brief MainWindow::onOWPollingPeriodChanged
 * @param value
 */
void MainWindow::onOWPollingPeriodChanged(int value)
{
    if (writeFilePeriodSpinbox->value() < value) {
        writeFilePeriodSpinbox->setValue(value);
    }
}

/**
 * @brief MainWindow::onCloseButtonClicked
 */
void MainWindow::onCloseSettingsClicked()
{
    if (settingsWindow != nullptr)
    {
        writeFilePeriod = writeFilePeriodSpinbox->value();
        settingsWindow->close();
        delete settingsWindow;
        settingsWindow = nullptr;
    }
}

/**
 * @brief MainWindow::onSendCommand
 * @param opcode
 * @param dev_type
 */
void MainWindow::onSendCommand(TOpcode opcode, quint8 *data, int data_len)
{
    if (!isConnected) return;

    TAppLayerPacket *tx_packet = (TAppLayerPacket*)txUsbBuffer;

    tx_packet->rep_id = eRepId_3;
    tx_packet->opcode = opcode;
    tx_packet->size = 0;
    for (int i = 0; i < data_len; i++) {
        tx_packet->data[i] = data[i];
    }

    int len = hidDevice->Write(txUsbBuffer, USB_BUFF_SIZE);
    if (len < 0) { // Lost connection
        //if (isConnected) this->onConnectButtonClicked();
        statusBar()->showMessage(tr("USB Transmit Error"));
    }
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
void MainWindow::handleReceivedPacket()
{
    TAppLayerPacket *rx_packet = (TAppLayerPacket*)rxUsbBuffer;
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
            if ((writedDevice < selDevices.size())
                && (ui->deviceComboBox->currentText() == "DS18B20")) {
                int index = selDevices.value(dev_addr);
                float value = *((float*)(rx_packet->data + sizeof(dev_addr)));
                this->writeCsvFile(value, index+1);
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
 * @brief MainWindow::writeCsvFile
 */
void MainWindow::writeCsvFile(float value, int index)
{
#ifdef __ANDROID__
    QString folderPath = "/storage/emulated/0/OWController/DS18B20_CSV";
#else
    QString folderPath = QDir::currentPath() + "/DS18B20_CSV";
#endif

    QDir folder(folderPath);
    if (!folder.exists()) {
        folder.mkpath(folderPath);
    }

    QString filename = (folderPath + "/sensor" + QString::number(index) +
                        QDate::currentDate().toString("_yyyy-MM-dd").append(".csv"));

    QFile csvFile(filename);
    QTextStream ts(&csvFile);

    if (!csvFile.exists()) {
        /* Write header for new csv file */
        if (csvFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            ts << "sep=" << sep << Qt::endl << "Time"
               << sep << "t, " << 'C' << Qt::endl;
            csvFile.close();
        }
    }

    /* Write data to csv file */
    if (!csvFile.isOpen())
        csvFile.open(QIODevice::Append | QIODevice::Text);

    ts << QTime::currentTime().toString("hh:mm:ss") << sep
       << QString::number(value, 'f', 1) << Qt::endl;

    csvFile.close();
    writedDevice++;
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
