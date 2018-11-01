QT += core gui widgets svg

TARGET = qubistic

TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    renderprocess.cpp \
    settingsdialog.cpp

HEADERS += \
    configuration.h \
    mainwindow.h \
    renderprocess.h \
    settingsdialog.h

FORMS += \
    mainwindow.ui \
    settingsdialog.ui
