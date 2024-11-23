#ifndef DS18B20_H
#define DS18B20_H

#include "cardview.h"

namespace Ui {
class DS18B20;
}

class DS18B20 : public CardView
{
    Q_OBJECT

public:
    explicit DS18B20(CardView *parent = nullptr);
    ~DS18B20();

    void showAddress(quint64 &addr);
    void showValue(float value);

private:
    Ui::DS18B20 *ui;
};

#endif // DS18B20_H
