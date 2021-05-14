#include "UiLoad.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QWidget *w = load();

    if (w) {
        w->show();
    }
    return (a.exec());
}
