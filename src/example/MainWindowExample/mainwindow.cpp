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
#include <QStatusBar>
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
#include "SARibbonCustomizeWidget.h"
#include <QCalendarWidget>
#include "SARibbonCustomizeDialog.h"
#include <QXmlStreamWriter>
#include <QTextStream>
#include "SAFramelessHelper.h"
#define PRINT_COST_START()                                                                                             \
    QElapsedTimer __TMP_COST;                                                                                          \
    int __TMP_LASTTIMES = 0

#define PRINT_COST(STR)                                                                                                \
    do {                                                                                                               \
        int ___TMP_INT = __TMP_COST.elapsed();                                                                         \
        qDebug() << STR << " cost " << ___TMP_INT - __TMP_LASTTIMES << " ms (" << ___TMP_INT << ")";                   \
        __TMP_LASTTIMES = ___TMP_INT;                                                                                  \
    } while (0)

MainWindow::MainWindow(QWidget* par) : SARibbonMainWindow(par), m_customizeWidget(nullptr)
{
    PRINT_COST_START();
    SAFramelessHelper* helper = framelessHelper();

    helper->setRubberBandOnResize(false);

    QElapsedTimer cost;
    int lastTimes = 0;

    cost.start();
    setWindowTitle(("ribbon test"));
    m_edit = new QTextEdit(this);
    setCentralWidget(m_edit);
    setStatusBar(new QStatusBar());
    PRINT_COST("setCentralWidget & setWindowTitle");
    SARibbonBar* ribbon = ribbonBar();

    //添加文件标签页
    ribbon->applicationButton()->setText(("File"));
    SARibbonCategory* categoryMain = ribbon->addCategoryPage(("Main"));

    PRINT_COST("new main page");
    createCategoryMain(categoryMain);
    PRINT_COST("add main page element");

    //添加其他标签页
    SARibbonCategory* categoryOther = new SARibbonCategory();

    categoryOther->setCategoryName(("Other"));
    categoryOther->setObjectName(("categoryOther"));
    ribbon->addCategoryPage(categoryOther);
    createCategoryOther(categoryOther);
    PRINT_COST("add other page");

    //添加删除标签页
    SARibbonCategory* categoryDelete = new SARibbonCategory();

    categoryDelete->setCategoryName(("Delete"));
    categoryDelete->setObjectName(("categoryDelete"));
    ribbon->addCategoryPage(categoryDelete);
    createCategoryDelete(categoryDelete);
    PRINT_COST("add category delete page");
    //添加尺寸标签页
    SARibbonCategory* categorySize = new SARibbonCategory();

    categorySize->setCategoryName(("Size"));
    categorySize->setObjectName(("categorySize"));
    ribbon->addCategoryPage(categorySize);
    createCategorySize(categorySize);
    PRINT_COST("add category delete page");
    createContextCategory1();
    PRINT_COST("add context1 category page");
    createContextCategory2();
    PRINT_COST("add context2 category page");
    SARibbonQuickAccessBar* quickAccessBar = ribbon->quickAccessBar();

    quickAccessBar->addAction(new QAction(QIcon(":/icon/icon/chartDataManager.png"), "action1", this));
    quickAccessBar->addAction(new QAction(QIcon(":/icon/icon/figureIcon.png"), "action2", this));
    quickAccessBar->addSeparator();
    quickAccessBar->addAction(new QAction(QIcon(":/icon/icon/information.png"), "action3", this));
    QMenu* m = new QMenu("action menu", this);

    m->setIcon(QIcon(":/icon/icon/inRangDataRemove.png"));
    m->addAction("1");
    m->addAction("2");
    m->addAction("3");
    m->addAction("4");
    m->addAction("5");
    quickAccessBar->addMenu(m);
    SARibbonButtonGroupWidget* rightBar = ribbon->rightButtonGroup();

    rightBar->addAction("Help", QIcon(":/icon/icon/help.png"));
    QAction* customize = new QAction(QIcon(":/icon/icon/chartDataManager.png"), "customize", this);

    quickAccessBar->addAction(customize);
    addSomeOtherAction();

    connect(customize, &QAction::triggered, this, [&]() {
        if (nullptr == m_customizeWidget) {
            m_customizeWidget = new SARibbonCustomizeWidget(this, this, Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint | Qt::Dialog);
            m_customizeWidget->setWindowModality(Qt::ApplicationModal);  //设置阻塞类型
            m_customizeWidget->setAttribute(Qt::WA_ShowModal, true);     //属性设置 true:模态 false:非模态
            m_customizeWidget->setupActionsManager(m_actMgr);
        }
        m_customizeWidget->show();
        m_customizeWidget->applys();
    });

    QAction* customize2 = new QAction(QIcon(":/icon/icon/chartDataManager.png"), "customize2", this);

    quickAccessBar->addAction(customize2);
    connect(customize2, &QAction::triggered, this, [&]() {
        SARibbonCustomizeDialog dlg(this);
        dlg.setupActionsManager(m_actMgr);
        dlg.fromXml("customize.xml");
        if (SARibbonCustomizeDialog::Accepted == dlg.exec()) {
            dlg.applys();
            QByteArray str;
            QXmlStreamWriter xml(&str);
            xml.setAutoFormatting(true);
            xml.setAutoFormattingIndent(2);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)  // QXmlStreamWriter always encodes XML in UTF-8.
            xml.setCodec("utf-8");
#endif
            xml.writeStartDocument();
            bool isok = dlg.toXml(&xml);
            xml.writeEndDocument();
            if (isok) {
                QFile f("customize.xml");
                if (f.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate)) {
                    QTextStream s(&f);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)  // QTextStream always encodes XML in UTF-8.
                    s.setCodec("utf-8");
#endif
                    s << str;
                    s.flush();
                }
                m_edit->append("write xml:");
                m_edit->append(str);
            }
        }
    });

    setMinimumWidth(500);
    //
    showMaximized();
    //
    setWindowIcon(QIcon(":/icon/icon/icon2.png"));
}

