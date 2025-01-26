#ifndef DS1971_H
#define DS1971_H

#include "devicewidget.h"
#include <QDialog>
#include <QPlainTextEdit>

class DS1971 : public DeviceWidget
{
    Q_OBJECT

public:
    explicit DS1971(DeviceWidget *parent = nullptr);
    ~DS1971();

    void showDeviceData(quint8 *data, int index) override;

private slots:
    void onSettingsButtonClicked() override;
    void onCopyButtonnClicked();
    void onCloseButtonClicked();
    void onWriteButtonClicked();
    void onReadButtonClicked();

private:
    const quint8 devFamilyCode = 0x14;
    static const quint8 DS1971_MEMORY_SIZE = 32;
    static const quint8 MEM_EDITOR_LINE_LEN = 16;

    QDialog *settingsWindow = nullptr;
    QPlainTextEdit *memEdit = nullptr;
    quint8 ds1971memory[DS1971_MEMORY_SIZE];

    void showDeviceMemory();
};

#endif // DS1971_H
