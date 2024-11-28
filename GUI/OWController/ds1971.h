#ifndef DS1971_H
#define DS1971_H

#include "cardview.h"
#include <QDialog>
#include <QPlainTextEdit>

namespace Ui {
class DS1971;
}

class DS1971 : public CardView
{
    Q_OBJECT

public:
    explicit DS1971(CardView *parent = nullptr);
    ~DS1971();

    void showDeviceData(quint8 *data, int index);

private slots:
    void onSettingsButtonClicked();
    void onCloseButtonClicked();
    void onReadButtonClicked();
    void onWriteButtonClicked();

private:
    Ui::DS1971 *ui;

    const quint8 devFamilyCode = 0x14;
    static const quint8 DS1971_MEMORY_SIZE = 32;
    static const quint8 MEM_EDITOR_LINE_LEN = 16;

    QDialog *settingsWindow = nullptr;
    QPlainTextEdit *memEdit = nullptr;

    quint64 deviceAddress = 0;

    quint8 ds1971memory[DS1971_MEMORY_SIZE];

    void showDeviceMemory();
};

#endif // DS1971_H
