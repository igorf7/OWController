#include "devicewidget.h"
#include "ui_devicewidget.h"

DeviceWidget::DeviceWidget(CardView *parent)
    : CardView(parent)
    , ui(new Ui::DeviceWidget)
{
    ui->setupUi(this);
}

DeviceWidget::~DeviceWidget()
{
    delete ui;
}
