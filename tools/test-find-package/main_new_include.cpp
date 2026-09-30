// Consumer TU using the new namespaced include path (<SARibbonWidgets/...>)
#include <SARibbonWidgets/SARibbonBar.h>
#include <QApplication>

int main_legacy();  // defined in main.cpp (legacy include path TU)

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    return main_legacy();
}
