#include "ds_other.h"
#include "ui_ds_other.h"

DS_OTHER::DS_OTHER(CardView *parent)
    : CardView(parent), ui(new Ui::DS_OTHER)
{
    ui->setupUi(this);
}

DS_OTHER::~DS_OTHER()
{
    delete ui;
}

/**
 * @brief DS_OTHER::showAddress
 * @param addr
 */
void DS_OTHER::showAddress(quint64 &addr)
{
    ui->addrLabel->setText(QString::number(addr, 16).toUpper());
}
