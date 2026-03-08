QT       += core gui core-private

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cardview.cpp \
    clockview.cpp \
    devicewidget.cpp \
    ds18b20.cpp \
    ds1971.cpp \
    ds_other.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    cardview.h \
    clockview.h \
    devicewidget.h \
    ds18b20.h \
    ds1971.h \
    ds_other.h \
    mainwindow.h \
    owdevice.h \
    usertypes.h

FORMS += \
    cardview.ui \
    clockview.ui \
    devicewidget.ui \
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
    INCLUDEPATH += $$PWD/hid/inc
    DEPENDPATH += $$PWD/hid/inc
}

linux:!android {
    SOURCES += customhid.cpp \
               hid/src/linux/hid.c
    HEADERS += customhid.h \
               hid/inc/hidapi.h
    DESTDIR = ../../Binary/linux
}

macx {
    SOURCES += customhid.cpp \
               hid/src/mac/hid.c
    HEADERS += customhid.h \
               hid/inc/hidapi.h
    DESTDIR = ../../Binary/mac
#    ICON = $${PWD}/images/press_mac.icns
}

win32 {
    SOURCES += customhid.cpp \
               hid/src/windows/hid.c
    HEADERS += customhid.h \
               hid/inc/hidapi.h
    DESTDIR = ../../Binary/windows
#    RC_ICONS += $$PWD/images/pressure.ico
}

RESOURCES += res.qrc
