#include "MainWindow.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    QFont f = a.font();
    f.setFamily(u8"微软雅黑");
    a.setFont(f);
    MainWindow w;
    w.show();
    return a.exec();
}
