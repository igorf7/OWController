#include "ds1971.h"
#include "ui_ds1971.h"
#include "owdevice.h"
#include <QLineEdit>
#include <QMessageBox>
#include <QDebug>

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
void DS1971::showDeviceData(quint8 *data, int index)
{
    quint8 *pData = data;

    deviceAddress = *((quint64*)pData);
    pData += sizeof(deviceAddress);

    for (auto i = 0; i < DS1971_MEMORY_SIZE; i++) {
        ds1971memory[i] = *pData;
        pData++;
    }

    ui->deviceIndexLabel->setText(QString::number(index));
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

    memEdit = new QPlainTextEdit;
    QLabel *memLabel = new QLabel("Device memory (can be edited and rewritten):");

    QPushButton *writeButton = new QPushButton("Write");
    QPushButton *readButton = new QPushButton("Read");
    QPushButton *closeButton = new QPushButton("Close");
    QHBoxLayout *hbtnLayout = new QHBoxLayout;

    hbtnLayout->addWidget(writeButton);
    hbtnLayout->addWidget(readButton);
    hbtnLayout->addWidget(closeButton);
    vdlgLayout->addWidget(addressLabel);
    vdlgLayout->addWidget(descrLabel);
    vdlgLayout->addWidget(memLabel);
    vdlgLayout->addWidget(memEdit);
    vdlgLayout->addStretch();
    vdlgLayout->addLayout(hbtnLayout);
    settingsWindow->setLayout(vdlgLayout);

    connect(closeButton, SIGNAL(clicked()), this, SLOT(onCloseButtonClicked()));
    connect(writeButton, SIGNAL(clicked()), this, SLOT(onWriteButtonClicked()));
    connect(readButton, SIGNAL(clicked()), this, SLOT(onReadButtonClicked()));

    addressLabel->setText("Address: " + QString::number(deviceAddress, 16).toUpper());
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

    *((quint64*)tx_data) = deviceAddress;
    quint8 len  = sizeof(deviceAddress);

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
    emit sendCommand(eWriteCmd, tx_data, len);
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
