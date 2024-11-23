#ifndef DS_OTHER_H
#define DS_OTHER_H

#include "cardview.h"

namespace Ui {
class DS_OTHER;
}

class DS_OTHER : public CardView
{
    Q_OBJECT

public:
    explicit DS_OTHER(CardView *parent = nullptr);
    ~DS_OTHER();

    void showAddress(quint64 &addr);

private:
    Ui::DS_OTHER *ui;
};

#endif // DS_OTHER_H
