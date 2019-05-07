#include "mainwindow.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton);
    w.show();

    return a.exec();
}
