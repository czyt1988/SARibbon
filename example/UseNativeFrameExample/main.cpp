#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <QElapsedTimer>
#include "SARibbonBar.h"

int main(int argc, char* argv[])
{
    // 以下是针对高分屏的设置，有高分屏需求都需要按照下面进行设置
    SARibbonBar::initHighDpi();

    QApplication a(argc, argv);

    QFont f = a.font();
    f.setFamily(u8"微软雅黑");
    a.setFont(f);
    MainWindow w;

    w.show();
    return (a.exec());
}
