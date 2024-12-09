#include "ds_other.h"
#include "ui_ds_other.h"
#include "owdevice.h"
#include <QClipboard>

DS_OTHER::DS_OTHER(DeviceWidget *parent)
    : DeviceWidget(parent), ui(new Ui::DS_OTHER)
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
    quint8 *pData = data;
    quint64 address = *((quint64*)pData);

    if ((address != myAddress) || (index != myIndex)) return;

    devFamilyCode = (quint8)(myAddress & 0xFF);

    ui->deviceIndexLabel->setText(QString::number(myIndex));
    ui->addrLabel->setText(QString::number(myAddress, 16).toUpper());
}

/**
 * @brief DS_OTHER::setAddress
 * @param address
 */
void DS_OTHER::setAddress(quint64 address)
{
    myAddress = address;
}

/**
 * @brief DS_OTHER::setIndex
 * @param index
 */
void DS_OTHER::setIndex(int index)
{
    myIndex = index;
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
    settingsWindow->resize(this->window()->width(), 200);
    settingsWindow->setModal(true);


    settingsWindow->setAttribute(Qt::WA_DeleteOnClose);

    QPushButton *copyButton = new QPushButton;
    copyButton->setFlat(true);
    copyButton->setIcon(QIcon(":/images/copy.png"));
    copyButton->setToolTip(tr("Copy to clipboard"));
    QVBoxLayout *vdlgLayout = new QVBoxLayout;
    QLabel *addressLabel = new QLabel;
    QLabel *descrLabel = new QLabel;
    QPushButton *closeButton = new QPushButton(tr("Close"));
    descrLabel->setStyleSheet("color: green");

    QHBoxLayout *haddrLayout = new QHBoxLayout;
    haddrLayout->addWidget(addressLabel);
    haddrLayout->addWidget(copyButton);
    haddrLayout->addStretch();

    vdlgLayout->addLayout(haddrLayout);
    vdlgLayout->addWidget(descrLabel);
    vdlgLayout->addWidget(closeButton);
    settingsWindow->setLayout(vdlgLayout);

    connect(copyButton, SIGNAL(clicked()), this, SLOT(onCopyButtonnClicked()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(onCloseButtonClicked()));

    addressLabel->setText(tr("Address: ") + QString::number(myAddress, 16).toUpper());
    descrLabel->setText(OWDevice::getDescription(devFamilyCode));

    settingsWindow->show();
}

/**
 * @brief DS_OTHER::onCopyButtonnClicked
 */
void DS_OTHER::onCopyButtonnClicked()
{
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(QString::number(myAddress, 16).toUpper());
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
