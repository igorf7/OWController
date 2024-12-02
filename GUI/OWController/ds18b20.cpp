#include "ds18b20.h"
#include "ui_ds18b20.h"
#include "owdevice.h"
#include <QValidator>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDateTime>

DS18B20::DS18B20(DeviceWidget *parent) :
    DeviceWidget(parent), ui(new Ui::DS18B20)
{
    ui->setupUi(this);

    /* Connecting signals to slots */
    connect(ui->settingsPushButton, SIGNAL(clicked()),
            this, SLOT(onSettingsButtonClicked()));

}

DS18B20::~DS18B20()
{
    delete ui;
}

/**
 * @brief DS18B20::showDateTime
 * @param data
 */
void DS18B20::showDeviceData(quint8 *data, int index)
{
    quint8 *pData = data;
    quint64 address = *((quint64*)pData);

    if ((address != myAddress) || (index != myIndex)) return;

    quint8 resolution;

    pData += sizeof(myAddress);

    temperValue = *((float*)pData);
    pData += sizeof(temperValue);

    devAlarmHigh = *(pData + 0);
    devAlarmLow = *(pData + 1);
    resolution = *(pData + 2);

    switch (resolution) {
    case 0x1F:
        devResolution = 9;
        break;
    case 0x3F:
        devResolution = 10;
        break;
    case 0x5F:
        devResolution = 11;
        break;
    case 0x7F:
        devResolution = 12;
        break;
    default:
        break;
    }

    if (qIsNaN(temperValue)) {
        ui->temperValueLabel->setText(tr("FAILURE"));
        return;
    }
    else if (temperValue < 0.0f) {
        ui->temperValueLabel->setStyleSheet("color: blue");
    }
    else {
        ui->temperValueLabel->setStyleSheet("color: red");
    }
    ui->deviceIndexLabel->setText(QString::number(myIndex));
    ui->temperValueLabel->setText(QString::number(temperValue, 'f', 1) + " °C");

    if (cycleCounter == 0) this->writeCsvFile();
    if (++cycleCounter > 59) cycleCounter = 0;
}

/**
 * @brief DS18B20::setAddress
 * @param address
 */
void DS18B20::setAddress(quint64 address)
{
    myAddress = address;
}

/**
 * @brief DS18B20::setIndex
 * @param index
 */
void DS18B20::setIndex(int index)
{
    myIndex = index;
}

/**
 * @brief DS18B20::onSettingsButtonClicked
 */
void DS18B20::onSettingsButtonClicked()
{
    settingsWindow = new QDialog(this);

    settingsWindow->setWindowFlags((settingsWindow->windowFlags())
                                 & (~Qt::WindowContextHelpButtonHint));

    QPixmap pm = QPixmap(1, 1);
    pm.fill(QColor(0, 0, 0, 0));
    settingsWindow->setWindowIcon(QIcon(pm));

    settingsWindow->setWindowTitle(OWDevice::getName(devFamilyCode));
    settingsWindow->resize(this->window()->width(), 200);
    settingsWindow->setModal(true);

    settingsWindow->setAttribute(Qt::WA_DeleteOnClose);

    QVBoxLayout *vdlgLayout = new QVBoxLayout;
    QLabel *addressLabel = new QLabel;
    QLabel *descrLabel = new QLabel;
    descrLabel->setStyleSheet("color: green");
    QLabel *almHighLabel = new QLabel(tr("Alarm High, °C"));
    QLabel *almLowLabel = new QLabel(tr("Alarm Low, °C"));
    QLabel *resolutionLabel = new QLabel(tr("Resolution, bit"));
    almHighLineEdit = new QLineEdit;
    almLowLineEdit = new QLineEdit;
    resolutionLineEdit = new QLineEdit;
    QGridLayout *editLayout = new QGridLayout;

    almHighLineEdit->setFrame(false);
    almLowLineEdit->setFrame(false);
    resolutionLineEdit->setFrame(false);

    editLayout->addWidget(almHighLabel, 0, 0);
    editLayout->addWidget(almHighLineEdit, 0, 1);
    editLayout->addWidget(almLowLabel, 1, 0);
    editLayout->addWidget(almLowLineEdit, 1, 1);
    editLayout->addWidget(resolutionLabel, 2, 0);
    editLayout->addWidget(resolutionLineEdit, 2, 1);

    QPushButton *writeButton = new QPushButton(tr("Write"));
    QPushButton *readButton = new QPushButton(tr("Read"));
    QPushButton *closeButton = new QPushButton(tr("Close"));
    QHBoxLayout *hbtnLayout = new QHBoxLayout;

    hbtnLayout->addWidget(writeButton);
    hbtnLayout->addWidget(readButton);
    hbtnLayout->addWidget(closeButton);
    vdlgLayout->addWidget(addressLabel);
    vdlgLayout->addWidget(descrLabel);
    vdlgLayout->addLayout(editLayout);
    vdlgLayout->addLayout(hbtnLayout);
    settingsWindow->setLayout(vdlgLayout);

    connect(closeButton, SIGNAL(clicked()), this, SLOT(onCloseButtonClicked()));
    connect(writeButton, SIGNAL(clicked()), this, SLOT(onWriteButtonClicked()));
    connect(readButton, SIGNAL(clicked()), this, SLOT(onReadButtonClicked()));

    addressLabel->setText("Address: " + QString::number(myAddress, 16).toUpper());
    descrLabel->setText(OWDevice::getDescription(devFamilyCode));

    QIntValidator *alm_val = new QIntValidator(settingsWindow);
    QIntValidator *res_val = new QIntValidator(settingsWindow);
    alm_val->setRange(-128, 127);
    res_val->setRange(9, 12);
    almHighLineEdit->setValidator(alm_val);
    almLowLineEdit->setValidator(alm_val);
    resolutionLineEdit->setValidator(res_val);
    almHighLineEdit->setText(QString::number(devAlarmHigh));
    almLowLineEdit->setText(QString::number(devAlarmLow));
    resolutionLineEdit->setText(QString::number(devResolution));

    settingsWindow->show();
}

