#include <QApplication>
#include "mainwindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setStyle("Fusion");
    app.setApplicationName("ZCapture");
    app.setOrganizationName("ZCapture");

    MainWindow window;
    window.show();

    return app.exec();
}
