QT       += core gui widgets charts network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GlobalEnergyRiskMonitor
TEMPLATE = app

CONFIG += c++17

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    riskengine.cpp \
    datamanager.cpp \
    aiassistant.cpp \
    stylemanager.cpp \
    configmanager.cpp \
    reportgenerator.cpp \
    splashscreen.cpp

HEADERS += \
    mainwindow.h \
    datamodels.h \
    riskengine.h \
    datamanager.h \
    aiassistant.h \
    stylemanager.h \
    configmanager.h \
    reportgenerator.h \
    splashscreen.h

# Default rules for deployment
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
