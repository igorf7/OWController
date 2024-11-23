#include "ds18s20.h"
#include "ui_ds18s20.h"

DS18S20::DS18S20(CardView *parent)
    : CardView(parent), ui(new Ui::DS18S20)
{
    ui->setupUi(this);
}

DS18S20::~DS18S20()
{
    delete ui;
}

/**
 * @brief DS18S20::showAddress
 * @param addr
 */
void DS18S20::showAddress(quint64 &addr)
{
    ui->addrLabel->setText(QString::number(addr, 16).toUpper());
}

/**
 * @brief DS18S20::showValue
 * @param value
 */
void DS18S20::showValue(float value)
{
    if (value < 0.0f) {
        ui->temperValueLabel->setStyleSheet("color: blue");
    }
    else {
        ui->temperValueLabel->setStyleSheet("color: red");
    }
    ui->temperValueLabel->setText(QString::number(value, 'f', 1) + " °C");
}
