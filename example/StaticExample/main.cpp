#include "MainWindow.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
#ifdef SA_RIBBON_BAR_NO_EXPORT
    Q_INIT_RESOURCE(SARibbonResource);  // 针对静态库的资源加载
#endif
    QFont f = a.font();
    f.setFamily(u8"微软雅黑");
    a.setFont(f);
    MainWindow w;
    w.show();
    return a.exec();
}
