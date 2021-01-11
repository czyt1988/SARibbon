#include "mainwindow.h"
#include <QTextEdit>
#include <QAbstractButton>
#include "SARibbonBar.h"
#include "SARibbonCategory.h"
#include <QPushButton>
#include "SARibbonPannel.h"
#include "SARibbonToolButton.h"
#include <QAction>
#include <QMenu>
#include <QDebug>
#include <QElapsedTimer>
#include <QRadioButton>
#include <QButtonGroup>
#include "SARibbonMenu.h"
#include "SARibbonComboBox.h"
#include "SARibbonLineEdit.h"
#include "SARibbonGallery.h"
#include "SARibbonCheckBox.h"
#include "SARibbonQuickAccessBar.h"
#include "SARibbonButtonGroupWidget.h"
#include "SARibbonApplicationButton.h"
#define PRINT_COST(ElapsedTimer, LastTime, STR)					      \
    do{									      \
        int ___TMP_INT = ElapsedTimer.elapsed();			      \
        qDebug() << STR << ___TMP_INT - LastTime << "(" << ___TMP_INT << ")"; \
        LastTime = ___TMP_INT;						      \
    }while(0)

MainWindow::MainWindow(QWidget *par) : SARibbonMainWindow(par)
{
#if 1
    QElapsedTimer cost;
    int lastTimes = 0;
    cost.start();
    setWindowTitle(QStringLiteral("ribbon test"));
    m_edit = new QTextEdit(this);
    setCentralWidget(m_edit);
    PRINT_COST(cost, lastTimes, "setCentralWidget & setWindowTitle");
    SARibbonBar *ribbon = ribbonBar();
    QFont f = ribbon->font();
    f.setFamily("微软雅黑");
    ribbon->setFont(f);
    ribbon->applicationButton()->setText(QStringLiteral("File"));
    SARibbonCategory *categoryMain = ribbon->addCategoryPage(QStringLiteral("Main"));
    PRINT_COST(cost, lastTimes, "new main page");
    createCategoryMain(categoryMain);
    PRINT_COST(cost, lastTimes, "add main page element");
    SARibbonCategory *categoryOther = ribbon->addCategoryPage(QStringLiteral("Other"));
    createCategoryOther(categoryOther);
    PRINT_COST(cost, lastTimes, "add other page");
    m_contextCategory = ribbon->addContextCategory(QStringLiteral("context"), Qt::red, 1);
    SARibbonCategory *contextCategoryPage1 = m_contextCategory->addCategoryPage(QStringLiteral("Page1"));
    SARibbonCategory *contextCategoryPage2 = m_contextCategory->addCategoryPage(QStringLiteral("Page1"));
    PRINT_COST(cost, lastTimes, "add contextCategory page");
#endif
    ribbon->quickAccessBar()->addButton(new QAction(QIcon(":/icon/icon/chartDataManager.png"), "action1", this));
    ribbon->quickAccessBar()->addButton(new QAction(QIcon(":/icon/icon/figureIcon.png"), "action2", this));
    ribbon->quickAccessBar()->addButton(new QAction(QIcon(":/icon/icon/information.png"), "action3", this));
    ribbon->quickAccessBar()->addButton(new QAction(QIcon(":/icon/icon/inRangDataRemove.png"), "action4", this));
    showMaximized();
}


void MainWindow::onShowContextCategory(bool on)
{
    ribbonBar()->setContextCategoryVisible(m_contextCategory, on);
}


void MainWindow::onStyleClicked(int id)
{
    ribbonBar()->setRibbonStyle(static_cast<SARibbonBar::RibbonStyle>(id));
}


