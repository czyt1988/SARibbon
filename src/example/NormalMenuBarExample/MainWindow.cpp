#include "MainWindow.h"
#include "ui_MainWindow.h"
#if SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
#include "FramelessHelper/Widgets/standardsystembutton.h"
#include "FramelessHelper/Widgets/framelesswidgetshelper.h"
FRAMELESSHELPER_USE_NAMESPACE
using namespace Global;
#endif

MainWindow::MainWindow(QWidget* parent) : SARibbonMainWindow(parent, false)
{
    if (!isUseRibbon()) {
        ui = new Ui::MainWindow;
        ui->setupUi(this);
    }
#if SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
    if (QWidget* w = menuWidget()) {
        auto fh = framelessHelper();
        fh->setHitTestVisible(w);
    }
#endif
}

MainWindow::~MainWindow()
{
    delete ui;
}
