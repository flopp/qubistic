#include <QtGui/QGuiApplication>
#include "application.h"


Application::Application() :
    QObject{}
{
    connect(qApp, &QCoreApplication::aboutToQuit, this, &Application::quitting);
}

QString Application::name() const {
    return qApp->applicationName();
}

void Application::quitting() {
    settings_.sync();
}

Application& app() {
    return Application::getInstance();
}
