#include "UiLoad.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QWidget *w = load();

    if (w) {
        w->show();
    }else{
        //用户放弃选择就退出
        exit(0);
    }
    return (a.exec());
}
