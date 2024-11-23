#ifndef OWDEVICE_H
#define OWDEVICE_H

#include "usertypes.h"
#include "qglobal.h"
#include <QMap>

class OWDevice
{
public:
    OWDevice();

    const QString &getName(quint8 family);
    quint8 getFamily(const QString &name);
    int getQuantity();
    void addAddress(quint64 &addr);
    void clearAddressList();
    int getCount();
    int getCountByName(const QString &name);
    void getFound(TDeviceMap &map);

private:
    QString devName = "";
    QList<quint64> devAddressList;
    // Supported devices map
    const TDeviceMap deviceMap =
        {
            {"DS1990A", 0x01},  // 64-bit unique serial number
            {"DS1991", 0x02},   // 2048 bits of nonvolatile read/write memory
            {"DS1994", 0x04},   // 4096 bit read/write nonvolatile memory
            {"DS2405", 0x05},   // Addressable Switch with controlled open drain PIO pin
            {"DS1993", 0x06},   // 4096 bit read/write nonvolatile memory
            {"DS1992", 0x08},   // 1024 bit read/write nonvolatile memory
            {"DS1982", 0x09},   // 1024 bit Electrically Programmable Read Only Memory
            {"DS1995", 0x0A},   // 16384 bit read/write nonvolatile memory
            {"DS1985", 0x0B},   // 16384 bit Electrically Programmable Read Only Memory
            {"DS1996", 0x0C},   // 65536 bit read/write nonvolatile memory
            {"DS1986", 0x0F},   // 65536 bit Electrically Programmable Read Only Memory
            {"DS18S20", 0x10},  // Digital thermometer -55C to 125C 9 bit resolution
            {"DS2406", 0x12},   // 1-Wire Dual Addressable Switch
            {"DS1983", 0x13},   // 4096 bit Electrically Programmable Read Only Memory
            {"DS1971", 0x14},   // Electrically Erasable Programmable Read Only Memory
            {"DS1963S", 0x18},  // 4096 bits of read/write nonvolatile memory
            {"DS1963L", 0x1A},  // 4096 bit read/write nonvolatile memory
            {"DS28E04", 0x1C},  // Addressable 1-Wire 4K-bit EEPROM
            {"DS2423", 0x1D},   // 1-Wire counter with 4096 bits of read/write
            {"DS2409", 0x1F},   // 1-Wire Network Coupler with dual addressable
            {"DS2450", 0x20},   // Four high-impedance inputs for measurement of analog voltages
            {"DS1921", 0x21},   // Thermochron
            {"DS1822", 0x22},   // Digital thermometer -55C to 125C 12 bit resolution
            {"DS1973", 0x23},   // EEPROM 4096 bits
            {"DS2415", 0x24},   // Real time clock
            {"DS2438", 0x26},   // Smart Battery Monitor
            {"DS2417", 0x27},   // Real time clock with interrupt
            {"DS18B20", 0x28},  // Digital thermometer -55C to 125C 12 bit resolution
            {"DS2408", 0x29},   // 1-Wire 8-Channel Addressable Switch
            {"DS2890", 0x2C},   // 1-Wire linear taper digitally controlled potentiometer with 256 wiper positions
            {"DS1972", 0x2D},   // 1K-Bit protected 1-Wire EEPROM
            {"DS2760", 0x30},   // 1-Cell Li-Ion Battery Monitor
            {"DS1961S", 0x33},  // 1K-Bit protected 1-Wire EEPROM with SHA-1 Engine
            {"DS1977", 0x37},   // 32KB password protected memory
            {"DS2413", 0x3A},   // Dual Channel Addressable Switch
            {"DS1923", 0x41},   // Hygrochron
            {"DS28EA00", 0x42}, // Programmable resolution digital thermometer with 'sequence detect'
            {"DS28EC20", 0x43}  // 20Kb 1-Wire EEPROM
    };
};

#endif // OWDEVICE_H