void MainWindow::createCategoryMain(SARibbonCategory *page)
{
    QElapsedTimer cost;

    cost.start();
    SARibbonToolButton *btn;
    SARibbonMenu *menu = new SARibbonMenu(this);

    menu->addAction(QIcon(":/icon/icon/folder.png"), QStringLiteral("1111111"));
    menu->addAction(QIcon(":/icon/icon/folder.png"), QStringLiteral("1"));
    menu->addAction(QIcon(":/icon/icon/folder.png"), QStringLiteral("11"));
    menu->addAction(QIcon(":/icon/icon/folder.png"), QStringLiteral("111"));
    menu->addAction(QIcon(":/icon/icon/folder.png"), QStringLiteral("11111"));
    menu->addAction(QIcon(":/icon/icon/folder.png"), QStringLiteral("1111111"));
    menu->addAction(QIcon(":/icon/icon/folder.png"), QStringLiteral("11111111"));
    menu->addAction(QIcon(":/icon/icon/folder.png"), QStringLiteral("1111111111"));
    menu->addAction(QIcon(":/icon/icon/folder.png"), QStringLiteral("1111111111111"));
    menu->addAction(QIcon(":/icon/icon/folder.png"), QStringLiteral("1111111111111111111111111111"));

    int lastTimes = 0;
    SARibbonPannel *pannel = page->addPannel(QStringLiteral("Panel 1"));

    PRINT_COST(cost, lastTimes, "addPannel pannel 1");

    QAction *act = new QAction(this);

    act->setIcon(QIcon(":/icon/icon/save.png"));
    act->setText(QStringLiteral("SAve"));
    PRINT_COST(cost, lastTimes, "new Large Action");
    pannel->addLargeAction(act);
    connect(act, &QAction::triggered, this, [this](bool b) {
        this->m_edit->append("action clicked");
    });
    PRINT_COST(cost, lastTimes, "add 1 Large Action");

    act = new QAction(this);
    act->setIcon(QIcon(":/icon/icon/save.png"));
    act->setText(QStringLiteral("hide ribbon"));
    act->setCheckable(true);
    pannel->addSmallAction(act);
    connect(act, &QAction::triggered, this, [this](bool b) {
        this->ribbonBar()->setHideMode(b);
    });

    act = new QAction(this);
    act->setIcon(QIcon(":/icon/icon/save.png"));
    act->setText(QStringLiteral("show hide button"));
    act->setCheckable(true);
    pannel->addSmallAction(act);
    connect(act, &QAction::triggered, this, [this](bool b) {
        this->ribbonBar()->showHideModeButton(b);
    });
    QButtonGroup *g = new QButtonGroup(page);
    QRadioButton *r = new QRadioButton();

    r->setText(QStringLiteral("use office style"));
    r->setChecked(true);
    pannel->addSmallWidget(r);
    g->addButton(r, SARibbonBar::OfficeStyle);
    r = new QRadioButton();
    r->setText(QStringLiteral("use wps style"));
    r->setChecked(false);
    pannel->addSmallWidget(r);
    g->addButton(r, SARibbonBar::WpsLiteStyle);
    r = new QRadioButton();
    r->setText(QStringLiteral("use office 2row style"));
    r->setChecked(false);
    pannel->addSmallWidget(r);
    g->addButton(r, SARibbonBar::OfficeStyleTwoRow);
    r = new QRadioButton();
    r->setText(QStringLiteral("use wps 2row style"));
    r->setChecked(false);
    pannel->addSmallWidget(r);
    g->addButton(r, SARibbonBar::WpsLiteStyleTwoRow);

    connect(g, QOverload<int>::of(&QButtonGroup::buttonClicked), this, &MainWindow::onStyleClicked);

    act = new QAction(this);
    act->setIcon(QIcon(":/icon/icon/folder.png"));
    act->setText(QStringLiteral("test 3"));
    act->setMenu(menu);
    btn = pannel->addSmallAction(act);
    btn->setPopupMode(QToolButton::MenuButtonPopup);

    act = new QAction(this);
    act->setIcon(QIcon(":/icon/icon/folder.png"));
    act->setText(QStringLiteral("test 4"));
    act->setMenu(menu);
    btn = pannel->addSmallAction(act);
    btn->setPopupMode(QToolButton::InstantPopup);

    pannel->addSeparator();



    act = new QAction(this);
    act->setIcon(QIcon(":/icon/icon/folder.png"));
    act->setText(QStringLiteral("DelayedPopup"));
    act->setMenu(menu);
    btn = pannel->addLargeAction(act);
    btn->setPopupMode(QToolButton::DelayedPopup);
    btn->setObjectName("SA_DelayedPopup");
    connect(act, &QAction::triggered, this, &MainWindow::onDelayedPopupCheckabletriggered);

    act = new QAction(this);
    act->setIcon(QIcon(":/icon/icon/folder.png"));
    act->setText(QStringLiteral("MenuButtonPopup"));
    act->setMenu(menu);
    btn = pannel->addLargeAction(act);
    btn->setObjectName(QStringLiteral("MenuButtonPopup"));
    btn->setPopupMode(QToolButton::MenuButtonPopup);
    connect(act, &QAction::triggered, this, &MainWindow::onMenuButtonPopupCheckabletriggered);

    act = new QAction(this);
    act->setIcon(QIcon(":/icon/icon/Graph-add.png"));
    act->setText(QStringLiteral("InstantPopup"));
    act->setMenu(menu);
    btn = pannel->addLargeAction(act);
    btn->setObjectName("buttonInstantPopup");
    btn->setPopupMode(QToolButton::InstantPopup);
    connect(act, &QAction::triggered, this, &MainWindow::onInstantPopupCheckabletriggered);

    act = new QAction(this);
    act->setCheckable(true);
    act->setIcon(QIcon(":/icon/icon/folder.png"));
    act->setText(QStringLiteral("DelayedPopup"));
    act->setMenu(menu);
    btn = pannel->addLargeAction(act);
    btn->setPopupMode(QToolButton::DelayedPopup);
    btn->setCheckable(true);
    connect(act, &QAction::triggered, this, &MainWindow::onDelayedPopupCheckableTest);

    act = new QAction(this);
    act->setCheckable(true);
    act->setIcon(QIcon(":/icon/icon/folder.png"));
    act->setText(QStringLiteral("MenuButtonPopup\n checkable"));
    act->setMenu(menu);
    btn = pannel->addLargeAction(act);
    btn->setPopupMode(QToolButton::MenuButtonPopup);
    btn->setCheckable(true);
    connect(act, &QAction::triggered, this, &MainWindow::onMenuButtonPopupCheckableTest);

    act = new QAction(this);
    act->setCheckable(true);
    act->setIcon(QIcon(":/icon/icon/Graph-add.png"));
    act->setText(QStringLiteral("InstantPopup"));
    act->setMenu(menu);
    btn = pannel->addLargeAction(act);
    btn->setCheckable(true);
    btn->setPopupMode(QToolButton::InstantPopup);
    connect(act, &QAction::triggered, this, &MainWindow::onInstantPopupCheckableTest);

    SARibbonPannel *panne2 = page->addPannel(QStringLiteral("pannel 2"));

    act = new QAction(this);
    act->setCheckable(true);
    act->setIcon(QIcon(":/icon/icon/Graph-add.png"));
    act->setText(QStringLiteral("show\nContext"));
    btn = panne2->addLargeAction(act);
    btn->setCheckable(true);
    //btn->setPopupMode(QToolButton::InstantPopup);
    connect(act, &QAction::triggered, this, [this](bool on) {
        if (on) {
            this->ribbonBar()->showContextCategory(this->m_contextCategory);
        }else{
            this->ribbonBar()->hideContextCategory(this->m_contextCategory);
        }
    });

    act = new QAction(this);
    act->setCheckable(true);
    act->setIcon(QIcon(":/icon/icon/Graph-add.png"));
    act->setText(QStringLiteral("unactive"));
    act->setMenu(menu);
    btn = panne2->addLargeAction(act);
    btn->setCheckable(true);
    btn->setPopupMode(QToolButton::InstantPopup);
    btn->setEnabled(false);


    SARibbonComboBox *com = new SARibbonComboBox(this);

    com->setSizePolicy(QSizePolicy::Expanding,
        QSizePolicy::Fixed);
    com->setWindowIcon(QIcon(":/icon/icon/Graph-add.png"));
    com->setWindowTitle("ComboBox");
    for (int i = 0; i < 40; ++i)
    {
        com->addItem("SARibbonComboBox test");
    }
    com->setEditable(true);
    pannel->addSmallWidget(com);

    com = new SARibbonComboBox(this);
    com->setSizePolicy(QSizePolicy::Expanding,
        QSizePolicy::Fixed);
    com->setWindowIcon(QIcon(":/icon/icon/folder.png"));
    com->setWindowTitle("ComboBox Editable");
    for (int i = 0; i < 40; ++i)
    {
        com->addItem("111111111111");
    }
    pannel->addSmallWidget(com);

    SARibbonLineEdit *lineEdit = new SARibbonLineEdit(this);

    lineEdit->setSizePolicy(QSizePolicy::Expanding,
        QSizePolicy::Fixed);
    lineEdit->setWindowIcon(QIcon(":/icon/icon/folder.png"));
    lineEdit->setWindowTitle("Line Edit");
    lineEdit->setText("SARibbonLineEdit");
    pannel->addSmallWidget(lineEdit);
    QWidget *w = lineEdit->parentWidget();

    while (w)
    {
        qDebug() << w->metaObject()->className();
        w = w->parentWidget();
    }
    SARibbonCheckBox *checkBox = new SARibbonCheckBox(this);

    checkBox->setSizePolicy(QSizePolicy::Expanding,
        QSizePolicy::Fixed);
    checkBox->setWindowIcon(QIcon(":/icon/icon/folder.png"));
    checkBox->setText("checkBox");
    pannel->addSmallWidget(checkBox);

    pannel->setExpanding();
    QAction *optAct = new QAction(this);

    pannel->addOptionAction(optAct);
}


