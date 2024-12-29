#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QFile>
#include <QDateTime>
#include <QTextStream>
#include <QSettings>

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

    readSettings();

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
    connect(ui->settingsPushButton, SIGNAL(clicked()),
            this, SLOT(onSettingsButtonClicked()));

    /* Activate status bar */
    QFont font;
    font.setItalic(true);
    statusBar()->setFont(font);

    //this->resize(260, 310);

    /* Attempting to connect a USB device */
    onConnectButtonClicked();
}

/**
 * @brief MainWindow Class Destructor
 */
MainWindow::~MainWindow()
{
    writeSettings();

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
        statusBar()->showMessage(tr("No USB connection"));
    }
    else {
        isConnected = hidDevice->Connect(ProductString);

        if (!isConnected) {
            this->deinitWidgets();
            ui->connectPushButton->setIcon(QIcon(":/images/switch-off.png"));
            statusBar()->showMessage(tr("USB device not found"));
            return;
        }

        ui->searchPushButton->setEnabled(true);
        ui->clockPushButton->setEnabled(true);
        ui->connectPushButton->setIcon(QIcon(":/images/switch-on.png"));
        statusBar()->showMessage(tr("USB device connected"));
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
    this->onSendCommand(eSearchCmd, nullptr, 0);
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
    settingsWindow = new QDialog(this);

    settingsWindow->setWindowFlags((settingsWindow->windowFlags())
                                   & (~Qt::WindowContextHelpButtonHint));

    QPixmap pm = QPixmap(1, 1);
    pm.fill(QColor(0, 0, 0, 0));
    settingsWindow->setWindowIcon(QIcon(pm));

    settingsWindow->resize(this->window()->width(), 200);
    settingsWindow->setModal(true);

    settingsWindow->setAttribute(Qt::WA_DeleteOnClose);

    QVBoxLayout *vdlgLayout = new QVBoxLayout;
    writeFileCheckbox = new QCheckBox(tr("Write CSV file"));
    writeFileCheckbox->setChecked(true);
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
    QLabel *writePeriodLabel = new QLabel(tr("Write File Period"));
    QLabel *pollingPeriodLabel = new QLabel(tr("1-Wire Bus Polling Period"));
    QPushButton *closeButton = new QPushButton(tr("Save and Close"));

    QHBoxLayout *h1Layout = new QHBoxLayout;
    h1Layout->addWidget(pollingPeriodLabel);
    h1Layout->addWidget(pollingPeriodSpinbox);
    h1Layout->addStretch();

    QHBoxLayout *h2Layout = new QHBoxLayout;
    h2Layout->addWidget(writeFileCheckbox);
    h2Layout->addStretch();
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
        isWriteFileEnabled = writeFileCheckbox->isChecked();

        if (writeFilePeriodSpinbox->value() < writeFilePeriod) {
            secCounter = 0;
        }
        writeFilePeriod = writeFilePeriodSpinbox->value();

        if (owPollingPeriod != pollingPeriodSpinbox->value()) {
            owPollingPeriod = pollingPeriodSpinbox->value();
            quint8 dev_family = OWDevice::getFamily(ui->deviceComboBox->currentText());
            quint8 data[2];
            data[0] = dev_family;
            data[1] = (quint8)owPollingPeriod;
            this->onSendCommand(eReadCmd, data, sizeof(data));
        }

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
        usbPollingPeriod = startTimer(10);
        secCounterEvent = startTimer(1000);
    }
}

/**
 * @brief MainWindow::stopUsbPolling
 */
void MainWindow::stopUsbPolling()
{
    killTimer(usbPollingPeriod);
    usbPollingPeriod = 0;
    isPollingRunning = false;
}

/**
 * @brief MainWindow::timerEvent
 * @param event
 */
