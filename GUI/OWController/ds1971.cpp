#include "ds1971.h"
#include "ui_ds1971.h"
#include "owdevice.h"
#include <QLineEdit>
#include <QMessageBox>
#include <QClipboard>

DS1971::DS1971(DeviceWidget *parent)
    : DeviceWidget(parent), ui(new Ui::DS1971)
{
    ui->setupUi(this);

#ifdef __ANDROID__
    QFont font;
    font.setPixelSize(16);
    ui->pointNameLabel->setFont(font);
    font.setPixelSize(18);
    ui->prmNameLabel->setFont(font);
    font.setPixelSize(20);
    ui->prmValueLabel->setFont(font);
#endif

    ui->prmNameLabel->setText(tr("Address:"));

    /* Connecting signals to slots */
    connect(ui->settingsPushButton, SIGNAL(clicked()),
            this, SLOT(onSettingsButtonClicked()));
}

DS1971::~DS1971()
{
    delete ui;
}

/**
 * @brief DS1971::showDateTime
 * @param addr
 */
void DS1971::showDeviceData(quint8 *data, int index)
{
    quint8 *pData = data;
    quint64 address = *((quint64*)pData);

    if ((address != myAddress) || (index != myIndex)) return;

    pData += sizeof(myAddress);

    for (auto i = 0; i < DS1971_MEMORY_SIZE; i++) {
        ds1971memory[i] = *pData;
        pData++;
    }

    ui->pointNameLabel->setText(tr("Device ") + QString::number(myIndex));
    ui->prmValueLabel->setText(QString::number(myAddress, 16).toUpper());
}

/**
 * @brief DS1971::setWriteFilePeriod
 * @param enabled
 * @param period
 */
void DS1971::setWriteFilePeriod(bool enabled, int period)
{
    Q_UNUSED(enabled)
    Q_UNUSED(period)
}

/**
 * @brief DS1971::setAddress
 * @param address
 */
void DS1971::setAddress(quint64 address)
{
    myAddress = address;
}

/**
 * @brief DS1971::setIndex
 * @param index
 */
void DS1971::setIndex(int index)
{
    myIndex = index;
}

/**
 * @brief DS1971::onSettingsButtonClicked
 */
void DS1971::onSettingsButtonClicked()
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

    addressLabel->setText("Address: " + QString::number(myAddress, 16).toUpper());
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
    clipboard->setText(QString::number(myAddress, 16).toUpper());
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

    *((quint64*)tx_data) = myAddress;
    quint8 len  = sizeof(myAddress);

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
        settingsWindow = nullptr;
    }
}
