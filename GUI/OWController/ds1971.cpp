#include "ds1971.h"
#include "ui_ds1971.h"

DS1971::DS1971(CardView *parent)
    : CardView(parent), ui(new Ui::DS1971)
{
    ui->setupUi(this);
}

DS1971::~DS1971()
{
    delete ui;
}

/**
 * @brief DS1971::showAddress
 * @param addr
 */
void DS1971::showAddress(quint64 &addr)
{
    ui->addrLabel->setText(QString::number(addr, 16).toUpper());
}