void MainWindow::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == usbPollingPeriod) {
        int len = hidDevice->Read(rxUsbBuffer, USB_BUFF_SIZE);
        if (len < 0) { // Lost connection
            if (isConnected) this->onConnectButtonClicked();
            return;
        }
        else if (len != 0) {
            handleReceivedPacket();
        }
    }
    if (event->timerId() == secCounterEvent) {
        if (++secCounter == writeFilePeriod) {
            secCounter = 0;
            writedDevice = 0;
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

    for (int i = 0; i < allDeviceAddressList.size(); ++i) {
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

    quint8 dev_family = OWDevice::getFamily(ui->deviceComboBox->currentText());

    for (int i = 0, j = 0; i < allDeviceAddressList.size(); ++i) {
        if (dev_family == (allDeviceAddressList.at(i) & 0xFF)) {
            selDevices.insert(allDeviceAddressList.at(i), j++);
        }
    }

    ui->deviceCountLabel->setText(QString::number(selDevices.size()));

    this->createWidgetsLayout(selDevices.size());

    quint8 data[2];
    data[0] = dev_family;
    data[1] = (quint8)owPollingPeriod;
    this->onSendCommand(eReadCmd, data, sizeof(data));
}

/**
 * @brief MainWindow::createWidgetsLayout
 */
void MainWindow::createWidgetsLayout(int count)
{
    if (count < 1) return;

    deviceWidget.clear();

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
            else {
                statusBar()->showMessage(tr("Total 1-Wre devices found: ") +
                                            QString::number(allDeviceAddressList.size()), 5000);
            }
            break;

        case eReadCmd:
            if (!isShowClockEnabled && !deviceWidget.isEmpty()) {
                int index = selDevices.value(dev_addr);
                deviceWidget.at(index)->showDeviceData(rx_packet->data, index + 1);
            }
            if (ui->deviceComboBox->currentText() == "DS18B20") {
                int index = selDevices.value(dev_addr);
                float value = *((float*)(rx_packet->data + sizeof(dev_addr)));
                this->writeCsvFile(value, index+1);
            }
            break;

        case eWriteCmd:
            break;

        case eGetRtcCmd:
            if (isShowClockEnabled) {
                timeStamp = *((quint32*)rx_packet->data);
                quint64 unixtime = QDateTime::currentSecsSinceEpoch();
                qint64 dif = (qint64)(unixtime - timeStamp);
                if (qAbs(dif) > 2) {
                    timeStamp = unixtime + 1;
                    this->onSendCommand(eSyncRtcCmd, (quint8*)&timeStamp, sizeof(timeStamp));
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
    if (isWriteFileEnabled && (writedDevice < selDevices.size()))
    {
        QString folderPath = QDir::currentPath() + "/DS18B20_CSV";
        QDir folder = folderPath;
        if (!folder.exists()) {
            folder.mkdir(folderPath);
        }

        QString filename = (folderPath + "/sensor" + QString::number(index) +
                            QDate::currentDate().toString("_yyyy-MM-dd").append(".csv"));

        QFile file(filename);
        QTextStream ts(&file);

        if (!file.exists()) {
            /* Write header for new csv file */
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                ts << "sep=" << column_sep << Qt::endl
                   << "Time" << column_sep << "t, " << QChar(176) << 'C' << Qt::endl;
                file.close();
            }
        }

        /* Write data to csv file */
        file.open(QIODevice::Append | QIODevice::Text);
        ts << QTime::currentTime().toString("hh:mm:ss") << column_sep
           << QString::number(value, 'f', 1) << Qt::endl;
        file.close();
        writedDevice++;
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
    ui->deviceCountLabel->setText("0");
    ui->deviceComboBox->clear();
    ui->deviceComboBox->setCurrentIndex(-1);
    ui->searchPushButton->setEnabled(false);
    ui->clockPushButton->setEnabled(false);
}

/**
 * @brief MainWindow::readSettings
 */
void MainWindow::readSettings()
{
    QSettings settings("settings.ini", QSettings::IniFormat);

    settings.beginGroup("/Settings");
    owPollingPeriod = settings.value("/PollPeriod", 1).toInt();
    writeFilePeriod = settings.value("/WriteFilePeriod", 60).toInt();
    settings.endGroup();
}

/**
 * @brief MainWindow::writeSettings
 */
void MainWindow::writeSettings()
{
    QSettings settings("settings.ini", QSettings::IniFormat);

    settings.beginGroup("/Settings");
    settings.setValue("/PollPeriod", owPollingPeriod);
    settings.setValue("/WriteFilePeriod", writeFilePeriod);
    settings.endGroup();
}
