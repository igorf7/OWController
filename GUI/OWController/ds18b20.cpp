#include "ds18b20.h"
#include "ui_ds18b20.h"

DS18B20::DS18B20(CardView *parent) :
    CardView(parent), ui(new Ui::DS18B20)
{
    ui->setupUi(this);
}

DS18B20::~DS18B20()
{
    delete ui;
}

/**
 * @brief DS18B20::showAddress
 * @param addr
 */
void DS18B20::showAddress(quint64 &addr)
{
    ui->addrLabel->setText(QString::number(addr, 16).toUpper());
}

/**
 * @brief DS18B20::showValue
 * @param value
 */
void DS18B20::showValue(float value)
{
    if (value < 0.0f) {
        ui->temperValueLabel->setStyleSheet("color: blue");
    }
    else {
        ui->temperValueLabel->setStyleSheet("color: red");
    }
    ui->temperValueLabel->setText(QString::number(value, 'f', 1) + " °C");
}