void MainWindow::onShowContextCategory(bool on)
{
    if (m_contextCategory == nullptr) {
        createContextCategory1();
    }
    if (on) {
        this->ribbonBar()->showContextCategory(m_contextCategory);
    } else {
        this->ribbonBar()->hideContextCategory(m_contextCategory);
    }
}

void MainWindow::onStyleClicked(int id)
{
    ribbonBar()->setRibbonStyle(static_cast< SARibbonBar::RibbonStyle >(id));
}

void MainWindow::createCategoryMain(SARibbonCategory* page)
{
    //! 1
    //! pannel 1 start
    //!

    //使用addPannel函数来创建SARibbonPannel，效果和new SARibbonPannel再addPannel一样
    SARibbonPannel* pannel = page->addPannel(("Panel 1"));

    QAction* actSave = createAction(tr("Save"), ":/icon/icon/save.svg");
    actSave->setShortcut(QKeySequence(QLatin1String("Ctrl+S")));
    pannel->addLargeAction(actSave);
    connect(actSave, &QAction::triggered, this, [this](bool b) {
        Q_UNUSED(b);
        this->m_edit->append("actSaveion clicked");
    });

    QAction* actHideRibbon = createAction(tr("hide ribbon"), ":/icon/icon/hideRibbon.svg", "actHideRibbon");
    actHideRibbon->setCheckable(true);
    pannel->addSmallAction(actHideRibbon);
    connect(actHideRibbon, &QAction::triggered, this, [this](bool b) { this->ribbonBar()->setMinimumMode(b); });
    connect(ribbonBar(), &SARibbonBar::ribbonModeChanged, this, [actHideRibbon](SARibbonBar::RibbonMode nowNode) {
        actHideRibbon->setChecked(nowNode == SARibbonBar::MinimumRibbonMode);
    });

    QAction* actShowHideButton = createAction(tr("show hide button"), ":/icon/icon/showHideButton.svg", "show hide button");
    actShowHideButton->setCheckable(true);
    pannel->addSmallAction(actShowHideButton);
    connect(actShowHideButton, &QAction::triggered, this, [this](bool b) { this->ribbonBar()->showMinimumModeButton(b); });
    actShowHideButton->trigger();

    QButtonGroup* g = new QButtonGroup(page);

    QRadioButton* r = new QRadioButton();
    r->setText(tr("use office style"));
    r->setObjectName(("use office style"));
    r->setChecked(true);
    pannel->addSmallWidget(r);
    g->addButton(r, SARibbonBar::OfficeStyle);

    r = new QRadioButton();
    r->setObjectName(("use wps style"));
    r->setText(tr("use wps style"));
    r->setChecked(false);
    pannel->addSmallWidget(r);
    g->addButton(r, SARibbonBar::WpsLiteStyle);

    r = new QRadioButton();
    r->setObjectName(("use office 2row style"));
    r->setText(tr("use office 2 row style"));
    r->setChecked(false);
    pannel->addSmallWidget(r);
    g->addButton(r, SARibbonBar::OfficeStyleTwoRow);

    r = new QRadioButton();
    r->setObjectName(("use wps 2row style"));
    r->setText(tr("use wps 2row style"));
    r->setChecked(false);
    pannel->addSmallWidget(r);
    g->addButton(r, SARibbonBar::WpsLiteStyleTwoRow);

//    connect(g, QOverload<int>::of(&QButtonGroup::buttonClicked), this, &MainWindow::onStyleClicked);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    connect(g, static_cast< void (QButtonGroup::*)(int) >(&QButtonGroup::idClicked), this, &MainWindow::onStyleClicked);
#else
    connect(g, static_cast< void (QButtonGroup::*)(int) >(&QButtonGroup::buttonClicked), this, &MainWindow::onStyleClicked);
#endif
    SARibbonToolButton* btn;
    SARibbonMenu* menu = new SARibbonMenu(this);
    QAction* a         = nullptr;
    {
        QIcon itemicon = QIcon(":/icon/icon/item.svg");
        for (int i = 0; i < 30; ++i) {
            a = menu->addAction(itemicon, tr("item %1").arg(i + 1));
            a->setObjectName(QStringLiteral("item %1").arg(i + 1));
        }
    }

    QAction* act = createAction(tr("test 1"), ":/icon/icon/test1.svg");
    act->setMenu(menu);
    act->setToolTip(tr("use QToolButton::MenuButtonPopup mode"));
    btn = pannel->addSmallAction(act);
    btn->setPopupMode(QToolButton::MenuButtonPopup);

    act = createAction(tr("test 2"), ":/icon/icon/test2.svg");
    act->setMenu(menu);
    act->setToolTip(tr("use QToolButton::InstantPopup mode"));
    btn = pannel->addSmallAction(act);
    btn->setPopupMode(QToolButton::InstantPopup);

    pannel->addSeparator();

    act = createAction(tr("Delayed Popup"), ":/icon/icon/folder-cog.svg");
    act->setMenu(menu);
    btn = pannel->addLargeAction(act);
    btn->setPopupMode(QToolButton::DelayedPopup);
    connect(act, &QAction::triggered, this, &MainWindow::onDelayedPopupCheckabletriggered);

    act = createAction(tr("Menu Button Popup"), ":/icon/icon/folder-star.svg");
    act->setMenu(menu);
    btn = pannel->addLargeAction(act);
    btn->setPopupMode(QToolButton::MenuButtonPopup);
    connect(act, &QAction::triggered, this, &MainWindow::onMenuButtonPopupCheckabletriggered);

    act = createAction(tr("Instant Popup"), ":/icon/icon/folder-stats.svg");
    act->setMenu(menu);
    btn = pannel->addLargeAction(act);
    btn->setPopupMode(QToolButton::InstantPopup);
    connect(act, &QAction::triggered, this, &MainWindow::onInstantPopupCheckabletriggered);

    act = createAction(tr("Delayed Popup checkable"), ":/icon/icon/folder-table.svg");
    act->setCheckable(true);
    act->setMenu(menu);
    btn = pannel->addLargeAction(act);
    btn->setPopupMode(QToolButton::DelayedPopup);
    connect(act, &QAction::triggered, this, &MainWindow::onDelayedPopupCheckableTest);

    act = createAction(tr("Menu Button Popup checkable"), ":/icon/icon/folder-checkmark.svg");
    act->setCheckable(true);
    act->setMenu(menu);
    btn = pannel->addLargeAction(act);
    btn->setPopupMode(QToolButton::MenuButtonPopup);
    connect(act, &QAction::triggered, this, &MainWindow::onMenuButtonPopupCheckableTest);

    act = createAction(tr("disable action"), ":/icon/icon/disable.svg");
    act->setCheckable(true);
    act->setMenu(menu);
    act->setEnabled(false);
    pannel->addLargeAction(act);

    //! 2
    //! pannel 2 start
    //!
    SARibbonPannel* panne2 = page->addPannel(("pannel 2"));

    QAction* actShowContext = createAction(tr("show Context"), ":/icon/icon/showContext.svg");
    actShowContext->setCheckable(true);
    panne2->addLargeAction(actShowContext);
    connect(actShowContext, &QAction::triggered, this, &MainWindow::onShowContextCategory);

    QAction* actDeleteContext = createAction(tr("delete Context"), ":/icon/icon/deleteContext.svg");
    btn                       = panne2->addLargeAction(actDeleteContext);
    connect(actDeleteContext, &QAction::triggered, this, [this, act](bool on) {
        if (this->m_contextCategory) {
            this->ribbonBar()->destroyContextCategory(this->m_contextCategory);
            this->m_contextCategory = nullptr;
            act->setDisabled(true);
        }
    });

    //! 3
    //! pannel 3 start -> widget test
    //!

    SARibbonPannel* pannelWidgetTest = page->addPannel(tr("widget test"));
    pannelWidgetTest->setObjectName(QStringLiteral(u"pannelWidgetTest"));

    SARibbonComboBox* com = new SARibbonComboBox(this);

    com->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    com->setWindowTitle(tr("ComboBox"));
    for (int i = 0; i < 40; ++i) {
        com->addItem(QString("SARibbonComboBox test%1").arg(i + 1));
    }
    com->setEditable(true);
    pannelWidgetTest->addSmallWidget(com);

    com = new SARibbonComboBox(this);
    com->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    com->setWindowTitle("ComboBox Editable");
    for (int i = 0; i < 40; ++i) {
        com->addItem(QString("item %1").arg(i + 1));
    }
    pannelWidgetTest->addSmallWidget(com);

    SARibbonLineEdit* lineEdit = new SARibbonLineEdit(this);

    lineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    lineEdit->setWindowTitle("Line Edit");
    lineEdit->setText("SARibbonLineEdit");
    pannelWidgetTest->addSmallWidget(lineEdit);
    QWidget* w = lineEdit->parentWidget();

    while (w) {
        qDebug() << w->metaObject()->className();
        w = w->parentWidget();
    }
    SARibbonCheckBox* checkBox = new SARibbonCheckBox(this);

    checkBox->setWindowIcon(QIcon(":/icon/icon/folder.png"));
    checkBox->setText(tr("checkBox"));
    pannelWidgetTest->addSmallWidget(checkBox);
    pannelWidgetTest->addSeparator();

    QCalendarWidget* calendarWidget = new QCalendarWidget(this);
    calendarWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    calendarWidget->setObjectName(("calendarWidget"));
    pannelWidgetTest->addLargeWidget(calendarWidget);
    pannelWidgetTest->setExpanding();
    QAction* optAct = new QAction(this);

    pannelWidgetTest->addOptionAction(optAct);

    pannelWidgetTest->setVisible(true);
}

