#include "cardview.h"
#include "ui_cardview.h"
#include <QPainter>

CardView::CardView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CardView)
{
    ui->setupUi(this);

    color.setRgb(0, 0, 0, 20);
}

CardView::~CardView()
{
    delete ui;
}

/**
 * @brief OWDevice::paintEvent
 */
void CardView::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(color));
    painter.setBrush(QBrush(color));
    painter.drawRoundedRect(QRect(1, 1, this->width()-3,
                                  this->height()-3), 11, 11);
}
