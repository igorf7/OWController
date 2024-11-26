#include "ds_other.h"
#include "ui_ds_other.h"
#include "owdevice.h"

DS_OTHER::DS_OTHER(CardView *parent)
    : CardView(parent), ui(new Ui::DS_OTHER)
{
    ui->setupUi(this);

    /* Connecting signals to slots */
    connect(ui->settingsPushButton, SIGNAL(clicked()),
            this, SLOT(onSettingsButtonClicked()));
}

DS_OTHER::~DS_OTHER()
{
    delete ui;
}

/**
 * @brief DS_OTHER::showAddress
 * @param addr
 */
void DS_OTHER::showDeviceData(quint8 *data, int index)
{
    deviceAddress = *((quint64*)data);

    devFamilyCode = (quint8)(deviceAddress & 0xFF);

    ui->deviceIndexLabel->setText(QString::number(index));
    ui->addrLabel->setText(QString::number(deviceAddress, 16).toUpper());
}

/**
 * @brief DS_OTHER::onSettingsButtonClicked
 */
void DS_OTHER::onSettingsButtonClicked()
{
    settingsWindow = new QDialog(this);

    settingsWindow->setWindowFlags((settingsWindow->windowFlags())
                                   & (~Qt::WindowContextHelpButtonHint));

    QPixmap pm = QPixmap(1, 1);
    pm.fill(QColor(0, 0, 0, 0));
    settingsWindow->setWindowIcon(QIcon(pm));

    settingsWindow->setWindowTitle(OWDevice::getName(devFamilyCode));
    settingsWindow->resize(this->window()->width(), 150);
    settingsWindow->setModal(true);


    settingsWindow->setAttribute(Qt::WA_DeleteOnClose);

    QVBoxLayout *vdlgLayout = new QVBoxLayout;
    QLabel *addressLabel = new QLabel;
    QLabel *descrLabel = new QLabel;
    descrLabel->setStyleSheet("color: green");

    vdlgLayout->addWidget(addressLabel);
    vdlgLayout->addWidget(descrLabel);
    settingsWindow->setLayout(vdlgLayout);

    addressLabel->setText("Address: " + QString::number(deviceAddress, 16).toUpper());
    descrLabel->setText(OWDevice::getDescription(devFamilyCode));

    settingsWindow->show();
}

/**
 * @brief DS_OTHER::onCloseButtonClicked
 */
void DS_OTHER::onCloseButtonClicked()
{
    if (settingsWindow != nullptr) {
        settingsWindow->close();
        delete settingsWindow;
    }
}