void MainWindow::createCategoryOther(SARibbonCategory* page)
{
    SARibbonMenu* menu = new SARibbonMenu(this);
    QAction* item      = menu->addAction(QIcon(":/icon/icon/folder.png"), ("menu item test"));

    item->setObjectName(("menu item test"));
    menu->addAction(QIcon(":/icon/icon/folder.png"), ("1"));
    menu->addAction(QIcon(":/icon/icon/folder.png"), ("2"));
    menu->addAction(QIcon(":/icon/icon/folder.png"), ("3"));
    menu->addAction(QIcon(":/icon/icon/folder.png"), ("4"));
    menu->addAction(QIcon(":/icon/icon/folder.png"), ("5"));
    menu->addAction(QIcon(":/icon/icon/folder.png"), ("6"));
    menu->addAction(QIcon(":/icon/icon/folder.png"), ("7"));
    menu->addAction(QIcon(":/icon/icon/folder.png"), ("8"));
    menu->addAction(QIcon(":/icon/icon/folder.png"), ("9"));

    SARibbonPannel* pannel = new SARibbonPannel(("pannel one"));

    page->addPannel(pannel);
    SARibbonButtonGroupWidget* btnGroup = new SARibbonButtonGroupWidget(pannel);

    btnGroup->addAction(new QAction(QIcon(":/icon/icon/figureIcon.png"), "", this));
    btnGroup->addAction(new QAction(QIcon(":/icon/icon/information.png"), "", this));
    btnGroup->addAction(new QAction(QIcon(":/icon/icon/chartDataManager.png"), "", this));
    btnGroup->addAction(new QAction(QIcon(":/icon/icon/inRangDataRemove.png"), "", this));
    pannel->addLargeWidget(btnGroup);

    SARibbonToolButton* btn;

    btn = pannel->addLargeAction(item);
    btn->setIcon(QIcon(":/icon/icon/folder.png"));
    btn->setText(("un format icon"));
    btn->setPopupMode(QToolButton::DelayedPopup);
    btn->setFixedHeight(78);
    btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btn->setMenu(menu);
    pannel->addLargeWidget(btn);

    btn = pannel->addLargeAction(item);
    btn->setIcon(QIcon(":/icon/icon/folder.png"));
    btn->setText(("change page test"));
    btn->setPopupMode(QToolButton::MenuButtonPopup);
    btn->setFixedHeight(78);
    btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btn->setMenu(menu);
    pannel->addLargeWidget(btn);

    btn = pannel->addLargeAction(item);
    btn->setIcon(QIcon(":/icon/icon/folder.png"));
    btn->setText(("LargeBtn"));
    btn->setPopupMode(QToolButton::InstantPopup);
    btn->setFixedHeight(78);
    btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btn->setMenu(menu);
    pannel->addLargeWidget(btn);

    SARibbonGallery* gallery    = pannel->addGallery();
    SARibbonGalleryGroup* group = gallery->addGalleryGroup();

    for (int i = 0; i < 100; ++i) {
        group->addItem(QString::number(i + 1), QIcon(":/icon/icon/folder.png"));
    }
    QAction* optAct = new QAction(this);

    optAct->setObjectName(("debug"));
    pannel->addOptionAction(optAct);
    pannel->setObjectName("debug");

    pannel->addSeparator();
    QAction* appBtn = new QAction(QIcon(":/icon/icon/icon2.png"), tr("no application button and very long word test"), this);

    appBtn->setObjectName(("no application button and very long word test"));
    appBtn->setCheckable(true);
    connect(appBtn, &QAction::toggled, this, [&](bool b) {
        if (b) {
            this->ribbonBar()->setApplicationButton(nullptr);
        } else {
            SARibbonApplicationButton* appBtn = new SARibbonApplicationButton();
            appBtn->setText(tr("File"));
            this->ribbonBar()->setApplicationButton(appBtn);
        }
    });
    SARibbonToolButton* b = pannel->addLargeAction(appBtn);

    b->setObjectName("ApplicationButtonTest");

    appBtn = new QAction(QIcon(":/icon/icon/icon2.png"), tr("Show Infomation Window"), this);
    appBtn->setObjectName(("Show Infomation Window"));
    pannel->addLargeAction(appBtn);

    QAction* useqss = new QAction(QIcon(":/icon/icon/icon2.png"), tr("use qss"), this);

    useqss->setObjectName(("use qss"));
    pannel->addLargeAction(useqss);
    connect(useqss, &QAction::triggered, this, [&]() {
        QFile f("ribbon.qss");
        if (!f.open(QIODevice::ReadWrite)) {
        }
        QString qss(f.readAll());
        this->ribbonBar()->setStyleSheet(qss);
    });

    QAction* useCustomize = new QAction(QIcon(":/icon/icon/506407.png"), tr("use customize from xml file"), this);

    useCustomize->setObjectName(("useCustomize"));
    pannel->addLargeAction(useCustomize);
    connect(useCustomize, &QAction::triggered, this, [&]() {
        //只能调用一次
        static bool has_call = false;
        if (!has_call) {
            has_call = sa_apply_customize_from_xml_file("customize.xml", this, m_actMgr);
        }
    });

    QAction* normalButton = new QAction(QIcon(":/icon/icon/506354.png"), ("正常模式"), this);

    normalButton->setObjectName(("normalButton"));
    pannel->addLargeAction(normalButton);
    connect(normalButton, &QAction::triggered, this, [&]() {
        //最大最小关闭按钮都有
        Qt::WindowFlags f = windowFlags();
        f |= (Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint);
        updateWindowFlag(f);
    });

    QAction* noneButton = new QAction(QIcon(":/icon/icon/506462.png"), ("无按钮模式"), this);

    noneButton->setObjectName(("noneButton"));
    pannel->addLargeAction(noneButton);
    connect(noneButton, &QAction::triggered, this, [&]() {
        //由于已经处于frameless状态，这个最大最小设置是无效的
        // setWindowFlags(windowFlags()&~Qt::WindowMaximizeButtonHint&~Qt::WindowMinimizeButtonHint);
        Qt::WindowFlags f = windowFlags();
        f &= ~Qt::WindowMinMaxButtonsHint & ~Qt::WindowCloseButtonHint;
        updateWindowFlag(f);
    });
    QAction* changename = new QAction(QIcon(":/icon/icon/529398.png"), ("改变pannel名字"), this);

    changename->setObjectName(("changename"));
    pannel->addLargeAction(changename);
    connect(changename, &QAction::triggered, this, [pannel]() { pannel->setPannelName(("改变pannel名字")); });

    pannel      = new SARibbonPannel(("ContextCategory"));
    QAction* a1 = new QAction(QIcon(":/icon/icon/529398.png"), ("Context Category 1"), this);

    a1->setCheckable(true);
    connect(a1, &QAction::triggered, this, [this](bool c) {
        this->ribbonBar()->setContextCategoryVisible(m_contextCategory, c);
    });
    QAction* a2 = new QAction(QIcon(":/icon/icon/529398.png"), ("Context Category 2"), this);

    a2->setCheckable(true);
    connect(a2, &QAction::triggered, this, [this](bool c) {
        this->ribbonBar()->setContextCategoryVisible(m_contextCategory2, c);
    });
    pannel->addLargeAction(a1);
    pannel->addLargeAction(a2);
    page->addPannel(pannel);
}

