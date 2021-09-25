#include "mainwindow.h"
#include <QApplication>
#include <ctime>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    std::srand(unsigned(std::time(nullptr)));


    return a.exec();

}
