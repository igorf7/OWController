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
 * @param timestamp
 */
void ClockView::showDateTime(quint32 timestamp)
{
    dateTime = QDateTime::fromSecsSinceEpoch(timestamp);
    dateNow = dateTime.date();
    timeNow = dateTime.time();
    ui->timeLcd->display(timeNow.toString("hh:mm:ss"));
    ui->dateLabel->setText(dateNow.toString(Qt::TextDate));
}
