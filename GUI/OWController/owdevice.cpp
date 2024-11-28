#include "owdevice.h"

/**
 * @brief Class constructor
 */
OWDevice::OWDevice()
{
}

/**
 * @brief addAddress
 * @param addr
 */
void OWDevice::addAddress(quint64 &addr)
{
    devAddressList.append(addr);
}

/**
 * @brief clearAddressList
 */
void OWDevice::clearAddressList()
{
    devAddressList.clear();
}

/**
 * @brief OWDevice::getCount
 * @return
 */
int OWDevice::getCount()
{
    return devAddressList.size();
}

/**
 * @brief OWDevice::getCountByName
 * @param name
 * @return
 */
int OWDevice::getCountByName(const QString &name)
{
    int dev_count = 0;
    quint8 family = getFamily(name);

    for (auto i = 0; i < devAddressList.size(); ++i) {
        if ((devAddressList.at(i) & 0xFF) == family) {
            dev_count++;
        }
    }
    return dev_count;
}

/**
 * @brief OWDevice::getFound
 * @param map
 */
void OWDevice::getFound(TDeviceMap &map)
{
    int cnt = 0;

    QMapIterator<QString, quint8> it(deviceMap);

    while (it.hasNext()) {
        it.next();
        cnt = getCountByName(it.key());
        if (cnt > 0) {
            map.insert(it.key(), cnt);
        }
    }
}
