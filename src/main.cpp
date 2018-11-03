#include <QtWidgets/QApplication>
#include "application.h"
#include "mainwindow.h"

int main(int argc, char** argv)
{
    QApplication application{argc, argv};
    QCoreApplication::setOrganizationName("flopp");
    QCoreApplication::setOrganizationDomain("flopp.net");
    QCoreApplication::setApplicationName("Qubistic");

    app();

    MainWindow w;
    w.show();

    return application.exec();
}
