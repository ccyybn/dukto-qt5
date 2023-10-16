QT += core gui network qml quickwidgets
linux:!android:QT += dbus
CONFIG += c++11

#============Compiling Options=============

# Allow only one instance
DEFINES += SINGLE_APP

# FIXME: Updater no longer works
#DEFINES += UPDATER

# Use libnotify for notifications (Linux only)
#DEFINES += NOTIFY_LIBNOTIFY

#==========================================

android: {
    DEFINES -= NOTIFY_LIBNOTIFY
    DEFINES -= SINGLE_APP
    DEFINES += MOBILE_APP
}
!linux: {
    DEFINES -= NOTIFY_LIBNOTIFY
}

TARGET = dukto
TEMPLATE = app

DEFINES += UNICODE

CONFIG(release, debug|release):DEFINES += QT_NO_DEBUG_OUTPUT

VERSION=$$fromfile(version.h, VERSION)

unix:!android {
    target.path = /usr/bin

    icon.path = /usr/share/pixmaps
    icon.files = dukto.png
    INSTALLS += icon

    desktop.path = /usr/share/applications/
    desktop.files = dukto.desktop
    INSTALLS += desktop
}

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    guibehind.cpp \
    miniwebserver.cpp \
    network/buddymessage.cpp \
    network/filedata.cpp \
    network/messenger.cpp \
    network/receiver.cpp \
    network/sender.cpp \
    platform.cpp \
    buddylistitemmodel.cpp \
    duktoprotocol.cpp \
    ipaddressitemmodel.cpp \
    recentlistitemmodel.cpp \
    settings.cpp \
    destinationbuddy.cpp \
    duktowindow.cpp \
    theme.cpp \
    systemtray.cpp

HEADERS += \
    guibehind.h \
    miniwebserver.h \
    network/buddymessage.h \
    network/filedata.h \
    network/messenger.h \
    network/receiver.h \
    network/sender.h \
    platform.h \
    buddylistitemmodel.h \
    duktoprotocol.h \
    peer.h \
    ipaddressitemmodel.h \
    recentlistitemmodel.h \
    settings.h \
    destinationbuddy.h \
    duktowindow.h \
    theme.h \
    systemtray.h \
    version.h

RESOURCES += \
    qml.qrc \
    qml/common/common.qrc

greaterThan(QT_MAJOR_VERSION, 5) {
    RESOURCES += qml/new/main.qrc
} else:lessThan(QT_MINOR_VERSION, 14) {
    RESOURCES += qml/old/main.qrc
} else {
    RESOURCES += qml/new/main.qrc
}

contains(DEFINES, UPDATER) {
    SOURCES += updateschecker.cpp
    HEADERS += updateschecker.h
}

contains(DEFINES, SINGLE_APP) {
    include(modules/SingleApplication/singleapplication.pri)
    DEFINES += QAPPLICATION_CLASS=QApplication
}

contains(DEFINES, NOTIFY_LIBNOTIFY) {
    CONFIG+=link_pkgconfig
    PKGCONFIG+=libnotify
}

OTHER_FILES += CMakeLists.txt dukto.rc

win32 {
    RC_FILE = dukto.rc
    msvc:LIBS += ws2_32.lib ole32.lib user32.lib
    gcc:LIBS += -lws2_32 -lole32 -luser32
    HEADERS += ecwin7.h
    SOURCES += ecwin7.cpp
}

mac:ICON = dukto.icns

android {
    lessThan(QT_MAJOR_VERSION, 6): QT += androidextras
    TARGET = Dukto

    ANDROID_MIN_SDK_VERSION = 21

    ANDROID_VERSION_NAME = $$VERSION
    defineReplace(androidVersionCode) {
        segments = $$split(1, ".")
        for (segment, $$list($$member(segments, 1, -1))) {
            verCode = "$${verCode}$$format_number($$segment, width=2 zeropad)"
        }
        abiCode = 0
        arch = $$ANDROID_ABIS
        isEmpty(arch): arch = $$ANDROID_TARGET_ARCH
        for (abi, $$list($$split(arch, " "))) {
            equals(abi, armeabi-v7a): abiCode = $$num_add($$abiCode, 1)
            else: equals(abi, arm64-v8a): abiCode = $$num_add($$abiCode, 2)
            else: equals(abi, x86_64): abiCode = $$num_add($$abiCode, 4)
            else: equals(abi, x86): abiCode = $$num_add($$abiCode, 8)
            else: error("Unknown ABI: $$abi")
        }
        return ($$first(segments)$${verCode}$${ANDROID_MIN_SDK_VERSION}$$format_number($$abiCode, width=2 zeropad))
    }
    ANDROID_VERSION_CODE = $$androidVersionCode($$VERSION)

    greaterThan(QT_MAJOR_VERSION, 5) {
        ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android/qt6
        OTHER_FILES += $$PWD/android/qt6/AndroidManifest.xml
    } else {
        ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android/qt5
        OTHER_FILES += $$PWD/android/qt5/AndroidManifest.xml
    }
    SOURCES += androidutils.cpp
    HEADERS += androidutils.h
}
