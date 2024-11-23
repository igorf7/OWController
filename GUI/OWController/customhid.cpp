#include "customhid.h"
#include <QDebug>

/**
 * @brief Class constructor
 * @param vid
 * @param pid
 */
CustomHid::CustomHid(unsigned short vid,
                     unsigned short pid) : VID(vid), PID(pid)
{
}

/**
 * @brief Class destructor
 */
CustomHid::~CustomHid()
{
}

/**
 * @brief CustomHid::Connect
 * @return true if connected, false otherwise
 */
bool CustomHid::Connect(const QString &product_string)
{
    struct hid_device_info *devs, *cur_dev;

    /* Find devices with specified VID/PID */
    devs = hid_enumerate(VID, PID);
    cur_dev = devs;

    while (cur_dev)
    {
        /* Trying to find a device with a given product string */
        if (QString::fromWCharArray(cur_dev->product_string) == product_string)
            break;
        cur_dev = cur_dev->next;
    }

    if (!cur_dev) return false;

    /* Open found device */
    deviceHandle = hid_open_path(cur_dev->path);

    if (!deviceHandle) {
        hid_exit();
        return false;
    }

    hid_set_nonblocking(deviceHandle, 1);

    /* Read the Manufacturer String */
    opResult = hid_get_manufacturer_string(deviceHandle, wstr, MAX_STR);
    /* Read the Product String */
    opResult = hid_get_product_string(deviceHandle, wstr, MAX_STR);
    /* Read the Serial Number String */
    opResult = hid_get_serial_number_string(deviceHandle, wstr, MAX_STR);
    /* Read VID/PID */
    deviceInfo = hid_get_device_info(deviceHandle);
    return true;
}

/**
 * @brief CustomHid::Disconnect
 */
void CustomHid::Disconnect()
{
    /* Close the device */
    hid_close(deviceHandle);
    /* Finalize the hidapi library */
    hid_exit();
}

/**
 * @brief CustomHid::Read
 * @param buff - pointer to rx data buffer
 * @param len - received data lenght
 * @return number of bytes received
 */
int CustomHid::Read(unsigned char *buff, size_t len)
{
    /* Read rx data */
    //return hid_get_feature_report(deviceHandle, buff, len);
    return hid_read(deviceHandle, buff, len);
}

/**
 * @brief CustomHid::Write
 * @param buff - pointer to tx data buffer
 * @param len - transmitted data lenght
 * @return number of bytes transmitted
 */
int CustomHid::Write(unsigned char *buff, size_t len, bool feat_mode)
{
    /* Send data to device */
    if (feat_mode) {
        return hid_send_feature_report(deviceHandle, buff, len);
    }
    else {
        return hid_write(deviceHandle, buff, len);
    }
}

/**
 * @brief CustomHid::setVid
 * @param vid
 */
void CustomHid::setVid(unsigned short vid)
{
    VID = vid;
}

/**
 * @brief CustomHid::setPid
 * @param pid
 */
void CustomHid::setPid(unsigned short pid)
{
    PID = pid;
}

/**
 * @brief CustomHid::getVid
 * @return device VID
 */
unsigned short CustomHid::getVid()
{
    return deviceInfo->vendor_id;
}

/**
 * @brief CustomHid::getPid
 * @return device PID
 */
unsigned short CustomHid::getPid()
{
    return deviceInfo->product_id;
}

/**
 * @brief CustomHid::getProductString
 * @return pointer to Product string
 */
wchar_t* CustomHid::getProductString()
{
    return deviceInfo->product_string;
}

/**
 * @brief CustomHid::getManufacturerString
 * @return pointer to Manufacturer string
 */
wchar_t* CustomHid::getManufacturerString()
{
    return deviceInfo->manufacturer_string;
}

/**
 * @brief CustomHid::getSerialNumber
 * @return pointer to Serian Number string
 */
wchar_t* CustomHid::getSerialNumber()
{
    return deviceInfo->serial_number;
}
