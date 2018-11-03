QT += core gui svg widgets
CONFIG += debug

TARGET = qubistic

TEMPLATE = app

SOURCES += \
    application.cpp \
    imagewidget.cpp \
    main.cpp \
    mainwindow.cpp \
    renderprocess.cpp \
    settings.cpp \
    settingsdialog.cpp

HEADERS += \
    application.h \
    imagewidget.h \
    mainwindow.h \
    renderprocess.h \
    settings.h \
    settingsdialog.h

FORMS += \
    mainwindow.ui \
    settingsdialog.ui

RESOURCES += \
    resources.qrc
