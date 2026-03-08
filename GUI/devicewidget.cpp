#include "devicewidget.h"
#include "ui_devicewidget.h"

DeviceWidget::DeviceWidget(CardView *parent)
    : CardView(parent), ui(new Ui::DeviceWidget)
{
    ui->setupUi(this);

    connect(ui->settingsPushButton, SIGNAL(clicked()),
            this, SLOT(onSettingsButtonClicked()));

    deviceFont.setFamily("Segoe UI");
}

DeviceWidget::~DeviceWidget()
{
    delete ui;
}
/**
 * @brief setPointName
 * @param poitName
 */
void DeviceWidget::setPointName(const QString &poitName)
{
    ui->pointNameLabel->setText(poitName);
}

/**
 * @brief DeviceWidget::setParameterName
 * @param prmName
 */
void DeviceWidget::setParameterName(const QString &prmName)
{
    ui->prmNameLabel->setText(prmName);
}

void DeviceWidget::setParameterValue(const QString &prmValue)
{
    ui->prmValueLabel->setText(prmValue);
}

/**
 * @brief DeviceWidget::setPrmValueLabelStyle
 * @param prmStyle
 */
void DeviceWidget::setPrmValueLabelStyle(const QString &prmStyle)
{
    ui->prmValueLabel->setStyleSheet(prmStyle);
}

/**
 * @brief DeviceWidget::setPointNameLabelFontSize
 * @param size
 */
void DeviceWidget::setPointNameLabelFontSize(int size)
{

    deviceFont.setPointSize(size);
    ui->pointNameLabel->setFont(deviceFont);
}

/**
 * @brief DeviceWidget::setPrmNameLabelFontSize
 * @param size
 */
void DeviceWidget::setPrmNameLabelFontSize(int size)
{
    deviceFont.setPointSize(size);
    ui->prmNameLabel->setFont(deviceFont);
}

/**
 * @brief DeviceWidget::setPrmValueLabelFontSize
 * @param size
 */
void DeviceWidget::setPrmValueLabelFontSize(int size)
{
    deviceFont.setPointSize(size);
    ui->prmValueLabel->setFont(deviceFont);
}
