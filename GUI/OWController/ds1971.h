#ifndef DS1971_H
#define DS1971_H

#include "cardview.h"

namespace Ui {
class DS1971;
}

class DS1971 : public CardView
{
    Q_OBJECT

public:
    explicit DS1971(CardView *parent = nullptr);
    ~DS1971();

    void showAddress(quint64 &addr);

private:
    Ui::DS1971 *ui;
};

#endif // DS1971_H