void MainWindow::createCategoryOther(SARibbonCategory *page)
{
    SARibbonMenu *menu = new SARibbonMenu(this);
    QAction *item = menu->addAction(QIcon(":/icon/icon/folder.png"), QStringLiteral("1111111"));

    menu->addAction(QIcon(":/icon/icon/folder.png"), QStringLiteral("1111111"));
    menu->addAction(QIcon(":/icon/icon/folder.png"), QStringLiteral("1111111"));
    menu->addAction(QIcon(":/icon/icon/folder.png"), QStringLiteral("1111111"));
    menu->addAction(QIcon(":/icon/icon/folder.png"), QStringLiteral("1111111"));
    menu->addAction(QIcon(":/icon/icon/folder.png"), QStringLiteral("1111111"));
    menu->addAction(QIcon(":/icon/icon/folder.png"), QStringLiteral("1111111"));
    menu->addAction(QIcon(":/icon/icon/folder.png"), QStringLiteral("1111111"));
    menu->addAction(QIcon(":/icon/icon/folder.png"), QStringLiteral("1111111"));
    menu->addAction(QIcon(":/icon/icon/folder.png"), QStringLiteral("1111111"));

    SARibbonPannel *pannel = page->addPannel(QStringLiteral("pannel 1"));
    SARibbonButtonGroupWidget *btnGroup = new SARibbonButtonGroupWidget(pannel);

    btnGroup->setFrameShape(QFrame::Box);
    btnGroup->addButton(new QAction(QIcon(":/icon/icon/figureIcon.png"), "", this));
    btnGroup->addButton(new QAction(QIcon(":/icon/icon/information.png"), "", this));
    pannel->addLargeWidget(btnGroup);

    SARibbonToolButton *btn;

    btn = pannel->addLargeAction(item);
    btn->setIcon(QIcon(":/icon/icon/folder.png"));
    btn->setText(QStringLiteral("un format\nicon"));
    btn->setPopupMode(QToolButton::DelayedPopup);
    btn->setFixedHeight(78);
    btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btn->setMenu(menu);
    pannel->addLargeWidget(btn);

    btn = pannel->addLargeAction(item);
    btn->setIcon(QIcon(":/icon/icon/folder.png"));
    btn->setText(QStringLiteral("change page\ntest"));
    btn->setPopupMode(QToolButton::MenuButtonPopup);
    btn->setFixedHeight(78);
    btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btn->setMenu(menu);
    pannel->addLargeWidget(btn);

    btn = pannel->addLargeAction(item);
    btn->setIcon(QIcon(":/icon/icon/folder.png"));
    btn->setText(QStringLiteral("LargeBtn"));
    btn->setPopupMode(QToolButton::InstantPopup);
    btn->setFixedHeight(78);
    btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btn->setMenu(menu);
    pannel->addLargeWidget(btn);

    SARibbonGallery *gallery = pannel->addGallery();
    SARibbonGalleryGroup *group = gallery->addGalleryGroup();

    for (int i = 0; i < 100; ++i)
    {
        group->addItem(QIcon(":/icon/icon/folder.png"));
    }
    QAction *optAct = new QAction(this);

    pannel->addOptionAction(optAct);
    pannel->setObjectName("debug");

    pannel->addSeparator();
    QAction *appBtn = new QAction(QIcon(":/icon/icon/icon2.png"), tr("no \napplication button"), this);

    appBtn->setCheckable(true);
    connect(appBtn, &QAction::toggled, this, [&](bool b) {
        if (b) {
            this->ribbonBar()->setApplicationButton(nullptr);
        }else{
            this->ribbonBar()->setApplicationButton(new SARibbonApplicationButton());
        }
    });
    pannel->addLargeAction(appBtn);
}


void MainWindow::onMenuButtonPopupCheckableTest(bool b)
{
    m_edit->append(QStringLiteral("MenuButtonPopupCheckableTest : %1").arg(b));
}


void MainWindow::onInstantPopupCheckableTest(bool b)
{
    m_edit->append(QStringLiteral("InstantPopupCheckableTest : %1").arg(b));
}


void MainWindow::onDelayedPopupCheckableTest(bool b)
{
    m_edit->append(QStringLiteral("DelayedPopupCheckableTest : %1").arg(b));
}


void MainWindow::onMenuButtonPopupCheckabletriggered(bool b)
{
    m_edit->append(QStringLiteral("MenuButtonPopupCheckabletriggered : %1").arg(b));
}


void MainWindow::onInstantPopupCheckabletriggered(bool b)
{
    m_edit->append(QStringLiteral("InstantPopupCheckabletriggered : %1").arg(b));
}


void MainWindow::onDelayedPopupCheckabletriggered(bool b)
{
    m_edit->append(QStringLiteral("DelayedPopupCheckabletriggered : %1").arg(b));
}
