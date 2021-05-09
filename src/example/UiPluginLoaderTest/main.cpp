#include "UiLoad.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    UiLoad w;
    w.show();

    return a.exec();
}
