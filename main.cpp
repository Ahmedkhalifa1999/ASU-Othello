#include "MainWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    Board bo;
    BoardWidget b(bo);
    b.show();
    w.show();
    return a.exec();
}