/**
 * @brief DS18B20::onWriteButtonClicked
 */
void DS18B20::onWriteButtonClicked()
{
    quint8 alm_high, alm_low,
           resolution;
    quint8 tx_data[11];

    if (almHighLineEdit->hasAcceptableInput()) {
        alm_high = (quint8)almHighLineEdit->text().toShort();
    }
    else {
        almHighLineEdit->setText(QString::number(devAlarmHigh));
        return;
    }
    if (almLowLineEdit->hasAcceptableInput()) {
        alm_low = (quint8)almLowLineEdit->text().toShort();
    }
    else {
        almLowLineEdit->setText(QString::number(devAlarmLow));
        return;
    }
    if (resolutionLineEdit->hasAcceptableInput()) {
        resolution = (quint8)resolutionLineEdit->text().toUShort();;
    }
    else {
        resolutionLineEdit->setText(QString::number(devResolution));
        return;
    }

    *((quint64*)tx_data) = myAddress;
    quint8 len  = sizeof(myAddress);
    tx_data[len++] = alm_high;
    tx_data[len++] = alm_low;

    switch (resolution) {
    case 9:
        resolution = 0x1F;
        break;
    case 10:
        resolution = 0x3F;
        break;
    case 11:
        resolution = 0x5F;
        break;
    case 12:
        resolution = 0x7F;
        break;
    default:
        break;
    }

    tx_data[len++] = resolution;

    almHighLineEdit->clear();
    almLowLineEdit->clear();
    resolutionLineEdit->clear();

    devAlarmHigh = devAlarmLow = devResolution = 0;

    emit sendCommand(eWriteCmd, tx_data, len);
}

/**
 * @brief DS18B20::onReadButtonClicked
 */
void DS18B20::onReadButtonClicked()
{
    if (settingsWindow != nullptr) {
        almHighLineEdit->setText(QString::number(devAlarmHigh));
        almLowLineEdit->setText(QString::number(devAlarmLow));
        resolutionLineEdit->setText(QString::number(devResolution));
    }
}

/**
 * @brief DS18B20::onCloseButtonClicked
 */
void DS18B20::onCloseButtonClicked()
{
    if (settingsWindow != nullptr) {
        settingsWindow->close();
        delete settingsWindow;
    }
}

/**
 * @brief DS18B20::writeCsvFile
 */
void DS18B20::writeCsvFile()
{
    QString folderPath = QDir::currentPath() + "/DS18B20_CSV";
    QDir folder = folderPath;
    if (!folder.exists()) folder.mkdir(folderPath);

    QString filename = (folderPath + "/sensor" + QString::number(myIndex) +
                        "_" + QDate::currentDate().toString("yyyy-MM-dd").append(".csv"));

    QFile file(filename);
    QTextStream ts(&file);
    ts.setDevice(&file);

    if (!file.exists()) {
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            // Write header for new csv file
            ts << "sep=" << column_sep << Qt::endl
               << "Time" << column_sep << "t, C" << Qt::endl;
            file.close();
        }
    }
    // Write data to csv file
    file.open(QIODevice::Append | QIODevice::Text);
    ts << QTime::currentTime().toString("hh:mm:ss") << column_sep
       << QString::number(temperValue, 'f', 1) << Qt::endl;
    file.close();
}
