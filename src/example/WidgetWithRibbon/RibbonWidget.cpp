#include "RibbonWidget.h"
#include "InnerWidget.h"
#include "SARibbonBar.h"
#include "SARibbonCategory.h"
#include "SARibbonPannel.h"
#include "SARibbonMainWindow.h"
#include "SARibbonQuickAccessBar.h"
RibbonWidget::RibbonWidget(QWidget* parent) : SARibbonWidget(parent)
{
    // 直接创建SARibbonBar
    SARibbonBar* ribbonbar = ribbonBar();
    // QWidget模式下，没有必要再显示标题
    ribbonbar->setTitleVisible(false);
    // QWidget模式下，直接使用紧凑模式效果更好
    ribbonbar->setRibbonStyle(SARibbonBar::RibbonStyleCompactThreeRow);
    // 取消applicationbutton
    ribbonbar->setApplicationButton(nullptr);

    buildRibbon(ribbonbar);
    //
    setWidget(new InnerWidget());
}

RibbonWidget::~RibbonWidget()
{
}

void RibbonWidget::buildRibbon(SARibbonBar* bar)
{
    SARibbonCategory* page1 = new SARibbonCategory();
    page1->setCategoryName("page1");
    SARibbonPannel* pannel1 = new SARibbonPannel("pannel1", page1);
    page1->addPannel(pannel1);
    QAction* act = createAction("  save  ", ":/icon/icon/save.svg");
    act->setIconText("  save  ");
    pannel1->addLargeAction(act);
    pannel1->addLargeAction(createAction("open", ":/icon/icon/folder-star.svg"));
    pannel1->addSmallAction(createAction("action1", ":/icon/icon/action.svg"));
    pannel1->addSmallAction(createAction("action2", ":/icon/icon/action2.svg"));
    SARibbonPannel* pannel2 = new SARibbonPannel("pannel2", page1);
    page1->addPannel(pannel2);
    pannel2->addLargeAction(createAction("setting", ":/icon/icon/customize0.svg"));
    pannel2->addLargeAction(createAction("windowsflag", ":/icon/icon/windowsflag-normal.svg"));
    bar->addCategoryPage(page1);
    // 加入主题
	mComboTheme = new QComboBox();
    mComboTheme->addItem("Theme Win7", static_cast< int >(SARibbonTheme::RibbonThemeWindows7));
    mComboTheme->addItem("Theme Office2013", static_cast< int >(SARibbonTheme::RibbonThemeOffice2013));
    mComboTheme->addItem("Theme Office2016 Blue", static_cast< int >(SARibbonTheme::RibbonThemeOffice2016Blue));
    mComboTheme->addItem("Theme Office2021 Blue", static_cast< int >(SARibbonTheme::RibbonThemeOffice2021Blue));
    mComboTheme->addItem("Theme Dark", static_cast< int >(SARibbonTheme::RibbonThemeDark));
    mComboTheme->addItem("Theme Dark2", static_cast< int >(SARibbonTheme::RibbonThemeDark2));
	mComboTheme->setCurrentIndex(mComboTheme->findData(static_cast< int >(SARibbonTheme::RibbonThemeOffice2013)));
	connect(mComboTheme,
	        QOverload< int >::of(&QComboBox::currentIndexChanged),
	        this,
            &RibbonWidget::onRibbonThemeComboBoxCurrentIndexChanged);
	pannel2->addSeparator();
	pannel2->addSmallWidget(mComboTheme);

    SARibbonQuickAccessBar* qbar = bar->quickAccessBar();
    qbar->addAction(createAction("undo", ":/icon/icon/undo.svg"));
    qbar->addAction(createAction("redo", ":/icon/icon/redo.svg"));
}

QAction* RibbonWidget::createAction(const QString& text, const QString& iconurl)
{
    QAction* act = new QAction(this);
    act->setText(text);
    act->setIcon(QIcon(iconurl));
    act->setObjectName(text);
    connect(act, &QAction::triggered, this, [ this, act ]() {
        InnerWidget* w = qobject_cast< InnerWidget* >(widget());
        if (w) {
            w->appendText(QString("action(%1) triggered").arg(act->text()));
        }
    });
    return act;
}

void RibbonWidget::onRibbonThemeComboBoxCurrentIndexChanged(int index)
{
	SARibbonTheme t = static_cast< SARibbonTheme >(mComboTheme->itemData(index).toInt());
    setRibbonTheme(t);
}
