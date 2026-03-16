#include "ds_other.h"
#include "owdevice.h"
#include <QApplication>
#include <QIcon>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QClipboard>

/**
 * @brief DS_OTHER::DS_OTHER
 * @param parent
 */
DS_OTHER::DS_OTHER(DeviceWidget *parent) : DeviceWidget(parent)
{
    setParameterName("");
}

/**
 * @brief DS_OTHER::~DS_OTHER
 */
DS_OTHER::~DS_OTHER()
{
}

/**
 * @brief DS_OTHER::showDeviceData
 * @param data
 * @param index
 */
void DS_OTHER::showDeviceData(quint8 *data, int index)
{
    quint8 *pData = data;
    quint64 address = *((quint64*)pData);

    if ((address != getDeviceAddress()) || (index != getDeviceIndex())) return;

    QString str = tr("Device ") + QString::number(getDeviceIndex());
    setPointName(str);
    str = QString::number(getDeviceAddress(), 16).toUpper();
    setParameterValue(str);
}

/**
 * @brief DS_OTHER::onSettingsButtonClicked
 */
void DS_OTHER::onSettingsButtonClicked()
{
    settingsWindow = new QDialog;

    settingsWindow->setWindowFlags((settingsWindow->windowFlags())
                                   & (~Qt::WindowContextHelpButtonHint));

    QPixmap pm = QPixmap(1, 1);
    pm.fill(QColor(0, 0, 0, 0));
    settingsWindow->setWindowIcon(QIcon(pm));

    quint8 family_code = (quint8)(getDeviceAddress() & 0xFF);
    settingsWindow->setWindowTitle(OWDevice::getName(family_code));
    settingsWindow->setModal(true);

    settingsWindow->setAttribute(Qt::WA_DeleteOnClose);

    QPushButton *copyButton = new QPushButton;
    copyButton->setFlat(true);
    copyButton->setIcon(QIcon(":/images/copy.png"));
    copyButton->setToolTip(tr("Copy address to clipboard"));
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

    addressLabel->setText(tr("Address: ") + QString::number(getDeviceAddress(), 16).toUpper());
    descrLabel->setText(OWDevice::getDescription(family_code));

    settingsWindow->show();
}

/**
 * @brief DS_OTHER::onCopyButtonnClicked
 */
void DS_OTHER::onCopyButtonnClicked()
{
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(QString::number(getDeviceAddress(), 16).toUpper());
}

/**
 * @brief DS_OTHER::onCloseButtonClicked
 */
void DS_OTHER::onCloseButtonClicked()
{
    if (settingsWindow != nullptr) {
        settingsWindow->close();
        delete settingsWindow;
        settingsWindow = nullptr;
    }
}
