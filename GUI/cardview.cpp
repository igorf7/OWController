#include "cardview.h"
#include "ui_cardview.h"
#include <QPainter>

CardView::CardView(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CardView)
{
    ui->setupUi(this);

    penColor.setRgb(0, 128, 128);
    //brushColor.setRgb(0, 0, 20);
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
    painter.setPen(QPen(penColor));
    //painter.setBrush(QBrush(brushColor));
    painter.drawRoundedRect(QRect(1, 1, this->width()-2,
                            this->height()-2), 14, 14);
}
