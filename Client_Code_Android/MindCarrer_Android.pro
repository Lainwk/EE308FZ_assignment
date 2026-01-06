QT       += core gui network sql concurrent svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MindCarrer
TEMPLATE = app

# Ensure consistent library naming for Android
android {
    TARGET = MindCarrer
}

# Android specific configuration
android {
    # Qt 6 doesn't have androidextras, functionality is in QtCore
    
    # Android package information
    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
    
    # Android permissions
    ANDROID_PERMISSIONS += \
        android.permission.INTERNET \
        android.permission.ACCESS_NETWORK_STATE \
        android.permission.WRITE_EXTERNAL_STORAGE \
        android.permission.READ_EXTERNAL_STORAGE \
        android.permission.WAKE_LOCK \
        android.permission.POST_NOTIFICATIONS
    
    # Android features
    ANDROID_FEATURES += \
        android.hardware.touchscreen
    
    # Minimum Android version: Android 9.0 (API 28) - Required by Qt 6.5.3
    ANDROID_MIN_SDK_VERSION = 28

    ANDROID_TARGET_SDK_VERSION = 33
    
    # Force use API 33 instead of API 36
    ANDROID_API_VERSION = android-33
    ANDROID_BUILD_TOOLS_REVISION = 33.0.3
    
    # Android app icon
    ANDROID_ICON = $$PWD/android/res/drawable/icon.png
    
    # Qt 库部署 - 确保所有 Qt 库被打包到 APK
    ANDROID_EXTRA_LIBS = \
        $$[QT_INSTALL_LIBS]/libQt6Core_$${QT_ARCH}.so \
        $$[QT_INSTALL_LIBS]/libQt6Gui_$${QT_ARCH}.so \
        $$[QT_INSTALL_LIBS]/libQt6Widgets_$${QT_ARCH}.so \
        $$[QT_INSTALL_LIBS]/libQt6Network_$${QT_ARCH}.so \
        $$[QT_INSTALL_LIBS]/libQt6Sql_$${QT_ARCH}.so \
        $$[QT_INSTALL_LIBS]/libQt6Concurrent_$${QT_ARCH}.so \
        $$[QT_INSTALL_LIBS]/libQt6Svg_$${QT_ARCH}.so
    
    # Note: PDF files are already copied to android/assets/PDF/
    # Qt will automatically include all files in ANDROID_PACKAGE_SOURCE_DIR/assets
}

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    homepage.cpp \
    contactspage.cpp \
    channelspage.cpp \
    settingspage.cpp \
    authwindow.cpp \
    frontclient.cpp \
    localstore.cpp \
    dailyassessmentpage.cpp \
    assessmentreportpage.cpp \
    ebookreaderwidget.cpp \
    trendchartwidget.cpp \
    assessmentcache.cpp \
    changepasswordpage.cpp \
    chatpage.cpp \
    reminderservice.cpp \
    psychotipsmanager.cpp

HEADERS += \
    mainwindow.h \
    homepage.h \
    contactspage.h \
    channelspage.h \
    settingspage.h \
    authwindow.h \
    frontclient.h \
    localstore.h \
    dailyassessmentpage.h \
    assessmentreportpage.h \
    ebookreaderwidget.h \
    trendchartwidget.h \
    assessmentcache.h \
    changepasswordpage.h \
    chatpage.h \
    reminderservice.h \
    psychotipsmanager.h

FORMS += \
    mainwindow.ui \
    homepage.ui \
    contactspage.ui \
    channelspage.ui \
    settingspage.ui \
    authwindow.ui \
    dailyassessmentpage.ui \
    assessmentreportpage.ui \
    ebookreaderwidget.ui \
    changepasswordpage.ui \
    chatpage.ui

RESOURCES += \
    resources.qrc

# Android specific: Remove Windows RC file
# win32:RC_FILE = MindCarrer.rc

# Deployment settings
DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/res/values/libs.xml

# Include paths
INCLUDEPATH += $$PWD

# Defines
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += ANDROID_BUILD

# C++ standard
CONFIG += c++11

# Optimize for mobile
android {
    # Enable optimization
    QMAKE_CXXFLAGS_RELEASE += -O2
    
    # Reduce binary size
    QMAKE_LFLAGS_RELEASE += -s
}
