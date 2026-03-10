#include "jnilayer.h"
#include <QJniObject>
#include <QCoreApplication>
#include <QtCore/private/qandroidextras_p.h>
using namespace Qt::StringLiterals;

/**
 * @brief Class constructor
 * @param vid
 * @param pid
 */
JniLayer::JniLayer(unsigned short vid,
                     unsigned short pid) : VID(vid), PID(pid)
{
    this->initUsbBridge();
}

/**
 * @brief Class destructor
 */
JniLayer::~JniLayer()
{
    this->closeHidDevice();
}

/**
 * @brief JniLayer::onConnect
 */
bool JniLayer::Connect()
{
    return this->openHidDevice(VID, PID);
}

/**
 * @brief JniLayer::onDisconnect
 */
void JniLayer::Disconnect()
{
    this->closeHidDevice();
}

/**
 * @brief JniLayer::initUsbBridge
 */
void JniLayer::initUsbBridge()
{
    QJniObject activity = QtAndroidPrivate::context();
    QJniObject::callStaticMethod<void>(
        "org/qtproject/example/UsbBridge",
        "init",
        "(Landroid/app/Activity;)V",
        activity.object()
        );
}

/**
 * @brief JniLayer::openHidDevice
 * @param vid
 * @param pid
 * @return
 */
bool JniLayer::openHidDevice(int vid, int pid)
{
    return QJniObject::callStaticMethod<jboolean>(
        "org/qtproject/example/UsbBridge",
        "openDevice",
        "(II)Z",
        vid, pid
        );
}

/**
 * @brief JniLayer::readFromDevice
 */
void JniLayer::readFromDevice(QByteArray &array)
{
    QJniObject data = QJniObject::callStaticObjectMethod(
        "org/qtproject/example/UsbBridge",
        "read",
        "(I)[B",
        RX_DATA_SIZE
        );

    if (!data.isValid()) {
        return;
    }

    jbyteArray byteArray = data.object<jbyteArray>();
    QJniEnvironment env;
    jsize size = env->GetArrayLength(byteArray);
    jbyte* bytes = env->GetByteArrayElements(byteArray, nullptr);
    array.append(reinterpret_cast<const char*>(bytes), size);
    env->ReleaseByteArrayElements(byteArray, bytes, 0);
}

/**
 * @brief JniLayer::writeToDevice
 * @param data
 * @return
 */
bool JniLayer::writeToDevice(const QByteArray &data)
{
    QJniEnvironment env;

    // Создаём Java byte array из QByteArray
    jbyteArray jData = env->NewByteArray(data.size());
    env->SetByteArrayRegion(jData, 0, data.size(), reinterpret_cast<const jbyte*>(data.constData()));

    // Вызываем Java-метод
    jboolean result = QJniObject::callStaticMethod<jboolean>(
        "org/qtproject/example/UsbBridge",
        "write",
        "([B)Z",
        jData
        );

    // Освобождаем ресурсы
    env->DeleteLocalRef(jData);
    return (bool)result;
}

/**
 * @brief JniLayer::closeHidDevice
 */
void JniLayer::closeHidDevice()
{
    QJniObject::callStaticMethod<jboolean>(
        "org/qtproject/example/UsbBridge",
        "closeConnection"
        );
}
