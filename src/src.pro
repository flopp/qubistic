QT += core gui svg widgets

TARGET = qubistic

TEMPLATE = app

SOURCES += \
    imagewidget.cpp \
    main.cpp \
    mainwindow.cpp \
    renderprocess.cpp \
    settingsdialog.cpp

HEADERS += \
    configuration.h \
    imagewidget.h \
    mainwindow.h \
    renderprocess.h \
    settingsdialog.h

FORMS += \
    mainwindow.ui \
    settingsdialog.ui
