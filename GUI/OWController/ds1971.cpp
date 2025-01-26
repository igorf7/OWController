#include "ds1971.h"
#include "owdevice.h"
#include <QApplication>
#include <QIcon>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QClipboard>
#include <QMessageBox>
//#include <QDebug>

/**
 * @brief DS1971::DS1971
 * @param parent
 */
DS1971::DS1971(DeviceWidget *parent) : DeviceWidget(parent)
{
#ifdef __ANDROID__
    setPointNameLabelFontSize(16);
    setPrmNameLabelFontSize(24);
    setPrmValueLabelFontSize(32);
#else
    setPointNameLabelFontSize(10);
    setPrmNameLabelFontSize(10);
    setPrmValueLabelFontSize(12);
#endif

    setParameterName("Address:");
}

/**
 * @brief DS1971::~DS1971
 */
DS1971::~DS1971()
{
}

/**
 * @brief DS1971::showDeviceData
 * @param data
 * @param index
 */
void DS1971::showDeviceData(quint8 *data, int index)
{
    quint8 *pData = data;
    quint64 address = *((quint64*)pData);

    if ((address != getDeviceAddress()) || (index != getDeviceIndex())) return;

    pData += sizeof(address);

    for (auto i = 0; i < DS1971_MEMORY_SIZE; i++) {
        ds1971memory[i] = *pData;
        pData++;
    }

    QString str = tr("Device ") + QString::number(getDeviceIndex());
    setPointName(str);
    str = QString::number(getDeviceAddress(), 16).toUpper();
    setParameterValue(str);
}

/**
 * @brief DS1971::onSettingsButtonClicked
 */
void DS1971::onSettingsButtonClicked()
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

    memEdit = new QPlainTextEdit;
    QLabel *memLabel = new QLabel(tr("Device memory (can be edited and rewritten):"));

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
    vdlgLayout->addWidget(memLabel);
    vdlgLayout->addWidget(memEdit);
    vdlgLayout->addStretch();
    vdlgLayout->addLayout(hbtnLayout);
    settingsWindow->setLayout(vdlgLayout);

    connect(copyButton, SIGNAL(clicked()), this, SLOT(onCopyButtonnClicked()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(onCloseButtonClicked()));
    connect(writeButton, SIGNAL(clicked()), this, SLOT(onWriteButtonClicked()));
    connect(readButton, SIGNAL(clicked()), this, SLOT(onReadButtonClicked()));

    addressLabel->setText("Address: " + QString::number(getDeviceAddress(), 16).toUpper());
    descrLabel->setText(OWDevice::getDescription(devFamilyCode));

    this->showDeviceMemory();

    settingsWindow->show();
}

/**
 * @brief DS1971::showDeviceMemory
 */
void DS1971::showDeviceMemory()
{
    memEdit->clear();
    QString output;
    for (auto i = 0; i < DS1971_MEMORY_SIZE; i += MEM_EDITOR_LINE_LEN)
    {
        for (auto j = 0; j < MEM_EDITOR_LINE_LEN; j++)
        {
            if (ds1971memory[j+i] < 16) output.append("0");
            output.append(QString::number(ds1971memory[j+i], 16).toUpper());
            output.append(" ");
        }
        memEdit->appendPlainText(output);
        output.clear();
    }
}

/**
 * @brief DS1971::onCopyButtonnClicked
 */
void DS1971::onCopyButtonnClicked()
{
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText(QString::number(getDeviceAddress(), 16).toUpper());
}

/**
 * @brief DS1971::onReadButtonClicked
 */
void DS1971::onReadButtonClicked()
{
    memEdit->clear();
    this->showDeviceMemory();
}

/**
 * @brief DS1971::onWriteButtonClicked
 */
void DS1971::onWriteButtonClicked()
{
    quint8 tx_data[44];

    quint64 address = getDeviceAddress();
    *((quint64*)tx_data) = address;
    quint8 len  = sizeof(address);

    QString str = memEdit->toPlainText().simplified();
    QStringList str_list = str.split(' ');
    bool ok = false;
    for (auto i = 0; i < DS1971_MEMORY_SIZE; i++) {
        tx_data[len + i] = str_list.at(i).toUInt(&ok, 16);

        if (!ok) {
            QMessageBox::critical(this, tr("Error!"), tr("Incorrect input."));
            return;
        }
    }

    memEdit->clear();
    len += DS1971_MEMORY_SIZE;
    emit sendCommand(eOwWriteData, tx_data, len);
}

/**
 * @brief DS1971::onCloseButtonClicked
 */
void DS1971::onCloseButtonClicked()
{
    if (settingsWindow != nullptr) {
        settingsWindow->close();
        delete settingsWindow;
        settingsWindow = nullptr;
    }
}