void MainWindow::createCategoryDelete(SARibbonCategory* page)
{
    SARibbonPannel* pannel1 = new SARibbonPannel(("pannel 1"));
    SARibbonPannel* pannel2 = new SARibbonPannel(("pannel 2"));
    QAction* act1           = new QAction(("删除Pannel2"), this);

    act1->setObjectName(("删除Pannel2"));
    act1->setIcon(QIcon(":/icon/icon/506356.png"));
    connect(act1, &QAction::triggered, this, [page, pannel2]() { page->removePannel(pannel2); });
    pannel1->addLargeAction(act1);

    QAction* act2 = new QAction(("删除本页"), this);

    act2->setObjectName(("删除本页"));
    act2->setIcon(QIcon(":/icon/icon/506357.png"));
    connect(act2, &QAction::triggered, this, [this, page, act2]() {
        this->ribbonBar()->removeCategory(page);
        page->hide();
        page->deleteLater();
        act2->setDisabled(true);
    });
    pannel2->addLargeAction(act2);

    page->addPannel(pannel1);
    page->addPannel(pannel2);
}
#include <QFontComboBox>
#include <QLabel>
/**
 * @brief 构建尺寸页
 * @param page
 */
void MainWindow::createCategorySize(SARibbonCategory* page)
{
    QAction* act = nullptr;

    SARibbonPannel* pannel = page->addPannel(("字体"));

    QLabel* labelFontSize = new QLabel(this);
    labelFontSize->setText(QStringLiteral(u"选择字体"));
    labelFontSize->setObjectName(QStringLiteral(u"labelFontSize"));
    act = pannel->addWidget(labelFontSize, SARibbonPannelItem::Small);
    act->setObjectName(labelFontSize->objectName());

    QFontComboBox* comWidget = new QFontComboBox(this);
    comWidget->setObjectName(QStringLiteral(u"fontComboBox"));
    act = pannel->addWidget(comWidget, SARibbonPannelItem::Small);
    act->setObjectName(comWidget->objectName());

    pannel->addSeparator();

    QAction* actLargerFontSize = new QAction(this);
    actLargerFontSize->setObjectName(QStringLiteral(u"actLargerFontSize"));
    actLargerFontSize->setIcon(QIcon(":/icon/icon/largerFont.svg"));
    actLargerFontSize->setText(QStringLiteral(u"Larger"));
    pannel->addLargeAction(actLargerFontSize);

    QAction* actSmallFontSize = new QAction(this);
    actSmallFontSize->setObjectName(QStringLiteral(u"actSmallFontSize"));
    actSmallFontSize->setIcon(QIcon(":/icon/icon/smallFontsvg"));
    actSmallFontSize->setText(QStringLiteral(u"Smaller"));
    pannel->addLargeAction(actSmallFontSize);
}

