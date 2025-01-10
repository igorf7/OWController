#include "customhid.h"

#ifdef __ANDROID__
#include <QtCore/qjniobject.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/private/qandroidextras_p.h>

using namespace Qt::StringLiterals;
#endif

/**
 * @brief Class constructor
 * @param vid
 * @param pid
 */
CustomHid::CustomHid(unsigned short vid,
                     unsigned short pid) : VID(vid), PID(pid)
{
    hid_init();

#ifdef __ANDROID__
    /* Find USB device via JNI */
    this->findUsbDevice();
#endif
}

/**
 * @brief Class destructor
 */
CustomHid::~CustomHid()
{
}

/**
 * @brief CustomHid::onConnect
 * @param product_string
 */
void CustomHid::Connect(const QString &product_string)
{
#ifdef __ANDROID__
    Q_UNUSED(product_string)

    /* Open USB device via JNI */
    int file_descriptor = this->openUsbDevice();

    if (file_descriptor > 0) {
        deviceHandle = hid_libusb_wrap_sys_device((intptr_t)file_descriptor, 0);
    }
    else {
        return;
    }
#else

    struct hid_device_info *devs, *cur_dev;

    /* Find devices with specified VID/PID */
    devs = hid_enumerate(VID, PID);
    cur_dev = devs;

    while (cur_dev)
    {
        /* Find USB device with a given product string */
        if (QString::fromWCharArray(cur_dev->product_string) == product_string)
        {
            break;
        }
        cur_dev = cur_dev->next;
    }

    if (!cur_dev) return;

    /* Open found device */
    deviceHandle = hid_open_path(cur_dev->path);

    if (!deviceHandle) {
        hid_exit();
        return;
    }
#endif

    hid_set_nonblocking(deviceHandle, 1);
    emit deviceConnected();
}

/**
 * @brief CustomHid::onDisconnect
 */
void CustomHid::Disconnect()
{
    /* Close the device */
    if (deviceHandle != nullptr) {
        hid_close(deviceHandle);
        deviceHandle = nullptr;
    }

    /* Finalize the hidapi library */
    hid_exit();
    emit deviceDisconnected();
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
    return hid_read(deviceHandle, buff, len);
}

/**
 * @brief CustomHid::getFeatureReport
 * @param buff - pointer to rx data buffer
 * @param len - received data lenght
 * @return  number of bytes received
 */
int CustomHid::getFeatureReport(unsigned char *buff, size_t len)
{
    return hid_get_feature_report(deviceHandle, buff, len);
}

/**
 * @brief CustomHid::Write
 * @param buff - pointer to tx data buffer
 * @param len - transmitted data lenght
 * @return number of bytes transmitted
 */
int CustomHid::Write(unsigned char *buff, size_t len)
{
    /* Send data to device */
    return hid_write(deviceHandle, buff, len);
}

/**
 * @brief CustomHid::sendFeatureReport
 * @param buff - pointer to tx data buffer
 * @param len - transmitted data lenght
 * @return  number of bytes transmitted
 */
int CustomHid::sendFeatureReport(unsigned char *buff, size_t len)
{
    return hid_send_feature_report(deviceHandle, buff, len);
}

/**
 * @brief CustomHid::findUsbDevice
 */
void CustomHid::findUsbDevice()
{
#ifdef __ANDROID__

    jint vid = (jint)VID;
    jint pid = (jint)PID;
    QJniObject::callStaticMethod<void>(
        "org/qtproject/example/OWController/CustomHid",
        "findUsbDevice",
        QNativeInterface::QAndroidApplication::context(),
        vid, pid);

#endif
}

/**
 * @brief CustomHid::openUsbDevice
 * @return
 */
int CustomHid::openUsbDevice()
{
#ifdef __ANDROID__

    jint file_descriptor = QJniObject::callStaticMethod<int>(
        "org/qtproject/example/OWController/CustomHid",
        "openUsbDevice",
        QNativeInterface::QAndroidApplication::context());

    return (int)file_descriptor;
#else
    return 0;
#endif
}
