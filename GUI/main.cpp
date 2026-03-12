#include "mainwindow.h"

#include <QApplication>

#ifdef Q_OS_ANDROID
#include <QJniObject>
#include <QCoreApplication>
#include <QtCore/private/qandroidextras_p.h>
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

#ifdef Q_OS_ANDROID
    // Запрет выключения экрана
    QNativeInterface::QAndroidApplication::runOnAndroidMainThread([=]() {
        QJniObject activity = QNativeInterface::QAndroidApplication::context();
        if (activity.isValid()) {
            QJniObject window = activity.callObjectMethod("getWindow", "()Landroid/view/Window;");
            if (window.isValid()) {
                window.callMethod<void>("addFlags", "(I)V", 0x00000080); // FLAG_KEEP_SCREEN_ON
            }
        }
    });
#endif

    w.show();
    return a.exec();
}