/**
 * @brief 创建上下文标签
 */
void MainWindow::createContextCategory1()
{
    //上下文标签
    SARibbonBar* ribbon = ribbonBar();
    m_contextCategory   = ribbon->addContextCategory(tr("context"), QColor(), 1);

    SARibbonCategory* contextCategoryPage1 = m_contextCategory->addCategoryPage(tr("Page1"));
    createContextCategoryPage1(contextCategoryPage1);

    SARibbonCategory* contextCategoryPage2 = m_contextCategory->addCategoryPage(tr("Page2"));
    createContextCategoryPage2(contextCategoryPage2);
}

void MainWindow::createContextCategory2()
{
    SARibbonBar* ribbon = ribbonBar();
    m_contextCategory2  = ribbon->addContextCategory(("context2"), QColor(), 2);
    m_contextCategory2->addCategoryPage(("context2 Page1"));
    m_contextCategory2->addCategoryPage(("context2 Page2"));
}

void MainWindow::createContextCategoryPage1(SARibbonCategory* page)
{
    SARibbonPannel* pannel = page->addPannel(("显示隐藏操作"));
    QAction* act           = new QAction(this);

    act->setCheckable(true);
    act->setIcon(QIcon(":/icon/icon/530150.png"));
    act->setText(("隐藏pannel"));
    pannel->addLargeAction(act);

    QAction* act2 = new QAction(this);

    act2->setDisabled(true);
    act2->setIcon(QIcon(":/icon/icon/529398.png"));
    act2->setText(("disable"));
    pannel->addLargeAction(act2);
    connect(act2, &QAction::triggered, this, [act2](bool b) { act2->setDisabled(true); });

    QAction* act21 = new QAction(this);

    act21->setIcon(QIcon(":/icon/icon/529398.png"));
    act21->setText(("解锁左边的按钮"));
    act21->setShortcut(QKeySequence(QLatin1String("Ctrl+E")));
    pannel->addLargeAction(act21);
    connect(act21, &QAction::triggered, this, [act2](bool b) {
        qDebug() << "act2->setEnabled(true);";
        act2->setEnabled(true);
        act2->setText(("Enabled"));
    });

    QAction* act3 = new QAction(this);

    act3->setCheckable(true);
    act3->setIcon(QIcon(":/icon/icon/530767.png"));
    act3->setText(("setText测试\r\nCtrl+D"));
    act3->setShortcut(QKeySequence(QLatin1String("Ctrl+D")));
    pannel->addLargeAction(act3);

    connect(act3, &QAction::toggled, this, [act3](bool b) {
        if (b) {
            act3->setText(("点击了"));
        } else {
            act3->setText(("setText测试"));
        }
    });
    //隐藏pannel
    QAction* act4 = new QAction(this);

    act4->setCheckable(true);
    act4->setIcon(QIcon(":/icon/icon/arror.png"));
    act4->setText(("显示旁边的pannel"));
    pannel->addLargeAction(act4);

    SARibbonPannel* pannel2 = page->addPannel(("用于隐藏显示的测试"));

    pannel2->addLargeAction(act3);

    connect(act4, &QAction::toggled, this, [act4, pannel2, this](bool b) {
        pannel2->setVisible(!b);
        if (b) {
            act4->setText(("隐藏旁边的pannel"));
        } else {
            act4->setText(("显示旁边的pannel"));
        }
        ribbonBar()->repaint();
    });

    SARibbonPannel* pannel3 = page->addPannel(("action隐藏显示的测试"));
    QAction* act31          = new QAction(this);

    act31->setCheckable(true);
    act31->setChecked(true);
    act31->setIcon(QIcon(":/icon/icon/arror.png"));
    act31->setText(("隐藏action2"));
    QAction* act32 = new QAction(this);

    act32->setIcon(QIcon(":/icon/icon/arror.png"));
    act32->setText(("action 2"));
    QAction* act33 = new QAction(this);

    act33->setIcon(QIcon(":/icon/icon/arror.png"));
    act33->setText(("action 3"));
    QAction* act34 = new QAction(this);

    act34->setIcon(QIcon(":/icon/icon/arror.png"));
    act34->setText(("action 4"));
    pannel3->addLargeAction(act31);
    pannel3->addSmallAction(act32);
    pannel3->addSmallAction(act33);
    pannel3->addSmallAction(act34);
    connect(act31, &QAction::triggered, this, [act31, act32](bool b) {
        if (b) {
            act32->setVisible(true);
            act31->setText(("隐藏action2"));
        } else {
            act32->setVisible(false);
            act31->setText(("显示action2"));
        }
    });
}

