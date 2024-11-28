#ifndef CARDVIEW_H
#define CARDVIEW_H

#include "usertypes.h"
#include <QWidget>

namespace Ui {
class CardView;
}

class CardView : public QWidget
{
    Q_OBJECT

public:
    explicit CardView(QWidget *parent = nullptr);
    ~CardView();

    virtual void showDeviceData(quint8 *data, int index) = 0;

signals:
    void sendCommand(TOpcode opcode, quint8 *data, quint8 data_len);

protected:
    void paintEvent(QPaintEvent*);

private:
    Ui::CardView *ui;

    QColor color;
};

#endif // CARDVIEW_H
