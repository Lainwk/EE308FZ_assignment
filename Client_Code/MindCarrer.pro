QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MindCarrer
TEMPLATE = app

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
    chatpage.cpp

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
    chatpage.h

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