void MainWindow::createContextCategoryPage2(SARibbonCategory* page)
{
    SARibbonPannel* pannel1 = page->addPannel(("删除CategoryPage测试"));
    QAction* act11          = new QAction(this);

    act11->setIcon(QIcon(":/icon/icon/529398.png"));
    act11->setText(("删除本页"));
    pannel1->addLargeAction(act11);
    connect(act11, &QAction::triggered, this, [this, page]() {
        this->ribbonBar()->removeCategory(page);
        page->deleteLater();
    });
    SARibbonPannel* pannel2 = page->addPannel(("特殊布局"));

    pannel2->addAction(("Large"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Large);
    pannel2->addAction(("Small"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Small);
    pannel2->addAction(("Small"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Small);
    pannel2->addSeparator();
    pannel2->addAction(("Small"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Small);
    pannel2->addAction(("Small"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Small);
    pannel2->addAction(("Small"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Small);
    pannel2->addAction(("Small"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Small);
    pannel2->addSeparator();
    pannel2->addAction(("Large"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Large);
    pannel2->addAction(("Medium"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Medium);
    pannel2->addAction(("Medium"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Medium);
    pannel2->addAction(("Small"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Small);
    pannel2->addAction(("Medium"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Medium);
    pannel2->addAction(("Large"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Large);
    pannel2->addAction(("Medium"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Medium);
    pannel2->addAction(("Medium"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Medium);
    pannel2->addAction(("Large"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Large);
    pannel2->addSeparator();
    pannel2->addAction(("Medium"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Medium);
    pannel2->addAction(("Large"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Large);
    pannel2->addAction(("Medium"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Medium);
    pannel2->addAction(("Small"), QIcon(":/icon/icon/530767.png"), QToolButton::InstantPopup, SARibbonPannelItem::Small);
}

void MainWindow::addSomeOtherAction()
{
    //添加其他的action，这些action并不在ribbon管理范围，主要用于SARibbonCustomizeWidget自定义用
    QAction* acttext1 = new QAction(("纯文本action1"), this);
    QAction* acttext2 = new QAction(("纯文本action2"), this);
    QAction* acttext3 = new QAction(("纯文本action3"), this);
    QAction* acttext4 = new QAction(("纯文本action4"), this);
    QAction* acttext5 = new QAction(("纯文本action5"), this);

    QAction* actIcon1 = new QAction(QIcon(":/icon/icon/506353.png"), ("带图标action1"), this);
    QAction* actIcon2 = new QAction(QIcon(":/icon/icon/506354.png"), ("带图标action2"), this);
    QAction* actIcon3 = new QAction(QIcon(":/icon/icon/506355.png"), ("带图标action3"), this);
    QAction* actIcon4 = new QAction(QIcon(":/icon/icon/506356.png"), ("带图标action4"), this);

    m_actionTagText     = SARibbonActionsManager::UserDefineActionTag + 1;
    m_actionTagWithIcon = SARibbonActionsManager::UserDefineActionTag + 2;

    m_actMgr = new SARibbonActionsManager(this);  //申明过程已经自动注册所有action

    //以下注册特别的action
    m_actMgr->registeAction(acttext1, SARibbonActionsManager::CommonlyUsedActionTag);
    m_actMgr->registeAction(acttext3, SARibbonActionsManager::CommonlyUsedActionTag);
    m_actMgr->registeAction(acttext5, SARibbonActionsManager::CommonlyUsedActionTag);
    m_actMgr->registeAction(actIcon1, SARibbonActionsManager::CommonlyUsedActionTag);
    m_actMgr->registeAction(actIcon3, SARibbonActionsManager::CommonlyUsedActionTag);

    m_actMgr->registeAction(acttext1, m_actionTagText);
    m_actMgr->registeAction(acttext2, m_actionTagText);
    m_actMgr->registeAction(acttext3, m_actionTagText);
    m_actMgr->registeAction(acttext4, m_actionTagText);
    m_actMgr->registeAction(acttext5, m_actionTagText);

    m_actMgr->registeAction(actIcon1, m_actionTagWithIcon);
    m_actMgr->registeAction(actIcon2, m_actionTagWithIcon);
    m_actMgr->registeAction(actIcon3, m_actionTagWithIcon);
    m_actMgr->registeAction(actIcon4, m_actionTagWithIcon);

    m_actMgr->setTagName(SARibbonActionsManager::CommonlyUsedActionTag, ("常用"));
    m_actMgr->setTagName(m_actionTagText, ("无图标action"));
    m_actMgr->setTagName(m_actionTagWithIcon, ("有图标action"));
}

QAction* MainWindow::createAction(const QString& text, const QString& iconurl, const QString& objName)
{
    QAction* act = new QAction(this);
    act->setText(text);
    act->setIcon(QIcon(iconurl));
    act->setObjectName(objName);
    return act;
}

QAction* MainWindow::createAction(const QString& text, const QString& iconurl)
{
    QAction* act = new QAction(this);
    act->setText(text);
    act->setIcon(QIcon(iconurl));
    act->setObjectName(text);
    return act;
}

void MainWindow::onMenuButtonPopupCheckableTest(bool b)
{
    m_edit->append(QString("MenuButtonPopupCheckableTest : %1").arg(b));
}

void MainWindow::onInstantPopupCheckableTest(bool b)
{
    m_edit->append(QString("InstantPopupCheckableTest : %1").arg(b));
}

void MainWindow::onDelayedPopupCheckableTest(bool b)
{
    m_edit->append(QString("DelayedPopupCheckableTest : %1").arg(b));
}

void MainWindow::onMenuButtonPopupCheckabletriggered(bool b)
{
    m_edit->append(QString("MenuButtonPopupCheckabletriggered : %1").arg(b));
}

void MainWindow::onInstantPopupCheckabletriggered(bool b)
{
    m_edit->append(QString("InstantPopupCheckabletriggered : %1").arg(b));
}

void MainWindow::onDelayedPopupCheckabletriggered(bool b)
{
    m_edit->append(QString("DelayedPopupCheckabletriggered : %1").arg(b));
}
