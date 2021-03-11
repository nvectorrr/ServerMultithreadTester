#include "mainwindow.h"

#include <QApplication>
#include <QScreen>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow main_window;

    const QRect availableSize = main_window.screen()->availableGeometry();
    main_window.resize(availableSize.width() / 3, availableSize.height() / 2);
    main_window.move((availableSize.width() - main_window.width()) / 2, (availableSize.height() - main_window.height()) / 2);

    main_window.show();
    return a.exec();
}
