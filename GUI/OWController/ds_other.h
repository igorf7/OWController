#ifndef DS_OTHER_H
#define DS_OTHER_H

#include "cardview.h"
#include <QDialog>

namespace Ui {
class DS_OTHER;
}

class DS_OTHER : public CardView
{
    Q_OBJECT

public:
    explicit DS_OTHER(CardView *parent = nullptr);
    ~DS_OTHER();

    void showDeviceData(quint8 *data);
    void setAddress(quint64 address);
    void setIndex(int index);

private slots:
    void onSettingsButtonClicked();
    void onCloseButtonClicked();

private:
    Ui::DS_OTHER *ui;

    QDialog *settingsWindow = nullptr;

    quint64 myAddress = 0;

    int myIndex = 0;

    quint8 devFamilyCode = 0;
};

#endif // DS_OTHER_H
