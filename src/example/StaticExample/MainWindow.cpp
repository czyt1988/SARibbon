#include "MainWindow.h"
#include "SARibbon.h"
MainWindow::MainWindow(QWidget* parent) : SARibbonMainWindow(parent)
{
    setWindowIcon(QIcon(":/app/icon/SA.svg"));
    SARibbonBar* bar = ribbonBar();
    bar->applicationButton()->setText(tr("  File  "));
    SARibbonCategory* mainCate = bar->addCategoryPage(tr("Main"));
    SARibbonPannel* pannel     = mainCate->addPannel(tr("actions"));
    pannel->addAction(tr("action1"), QIcon(":/app/icon/action.svg"), QToolButton::InstantPopup);
    pannel->addAction(tr("action2"), QIcon(":/app/icon/customize0.svg"), QToolButton::InstantPopup);
    pannel->addAction(tr("action3"), QIcon(":/app/icon/save.svg"), QToolButton::InstantPopup);
    pannel->addAction(tr("action4"), QIcon(":/app/icon/item.svg"), QToolButton::InstantPopup, SARibbonPannelItem::Small);
    pannel->addAction(tr("action5"), QIcon(":/app/icon/folder-star.svg"), QToolButton::InstantPopup, SARibbonPannelItem::Small);
    pannel->addAction(tr("action6"), QIcon(":/app/icon/test1.svg"), QToolButton::InstantPopup, SARibbonPannelItem::Small);
    SARibbonCategory* otherCate = bar->addCategoryPage(tr("Other"));
    Q_UNUSED(otherCate);
    resize(800, 600);
    // more example see MainWindowExample
}

MainWindow::~MainWindow()
{
}
