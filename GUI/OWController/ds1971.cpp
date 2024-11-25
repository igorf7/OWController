#include "ds1971.h"
#include "ui_ds1971.h"
#include "owdevice.h"
#include <QLineEdit>
#include <QTextEdit>

DS1971::DS1971(CardView *parent)
    : CardView(parent), ui(new Ui::DS1971)
{
    ui->setupUi(this);

    /* Connecting signals to slots */
    connect(ui->settingsPushButton, SIGNAL(clicked()),
            this, SLOT(onSettingsButtonClicked()));
}

DS1971::~DS1971()
{
    delete ui;
}

/**
 * @brief DS1971::showDeviceData
 * @param addr
 */
void DS1971::showDeviceData(quint8 *data)
{
    deviceAddress = *((quint64*)data);

    devIndex = *(data + sizeof(deviceAddress));

    ui->deviceIndexLabel->setText(QString::number(devIndex));
    ui->addrLabel->setText(QString::number(deviceAddress, 16).toUpper());
}

/**
 * @brief DS1971::onSettingsButtonClicked
 */
void DS1971::onSettingsButtonClicked()
{
    settingsWindow = new QDialog(this);

    settingsWindow->setWindowFlags((settingsWindow->windowFlags())
                                   & (~Qt::WindowContextHelpButtonHint));

    settingsWindow->setWindowTitle(OWDevice::getName(devFamilyCode));
    settingsWindow->resize(this->window()->width(), 150);
    settingsWindow->setModal(true);

    settingsWindow->setAttribute(Qt::WA_DeleteOnClose);

    QVBoxLayout *vdlgLayout = new QVBoxLayout;
    QLabel *addressLabel = new QLabel;
    QLabel *descrLabel = new QLabel;
    descrLabel->setStyleSheet("color: green");

    QTextEdit *memEdit = new QTextEdit;

    QPushButton *writeButton = new QPushButton("Write");
    QPushButton *readButton = new QPushButton("Read");
    QPushButton *closeButton = new QPushButton("Close");
    QHBoxLayout *hbtnLayout = new QHBoxLayout;

    hbtnLayout->addWidget(writeButton);
    hbtnLayout->addWidget(readButton);
    hbtnLayout->addWidget(closeButton);
    vdlgLayout->addWidget(addressLabel);
    vdlgLayout->addWidget(descrLabel);
    vdlgLayout->addWidget(memEdit);
    vdlgLayout->addLayout(hbtnLayout);
    settingsWindow->setLayout(vdlgLayout);

    connect(closeButton, SIGNAL(clicked()), this, SLOT(onCloseButtonClicked()));

    addressLabel->setText("Address: " + QString::number(deviceAddress, 16).toUpper());
    descrLabel->setText(OWDevice::getDescription(devFamilyCode));

    settingsWindow->show();
}

/**
 * @brief DS1971::onCloseButtonClicked
 */
void DS1971::onCloseButtonClicked()
{
    if (settingsWindow != nullptr) {
        settingsWindow->close();
        delete settingsWindow;
    }
}
