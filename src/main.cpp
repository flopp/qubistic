#include <QtWidgets/QApplication>
#include "mainwindow.h"

int main(int argc, char** argv)
{
    QApplication app{argc, argv};

    QCoreApplication::setOrganizationName("flopp");
    QCoreApplication::setOrganizationDomain("flopp.net");
    QCoreApplication::setApplicationName("Qubistic");

    MainWindow w;
    w.show();

    return app.exec();
}
