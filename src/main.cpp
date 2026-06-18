/**
 * 应用程序入口。
 * 创建 QApplication（Fusion 风格），显示主窗口，进入事件循环。
 */
#include <QApplication>
#include "mainwindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setStyle("Fusion");          // 跨 Windows 版本统一风格
    app.setApplicationName("PixelShot");
    app.setOrganizationName("PixelShot");

    MainWindow window;
    window.show();

    return app.exec();
}
