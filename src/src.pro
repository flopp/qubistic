QT += core gui svg widgets

TARGET = qubistic

TEMPLATE = app

SOURCES += \
    imagewidget.cpp \
    main.cpp \
    mainwindow.cpp \
    renderprocess.cpp \
    settings.cpp \
    settingsdialog.cpp

HEADERS += \
    imagewidget.h \
    mainwindow.h \
    renderprocess.h \
    settings.h \
    settingsdialog.h

FORMS += \
    mainwindow.ui \
    settingsdialog.ui
