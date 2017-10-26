#include "MainWindow.h"
#include <QApplication>
#include <QElapsedTimer>
#include <QDebug>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QElapsedTimer cost;
    cost.start();
    MainWindow w;
    qDebug() <<"window build cost:"<<cost.elapsed()<<" ms";
    w.show();

    return a.exec();
}
