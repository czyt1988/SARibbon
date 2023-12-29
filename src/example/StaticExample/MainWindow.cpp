#include "MainWindow.h"
#include "SARibbon.h"
#include <QComboBox>
#include <QTimer>
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
    SARibbonPannel* pannel2     = otherCate->addPannel(tr("other"));
    pannel2->addAction(tr("action1"), QIcon(":/app/icon/action.svg"), QToolButton::InstantPopup);
    QComboBox* combo = new QComboBox(this);
    pannel2->addMediumWidget(combo);
    combo->addItem("RibbonThemeWindows7", static_cast< int >(SARibbonMainWindow::RibbonThemeWindows7));
    combo->addItem("RibbonThemeOffice2013", static_cast< int >(SARibbonMainWindow::RibbonThemeOffice2013));
    combo->addItem("RibbonThemeOffice2016Blue", static_cast< int >(SARibbonMainWindow::RibbonThemeOffice2016Blue));
    combo->addItem("RibbonThemeOffice2021Blue", static_cast< int >(SARibbonMainWindow::RibbonThemeOffice2021Blue));
    combo->addItem("RibbonThemeDark", static_cast< int >(SARibbonMainWindow::RibbonThemeDark));
    connect(combo, QOverload< int >::of(&QComboBox::currentIndexChanged), [ this, combo ](int index) {
        SARibbonMainWindow::RibbonTheme th = static_cast< SARibbonMainWindow::RibbonTheme >(combo->itemData(index).toInt());
        this->setRibbonTheme(th);
    });
    resize(800, 600);
    // 在构造函数设置主题会不完全生效
    QTimer::singleShot(0, this, [ this ]() { this->setRibbonTheme(SARibbonMainWindow::RibbonThemeDark); });

    // more example see MainWindowExample
}

MainWindow::~MainWindow()
{
}
