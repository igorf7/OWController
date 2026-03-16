#include "devicewidget.h"
#include "ui_devicewidget.h"

DeviceWidget::DeviceWidget(CardView *parent)
    : CardView(parent), ui(new Ui::DeviceWidget)
{
    ui->setupUi(this);

    connect(ui->settingsButton, SIGNAL(clicked()),
            this, SLOT(onSettingsButtonClicked()));
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
