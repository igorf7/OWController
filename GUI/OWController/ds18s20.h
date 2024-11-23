#ifndef DS18S20_H
#define DS18S20_H

#include "cardview.h"

namespace Ui {
class DS18S20;
}

class DS18S20 : public CardView
{
    Q_OBJECT

public:
    explicit DS18S20(CardView *parent = nullptr);
    ~DS18S20();

    void showAddress(quint64 &addr);
    void showValue(float value);

private:
    Ui::DS18S20 *ui;
};

#endif // DS18S20_H
