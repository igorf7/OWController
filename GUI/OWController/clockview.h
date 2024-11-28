#ifndef CLOCKVIEW_H
#define CLOCKVIEW_H

#include "cardview.h"
#include <QDateTime>

namespace Ui {
class ClockView;
}

class ClockView : public CardView
{
    Q_OBJECT

public:
    explicit ClockView(CardView *parent = nullptr);
    ~ClockView();

    void showDeviceData(quint8 *data, int index);
    void showDeviceData(quint32 timestamp);

private:
    Ui::ClockView *ui;

    QDateTime dateTime;
    QDate dateNow;
    QTime timeNow;
};

#endif // CLOCKVIEW_H
