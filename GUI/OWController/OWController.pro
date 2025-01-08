QT       += core gui core-private

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cardview.cpp \
    clockview.cpp \
    customhid.cpp \
    devicewidget.cpp \
    ds18b20.cpp \
    ds1971.cpp \
    ds_other.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    cardview.h \
    clockview.h \
    customhid.h \
    devicewidget.h \
    ds18b20.h \
    ds1971.h \
    ds_other.h \
    hidapi/hidapi.h \
    hidapi/hidapi_libusb.h \
    hidapi/libusb.h \
    mainwindow.h \
    owdevice.h \
    usertypes.h

FORMS += \
    cardview.ui \
    clockview.ui \
    ds18b20.ui \
    ds1971.ui \
    ds_other.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

android {
    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
    OTHER_FILES += \
                android/src/org/qtproject/example/OWController/CustomHid.java \
                android/AndroidManifest.xml

    arm64-v8a {
        LIBS += -L$$PWD/android/libs/arm64-v8a/ -lhidapi -lusb1.0
    }
    armeabi-v7a {
        LIBS += -L$$PWD/android/libs/armeabi-v7a/ -lhidapi -lusb1.0
    }
    x86 {
        LIBS += -L$$PWD/android/libs/x86/ -lhidapi -lusb1.0
    }
    x86_64 {
        LIBS += -L$$PWD/android/libs/x86_64/ -lhidapi -lusb1.0
    }
    INCLUDEPATH += $$PWD/android/libs
    DEPENDPATH += $$PWD/android/libs
}

win32 {
    LIBS += -L$$PWD/windows/libs/ -lhidapi
    INCLUDEPATH += $$PWD/hidapi
    DEPENDPATH += $$PWD/hidapi
}

unix:!macx {
    unix:!android {
        LIBS += -L$$PWD/linux/libs/ -lhidapi-hidraw
        INCLUDEPATH += $$PWD/hidapi
        DEPENDPATH += $$PWD/hidapi
    }
}

RESOURCES += res.qrc
