#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QFont>
#include "SARibbonBar.h"

int main(int argc, char* argv[])
{
    SARibbonBar::initHighDpi();

    QApplication a(argc, argv);
#ifdef SA_RIBBON_BAR_NO_EXPORT
    Q_INIT_RESOURCE(SARibbonResource);
#endif
    QFont f = a.font();
    f.setFamily("Microsoft YaHei");
    a.setFont(f);
    QElapsedTimer cost;
    cost.start();
    MainWindow w;
    qDebug() << "window build cost:" << cost.elapsed() << "ms";
    w.show();

    return a.exec();
}
