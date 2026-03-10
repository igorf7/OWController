#include "customhid.h"
#include <QTimerEvent>

/**
 * @brief Class constructor
 * @param vid
 * @param pid
 */
CustomHid::CustomHid(unsigned short vid,
                     unsigned short pid) : VID(vid), PID(pid)
{
    hid_init();
}

/**
 * @brief Class destructor
 */
CustomHid::~CustomHid()
{
    this->closeHidDevice();
}

/**
 * @brief CustomHid::onConnect
 * @param product_string
 */
void CustomHid::Connect()
{
    struct hid_device_info *devs, *cur_dev;

    /* Find devices with specified VID/PID */
    devs = hid_enumerate(VID, PID);
    cur_dev = devs;

    while (cur_dev)
    {
        /* Find USB device with a given product string */
        if (QString::fromWCharArray(cur_dev->product_string) == ProductString)
        {
            break;
        }
        cur_dev = cur_dev->next;
    }

    if (!cur_dev)
        return false;

    /* Open found device */
    deviceHandle = hid_open_path(cur_dev->path);


    if (!deviceHandle) {
        hid_exit();
        return false;
    }

    hid_set_nonblocking(deviceHandle, 1);
    emit deviceConnected();
}

/**
 * @brief CustomHid::onDisconnect
 */
void CustomHid::Disconnect()
{
    this->closeHidDevice();
    emit deviceDisconnected();
}

/**
 * @brief CustomHid::Read
 * @param buff - pointer to rx data rx_data
 * @param len - received data lenght
 * @return number of bytes received
 */
void CustomHid::readFromDevice(QByteArray &rx_data)
{
    unsigned char data[BUFF_SIZE];

    /* Read rx data */
    int len = hid_read(deviceHandle, data, BUFF_SIZE);
    if (len > 0) {
        rx_data.append((char*)data, len);
    }
}

/**
 * @brief CustomHid::writeToDevice
 * @param buff - pointer to tx data buffer
 * @return number of bytes transmitted
 */
bool CustomHid::writeToDevice(const QByteArray &tx_data)
{
    unsigned char data[BUFF_SIZE];

    if (tx_data.size() > BUFF_SIZE) return false;

    memcpy(data, tx_data.data(), tx_data.size());

    /* Send data to device */
    int len = hid_write(deviceHandle, data, BUFF_SIZE);

    return len > 0 ? true : false;
}

/**
 * @brief CustomHid::closeHidDevice
 */
void CustomHid::closeHidDevice()
{
    /* Close the device */
    if (deviceHandle != nullptr) {
        hid_close(deviceHandle);
        deviceHandle = nullptr;
    }

    /* Finalize the hidapi library */
    hid_exit();
}
