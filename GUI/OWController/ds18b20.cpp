#include "ds18b20.h"
#include "owdevice.h"
#include <QApplication>
#include <QClipboard>
#include <QIcon>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QIntValidator>
//#include <QDebug>

/**
 * @brief DS18B20::DS18B20
 * @param parent
 */
DS18B20::DS18B20(DeviceWidget *parent) : DeviceWidget(parent)
{

#ifdef __ANDROID__
    setPointNameLabelFontSize(18);
    setPrmNameLabelFontSize(26);
    setPrmValueLabelFontSize(34);
#else
    setPointNameLabelFontSize(10);
    setPrmNameLabelFontSize(14);
    setPrmValueLabelFontSize(18);
#endif

    setParameterName("Temperature:");
}

/**
 * @brief DS18B20::~DS18B20
 */
DS18B20::~DS18B20()
{
}

/**
 * @brief DS18B20::showDeviceData
 * @param data
 * @param index
 */
void DS18B20::showDeviceData(quint8 *data, int index)
{
    quint8 *pData = data;
    quint64 address = *((quint64*)pData);

    if ((address != getDeviceAddress()) || (index != getDeviceIndex())) return;

    quint8 resolution;

    pData += sizeof(address);

    temperValue = *((float*)pData);
    pData += sizeof(temperValue);

    devAlarmHigh = *(pData + 0);
    devAlarmLow = *(pData + 1);
    resolution = *(pData + 2);

    switch (resolution)
    {
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

    if (temperValue < 0.0f) {
        setPrmValueLabelStyle("color: blue");
    }
    else {
        setPrmValueLabelStyle("color: red");
    }

    QString str = tr("Sensor ") + QString::number(getDeviceIndex());

    setPointName(str);

    if (qIsNaN(temperValue)) {
        str = tr("FAILURE");
    }
    else {
        str = QString::number(temperValue, 'f', 1) + " °C";
    }

    setParameterValue(str);
}

/**
 * @brief DS18B20::onSettingsButtonClicked
 */
void DS18B20::onSettingsButtonClicked()
{
    settingsWindow = new QDialog;

    settingsWindow->setWindowFlags((settingsWindow->windowFlags())
                                   & (~Qt::WindowContextHelpButtonHint));

    QPixmap pm = QPixmap(1, 1);
    pm.fill(QColor(0, 0, 0, 0));
    settingsWindow->setWindowIcon(QIcon(pm));

    settingsWindow->setWindowTitle(OWDevice::getName(devFamilyCode));
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

    QPushButton *copyButton = new QPushButton;
    copyButton->setFlat(true);
    copyButton->setIcon(QIcon(":/images/copy.png"));
    copyButton->setToolTip(tr("Copy address to clipboard"));
    QPushButton *writeButton = new QPushButton(tr("Write"));
    QPushButton *readButton = new QPushButton(tr("Read"));
    QPushButton *closeButton = new QPushButton(tr("Close"));
    QHBoxLayout *hbtnLayout = new QHBoxLayout;

    QHBoxLayout *haddrLayout = new QHBoxLayout;
    haddrLayout->addWidget(addressLabel);
    haddrLayout->addWidget(copyButton);
    haddrLayout->addStretch();

    hbtnLayout->addWidget(writeButton);
    hbtnLayout->addWidget(readButton);
    hbtnLayout->addWidget(closeButton);
    vdlgLayout->addLayout(haddrLayout);
    vdlgLayout->addWidget(descrLabel);
    vdlgLayout->addLayout(editLayout);
    vdlgLayout->addLayout(hbtnLayout);
    settingsWindow->setLayout(vdlgLayout);

    connect(copyButton, SIGNAL(clicked()), this, SLOT(onCopyButtonnClicked()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(onCloseButtonClicked()));
    connect(writeButton, SIGNAL(clicked()), this, SLOT(onWriteButtonClicked()));
    connect(readButton, SIGNAL(clicked()), this, SLOT(onReadButtonClicked()));

    addressLabel->setText("Address: " + QString::number(getDeviceAddress(), 16).toUpper());
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
 * @brief DS18B20::onCopyButtonnClicked
 */
void DS18B20::onCopyButtonnClicked()
{
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(QString::number(getDeviceAddress(), 16).toUpper());
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

    quint64 address = getDeviceAddress();
    *((quint64*)tx_data) = address;
    quint8 len  = sizeof(address);
    tx_data[len++] = alm_high;
    tx_data[len++] = alm_low;

    switch (resolution)
    {
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

    emit sendCommand(eOwWriteData, tx_data, len);
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
        settingsWindow = nullptr;
    }
}
