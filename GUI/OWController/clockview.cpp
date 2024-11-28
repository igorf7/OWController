#include "clockview.h"
#include "ui_clockview.h"
#include <QDebug>

ClockView::ClockView(CardView *parent)
    : CardView(parent) , ui(new Ui::ClockView)
{
    ui->setupUi(this);

    ui->timeLcd->display("00:00:00");
}

ClockView::~ClockView()
{
    delete ui;
}

/**
 * @brief ClockView::showDateTime
 * @param data
 * @param index
 */
void ClockView::showDeviceData(quint8 *data, int index)
{
    Q_UNUSED(data)
    Q_UNUSED(index)
}

/**
 * @brief ClockView::setAddress
 * @param address
 */
void ClockView::setAddress(quint64 address)
{
    Q_UNUSED(address)
}

/**
 * @brief ClockView::setIndex
 * @param index
 */
void ClockView::setIndex(int index)
{
    Q_UNUSED(index)
}

/**
 * @brief ClockView::showDateTime
 * @param timestamp
 */
void ClockView::showDeviceData(quint32 timestamp)
{
    dateTime = QDateTime::fromSecsSinceEpoch(timestamp);
    dateNow = dateTime.date();
    timeNow = dateTime.time();
    ui->timeLcd->display(timeNow.toString("hh:mm:ss"));
    ui->dateLabel->setText(dateNow.toString(Qt::TextDate));
}
