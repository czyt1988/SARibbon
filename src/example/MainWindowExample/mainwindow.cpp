#include "mainwindow.h"
#include <QFile>
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
#include <QCalendarWidget>
#define PRINT_COST(ElapsedTimer, LastTime, STR)					      \
        do{									      \
                int ___TMP_INT = ElapsedTimer.elapsed();			      \
                qDebug() << STR << ___TMP_INT - LastTime << "(" << ___TMP_INT << ")"; \
                LastTime = ___TMP_INT;						      \
        }while(0)

MainWindow::MainWindow(QWidget *par) : SARibbonMainWindow(par)
{
        QElapsedTimer cost;
        int lastTimes = 0;

        cost.start();
        setWindowTitle(QStringLiteral("ribbon test"));
        m_edit = new QTextEdit(this);
        setCentralWidget(m_edit);
        PRINT_COST(cost, lastTimes, "setCentralWidget & setWindowTitle");
        SARibbonBar *ribbon = ribbonBar();

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

        createContextCategoryPage1(contextCategoryPage1);
        SARibbonCategory *contextCategoryPage2 = m_contextCategory->addCategoryPage(QStringLiteral("Page1"));

        createContextCategoryPage2(contextCategoryPage2);
        PRINT_COST(cost, lastTimes, "add contextCategory page");
        SARibbonQuickAccessBar *quickAccessBar = ribbon->quickAccessBar();

        quickAccessBar->addAction(new QAction(QIcon(":/icon/icon/chartDataManager.png"), "action1", this));
        quickAccessBar->addAction(new QAction(QIcon(":/icon/icon/figureIcon.png"), "action2", this));
        quickAccessBar->addSeparator();
        quickAccessBar->addAction(new QAction(QIcon(":/icon/icon/information.png"), "action3", this));
        QMenu *m = new QMenu("action menu", this);

        m->setIcon(QIcon(":/icon/icon/inRangDataRemove.png"));
        m->addAction("1");
        m->addAction("2");
        m->addAction("3");
        m->addAction("4");
        m->addAction("5");
        quickAccessBar->addMenu(m);
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
        act->setText(QStringLiteral("Save"));
        act->setShortcut(QKeySequence(QLatin1String("Ctrl+S")));
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
                this->ribbonBar()->setMinimumMode(b);
        });

        act = new QAction(this);
        act->setIcon(QIcon(":/icon/icon/save.png"));
        act->setText(QStringLiteral("show hide button"));
        act->setCheckable(true);
        pannel->addSmallAction(act);
        connect(act, &QAction::triggered, this, [this](bool b) {
                this->ribbonBar()->showMinimumModeButton(b);
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
        act->setText(QStringLiteral("DelayedPopup checkable"));
        act->setMenu(menu);
        btn = pannel->addLargeAction(act);
        btn->setPopupMode(QToolButton::DelayedPopup);
        btn->setCheckable(true);
        btn->setObjectName("DelayedPopup checkable");
        connect(act, &QAction::triggered, this, &MainWindow::onDelayedPopupCheckableTest);

        act = new QAction(this);
        act->setCheckable(true);
        act->setIcon(QIcon(":/icon/icon/folder.png"));
        act->setText(QStringLiteral("MenuButtonPopup checkable"));
        act->setMenu(menu);
        btn = pannel->addLargeAction(act);
        btn->setPopupMode(QToolButton::MenuButtonPopup);
        btn->setCheckable(true);
        connect(act, &QAction::triggered, this, &MainWindow::onMenuButtonPopupCheckableTest);


        SARibbonPannel *panne2 = page->addPannel(QStringLiteral("pannel 2"));

        act = new QAction(this);
        act->setCheckable(true);
        act->setIcon(QIcon(":/icon/icon/Graph-add.png"));
        act->setText(QStringLiteral("show Context"));
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

        QAction *act2 = new QAction(this);

        act2->setIcon(QIcon(":/icon/icon/529398.png"));
        act2->setText(QStringLiteral("delete Context"));
        btn = panne2->addLargeAction(act2);
        connect(act2, &QAction::triggered, this, [this, act](bool on) {
                if (this->m_contextCategory) {
                        this->ribbonBar()->destroyContextCategory(this->m_contextCategory);
                        this->m_contextCategory = nullptr;
                        act->setDisabled(true);
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
                com->addItem(QString("SARibbonComboBox test%1").arg(i+1));
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
                com->addItem(QString("item %1").arg(i+1));
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

        checkBox->setWindowIcon(QIcon(":/icon/icon/folder.png"));
        checkBox->setText("checkBox");
        pannel->addSmallWidget(checkBox);
        pannel->addSeparator();
        QCalendarWidget *calendarWidget = new QCalendarWidget(this);

        calendarWidget->setSizePolicy(QSizePolicy::Expanding,
            QSizePolicy::Expanding);
        calendarWidget->setObjectName(QStringLiteral("calendarWidget"));
        pannel->addLargeWidget(calendarWidget);
        pannel->setExpanding();
        QAction *optAct = new QAction(this);

        pannel->addOptionAction(optAct);

        pannel->setVisible(true);
}


void MainWindow::createCategoryOther(SARibbonCategory *page)
{
        SARibbonMenu *menu = new SARibbonMenu(this);
        QAction *item = menu->addAction(QIcon(":/icon/icon/folder.png"), QStringLiteral("1111111"));

        menu->addAction(QIcon(":/icon/icon/folder.png"), QStringLiteral("1"));
        menu->addAction(QIcon(":/icon/icon/folder.png"), QStringLiteral("2"));
        menu->addAction(QIcon(":/icon/icon/folder.png"), QStringLiteral("3"));
        menu->addAction(QIcon(":/icon/icon/folder.png"), QStringLiteral("4"));
        menu->addAction(QIcon(":/icon/icon/folder.png"), QStringLiteral("5"));
        menu->addAction(QIcon(":/icon/icon/folder.png"), QStringLiteral("6"));
        menu->addAction(QIcon(":/icon/icon/folder.png"), QStringLiteral("7"));
        menu->addAction(QIcon(":/icon/icon/folder.png"), QStringLiteral("8"));
        menu->addAction(QIcon(":/icon/icon/folder.png"), QStringLiteral("9"));

        SARibbonPannel *pannel = page->addPannel(QStringLiteral("pannel one"));
        SARibbonButtonGroupWidget *btnGroup = new SARibbonButtonGroupWidget(pannel);

        btnGroup->setFrameShape(QFrame::Box);
        btnGroup->addAction(new QAction(QIcon(":/icon/icon/figureIcon.png"), "", this));
        btnGroup->addAction(new QAction(QIcon(":/icon/icon/information.png"), "", this));
        pannel->addLargeWidget(btnGroup);

        SARibbonToolButton *btn;

        btn = pannel->addLargeAction(item);
        btn->setIcon(QIcon(":/icon/icon/folder.png"));
        btn->setText(QStringLiteral("un format icon"));
        btn->setPopupMode(QToolButton::DelayedPopup);
        btn->setFixedHeight(78);
        btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        btn->setMenu(menu);
        pannel->addLargeWidget(btn);

        btn = pannel->addLargeAction(item);
        btn->setIcon(QIcon(":/icon/icon/folder.png"));
        btn->setText(QStringLiteral("change page test"));
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
        QAction *appBtn = new QAction(QIcon(":/icon/icon/icon2.png"), tr("no application button and very long word test"), this);

        appBtn->setCheckable(true);
        connect(appBtn, &QAction::toggled, this, [&](bool b) {
                if (b) {
                        this->ribbonBar()->setApplicationButton(nullptr);
                }else{
                        this->ribbonBar()->setApplicationButton(new SARibbonApplicationButton());
                }
        });
        SARibbonToolButton *b = pannel->addLargeAction(appBtn);

        b->setObjectName("ApplicationButtonTest");

        QAction *useqss = new QAction(QIcon(":/icon/icon/icon2.png"), tr("use qss"), this);

        pannel->addLargeAction(useqss);
        connect(useqss, &QAction::triggered, this, [&]() {
                QFile f("ribbon.qss");
                if (!f.open(QIODevice::ReadWrite)) {
                        return;
                }
                QString qss(f.readAll());
                this->ribbonBar()->setStyleSheet(qss);
        });
}


void MainWindow::createContextCategoryPage1(SARibbonCategory *page)
{
        SARibbonPannel *pannel = page->addPannel(QStringLiteral("显示隐藏操作"));
        QAction *act = new QAction(this);

        act->setCheckable(true);
        act->setIcon(QIcon(":/icon/icon/530150.png"));
        act->setText(QStringLiteral("隐藏pannel"));
        pannel->addLargeAction(act);

        QAction *act2 = new QAction(this);

        act2->setDisabled(true);
        act2->setIcon(QIcon(":/icon/icon/529398.png"));
        act2->setText(QStringLiteral("disable"));
        pannel->addLargeAction(act2);
        connect(act2, &QAction::triggered, this, [act2](bool b) {
                act2->setDisabled(true);
        });

        QAction *act21 = new QAction(this);

        act21->setIcon(QIcon(":/icon/icon/529398.png"));
        act21->setText(QStringLiteral("解锁左边的按钮"));
        act21->setShortcut(QKeySequence(QLatin1String("Ctrl+E")));
        pannel->addLargeAction(act21);
        connect(act21, &QAction::triggered, this, [act2](bool b) {
                qDebug() << "act2->setEnabled(true);";
                act2->setEnabled(true);
                act2->setText(QStringLiteral("Enabled"));
        });

        QAction *act3 = new QAction(this);

        act3->setCheckable(true);
        act3->setIcon(QIcon(":/icon/icon/530767.png"));
        act3->setText(QStringLiteral("setText测试\r\nCtrl+D"));
        act3->setShortcut(QKeySequence(QLatin1String("Ctrl+D")));
        pannel->addLargeAction(act3);

        connect(act3, &QAction::toggled, this, [act3](bool b) {
                if (b) {
                        act3->setText(QStringLiteral("点击了"));
                }else{
                        act3->setText(QStringLiteral("setText测试"));
                }
        });
        //隐藏pannel
        QAction *act4 = new QAction(this);

        act4->setCheckable(true);
        act4->setIcon(QIcon(":/icon/icon/arror.png"));
        act4->setText(QStringLiteral("显示旁边的pannel"));
        pannel->addLargeAction(act4);

        SARibbonPannel *pannel2 = page->addPannel(QStringLiteral("用于隐藏显示的测试"));

        pannel2->addLargeAction(act3);

        connect(act4, &QAction::toggled, this, [act4, pannel2, this](bool b) {
                pannel2->setVisible(!b);
                if (b) {
                        act4->setText(QStringLiteral("隐藏旁边的pannel"));
                }else{
                        act4->setText(QStringLiteral("显示旁边的pannel"));
                }
                ribbonBar()->repaint();
        });

        SARibbonPannel *pannel3 = page->addPannel(QStringLiteral("action隐藏显示的测试"));
        QAction *act31 = new QAction(this);

        act31->setCheckable(true);
        act31->setChecked(true);
        act31->setIcon(QIcon(":/icon/icon/arror.png"));
        act31->setText(QStringLiteral("隐藏action2"));
        QAction *act32 = new QAction(this);

        act32->setIcon(QIcon(":/icon/icon/arror.png"));
        act32->setText(QStringLiteral("action 2"));
        QAction *act33 = new QAction(this);

        act33->setIcon(QIcon(":/icon/icon/arror.png"));
        act33->setText(QStringLiteral("action 3"));
        QAction *act34 = new QAction(this);

        act34->setIcon(QIcon(":/icon/icon/arror.png"));
        act34->setText(QStringLiteral("action 4"));
        pannel3->addLargeAction(act31);
        pannel3->addSmallAction(act32);
        pannel3->addSmallAction(act33);
        pannel3->addSmallAction(act34);
        connect(act31, &QAction::triggered, this, [act31, act32](bool b) {
                if (b) {
                        act32->setVisible(true);
                        act31->setText(QStringLiteral("隐藏action2"));
                }else{
                        act32->setVisible(false);
                        act31->setText(QStringLiteral("显示action2"));
                }
        });
}


void MainWindow::createContextCategoryPage2(SARibbonCategory *page)
{
        SARibbonPannel *pannel1 = page->addPannel(QStringLiteral("删除CategoryPage测试"));
        QAction *act11 = new QAction(this);

        act11->setIcon(QIcon(":/icon/icon/529398.png"));
        act11->setText(QStringLiteral("删除本页"));
        pannel1->addLargeAction(act11);
        connect(act11, &QAction::triggered, this, [this, page]() {
                this->ribbonBar()->removeCategory(page);
                page->deleteLater();
        });
        SARibbonPannel *pannel2 = page->addPannel(QStringLiteral("特殊布局"));

        pannel2->addAction(QStringLiteral("Large"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Large);
        pannel2->addAction(QStringLiteral("Small"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Small);
        pannel2->addAction(QStringLiteral("Small"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Small);
        pannel2->addSeparator();
        pannel2->addAction(QStringLiteral("Small"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Small);
        pannel2->addAction(QStringLiteral("Small"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Small);
        pannel2->addAction(QStringLiteral("Small"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Small);
        pannel2->addAction(QStringLiteral("Small"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Small);
        pannel2->addSeparator();
        pannel2->addAction(QStringLiteral("Large"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Large);
        pannel2->addAction(QStringLiteral("Medium"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Medium);
        pannel2->addAction(QStringLiteral("Medium"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Medium);
        pannel2->addAction(QStringLiteral("Large"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Small);
        pannel2->addAction(QStringLiteral("Large"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Small);
        pannel2->addAction(QStringLiteral("Large"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Small);
        pannel2->addAction(QStringLiteral("Medium"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Medium);
        pannel2->addAction(QStringLiteral("Medium"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Medium);
        pannel2->addAction(QStringLiteral("Large"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Large);
        pannel2->addSeparator();
        pannel2->addAction(QStringLiteral("Medium"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Medium);
        pannel2->addAction(QStringLiteral("Large"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Large);
        pannel2->addAction(QStringLiteral("Medium"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Medium);
        pannel2->addAction(QStringLiteral("Small"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Small);
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
