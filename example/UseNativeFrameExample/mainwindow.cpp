#include "mainwindow.h"
#if !SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
#include "SAFramelessHelper.h"
#endif
#include "SARibbonApplicationButton.h"
#include "SARibbonBar.h"
#include "SARibbonButtonGroupWidget.h"
#include "SARibbonCategory.h"
#include "SARibbonCheckBox.h"
#include "SARibbonColorToolButton.h"
#include "SARibbonComboBox.h"
#include "SARibbonCustomizeDialog.h"
#include "SARibbonCustomizeWidget.h"
#include "SARibbonGallery.h"
#include "SARibbonLineEdit.h"
#include "SARibbonMenu.h"
#include "SARibbonPannel.h"
#include "SARibbonQuickAccessBar.h"
#include "SARibbonToolButton.h"
#include "colorWidgets/SAColorGridWidget.h"
#include "colorWidgets/SAColorPaletteGridWidget.h"
#include "SARibbonSystemButtonBar.h"
#include "SARibbonApplicationWidget.h"
#include <QAbstractButton>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QCalendarWidget>
#include <QDebug>
#include <QElapsedTimer>
#include <QFile>
#include <QFileDialog>
#include <QFontComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QStatusBar>
#include <QTextEdit>
#include <QTextStream>
#include <QXmlStreamWriter>
#include <QMessageBox>
#include <QShortcut>
#include <QLineEdit>
#include <QDialogButtonBox>
#define PRINT_COST_START()                                                                                             \
	QElapsedTimer __TMP_COST;                                                                                          \
	__TMP_COST.start();                                                                                                \
	int __TMP_LASTTIMES = 0

#define PRINT_COST(STR)                                                                                                \
	do {                                                                                                               \
		int ___TMP_INT = __TMP_COST.elapsed();                                                                         \
		qDebug() << STR << " cost " << ___TMP_INT - __TMP_LASTTIMES << " ms (" << ___TMP_INT << ")";                   \
		mTextedit->append(QString("%1 cost %2 ms(%3)").arg(STR).arg(___TMP_INT - __TMP_LASTTIMES).arg(___TMP_INT));    \
		__TMP_LASTTIMES = ___TMP_INT;                                                                                  \
	} while (0)

MainWindow::MainWindow(QWidget* par, SARibbonMainWindowStyles style) : SARibbonMainWindow(par, style)
{
    setWindowTitle(("ribbon use native frame test[*]"));
	setWindowModified(true);
	mTextedit = new QTextEdit(this);
	setCentralWidget(mTextedit);
	setStatusBar(new QStatusBar());

	SARibbonBar* ribbon = ribbonBar();
	//! 通过setContentsMargins设置ribbon四周的间距
	ribbon->setContentsMargins(5, 0, 5, 0);

	connect(ribbon, &SARibbonBar::actionTriggered, this, [ this ](QAction* action) {
		mTextedit->append(QString("action object name=%1 triggered").arg(action->objectName()));
	});

	//! cn:
	//! 这里示例的是如何设置和修改applicationButton，默认情况下SARibbonBar会创建一个SARibbonApplicationButton，
	//! SARibbonApplicationButton的父类是QToolButton,用户也可以创建自己的applicationButton，
	//! 通过SARibbonBar::setApplicationButton函数设置自己定义的按钮，如果传入一个空指针，将取消掉applicationButton
	//! en:
	//! Here is an example of how to set and modify the applicationButton. By default,
	//! SARibbonBar will create a SARibbonApplicationButton. The parent class of SARibbonApplicationButton is QToolButton.
	//! Users can also create their own applicationButton, and set their own buttons through the SARibbonBar::setApplicationButton function.
	//! If a null pointer is passed in, the applicationButton will be canceled
	createRibbonApplicationButton();

	//! cn:
	//! 添加主标签页,这里演示通过SARibbonBar::addCategoryPage函数添加一个标签页
	//! en:
	//! Add the main tab. Here we show how to add a tab through the SARibbonBar::addCategoryPage function
	SARibbonCategory* categoryMain = ribbon->addCategoryPage(tr("&Main"));
	//! cn: SARibbonBar的Category和Pannel，以及对应的Action都应该设置ObjectName，因为如果要自定义action，这些ObjectName是必不可少的
	//! en: The category , pannel and actions of SARibbonBar, should be set with Object Names, as these Object Names are essential for customizing actions
	categoryMain->setObjectName(("categoryMain"));
	createCategoryMain(categoryMain);

	//! cn:这里演示了另外一种添加标签页的方式，先创建SARibbonCategory指针，并通过SARibbonBar::addCategoryPage函数把SARibbonCategory指针传递给SARibbonBar
	//! en:Here is another way to add a tab: first create a SARibbonCategory pointer and pass it to SARibbonBar through the SARibbonBar::addCategoryPage function
	SARibbonCategory* categoryOther = new SARibbonCategory();
	categoryOther->setCategoryName(tr("Other"));
	categoryOther->setObjectName(("categoryOther"));
	createCategoryOther(categoryOther);
	ribbon->addCategoryPage(categoryOther);

    createContextCategory();

	//! cn:
	//! 创建RightButtonGroup,RightButtonGroup类似一个在右上角的工具栏，给用户放置一些快捷图标，例如关于、帮助等图标，
	//! RightButtonGroup在SARibbonBar创建时就会构建一个默认的SARibbonButtonGroupWidget，可以通过SARibbonBar::rightButtonGroup函数获取
	//! en:
	//! Create a RightButtonGroup, which is similar to a toolbar in the upper right corner,
	//! providing users with shortcut icons such as About, Help, etc. RightButtonGroup will build a default SARibbonButtonGroupWidget when creating SARibbonBar,
	//! which can be obtained through the SARibbonBar::rightButtonGroup function
	createRightButtonGroup();

	//! cn:
	//! 这里演示了如何在系统窗口最小化最大化关闭按钮旁边添加其他按钮
	createWindowButtonGroupBar();

	setMinimumWidth(500);
	setWindowIcon(QIcon(":/icon/icon/SA.svg"));

	connect(ribbon, &SARibbonBar::currentRibbonTabChanged, this, [ this ](int v) {
		mTextedit->append(QString("SARibbonBar::currentRibbonTabChanged(%1)").arg(v));
	});
	//! 全屏显示
	showMaximized();
}

/**
 * @brief 创建ApplicationButton
 */
void MainWindow::createRibbonApplicationButton()
{
	SARibbonBar* ribbon = ribbonBar();
	if (!ribbon) {
		return;
	}
	QAbstractButton* btn = ribbon->applicationButton();
	if (!btn) {
		//! cn: SARibbonBar默认就会创建一个SARibbonApplicationButton，因此，在正常情况下，这个位置不会进入
		//! en: SARibbonBar creates a SARibbonApplicationButton by default. Therefore, under normal circumstances, this location will not enter
		btn = new SARibbonApplicationButton(this);
		ribbon->setApplicationButton(btn);
	}

    btn->setText(("&File"));
    if (0) {
        SARibbonApplicationButton* appBtn = qobject_cast< SARibbonApplicationButton* >(btn);
        if (!appBtn) {
            return;
        }
        //! cn: SARibbonMenu和QMenu的操作是一样的
        //! en: The operations of SARibbonMenu and QMenu are the same
        if (!mMenuApplicationBtn) {
            mMenuApplicationBtn = new SARibbonMenu(this);
            mMenuApplicationBtn->addAction(createAction("appbtn-test1", ":/icon/icon/action.svg"));
            mMenuApplicationBtn->addAction(createAction("appbtn-test2", ":/icon/icon/action2.svg"));
            mMenuApplicationBtn->addAction(createAction("appbtn-test3", ":/icon/icon/action3.svg"));
            mMenuApplicationBtn->addAction(createAction("appbtn-test4", ":/icon/icon/action4.svg"));
        }

        appBtn->setMenu(mMenuApplicationBtn);
    } else {
        mAppWidget = new SARibbonApplicationWidget(this);
        mAppWidget->hide();
        connect(btn, &QAbstractButton::clicked, this, [ this ](bool c) {
            Q_UNUSED(c);
            this->mAppWidget->show();
        });
	}
}

/**
 * @brief 显示ContextCategory1
 * @param on
 */
void MainWindow::onShowContextCategory(bool on)
{
	if (mContextCategory == nullptr) {
        createContextCategory();
	}
	if (on) {
		this->ribbonBar()->showContextCategory(mContextCategory);
	} else {
		this->ribbonBar()->hideContextCategory(mContextCategory);
	}
}

void MainWindow::onStyleClicked(int id)
{

	SARibbonBar::RibbonStyles ribbonStyle = static_cast< SARibbonBar::RibbonStyles >(id);
	ribbonBar()->setRibbonStyle(ribbonStyle);
    mActionWordWrap->setChecked(ribbonBar()->isEnableWordWrap());
	switch (ribbonStyle) {
	case SARibbonBar::RibbonStyleLooseThreeRow:
		// cn:"LooseThreeRow"样式的文字显示是换行的，同时也会显示标题栏，你也可以通过SARibbonBar::setEnableWordWrap来控制按钮是否换行显示，
		// 可以通过SARibbonBar::setEnableShowPannelTitle控制标题栏是否显示
		mTextedit->append(
            tr("\nThe text display of the \"LooseThreeRow\" style is word wrap and also displays the title bar. "
               "You can also control whether the button is line breaking through SARibbonBar::setEnableWordWrap,"
               "and whether the title bar is displayed through SARibbonBar::setEnableShowPannelTitle"));
		mTextedit->append(tr("ribbonBar()->setRibbonStyle(SARibbonBar::RibbonStyleLooseThreeRow);"));
		break;
	case SARibbonBar::RibbonStyleLooseTwoRow:
		// cn:"LooseThreeRow"样式的文字显示是不换行的，同时也会显示标题栏，你也可以通过SARibbonBar::setEnableWordWrap来控制按钮是否换行显示，
		// 可以通过SARibbonBar::setEnableShowPannelTitle控制标题栏是否显示
		mTextedit->append(
            tr("\nThe text display of the \"LooseTwoRow\" style is not word wrap and also displays the title bar. "
               "You can also control whether the button is line breaking through SARibbonBar::setEnableWordWrap,"
               "and whether the title bar is displayed through SARibbonBar::setEnableShowPannelTitle"));
		mTextedit->append(tr("ribbonBar()->setRibbonStyle(SARibbonBar::RibbonStyleLooseTwoRow);"));
		break;
	case SARibbonBar::RibbonStyleCompactThreeRow:
		// cn:"CompactThreeRow"样式的文字显示是换行的，不会显示标题栏，你也可以通过SARibbonBar::setEnableWordWrap来控制按钮是否换行显示，
		// 可以通过SARibbonBar::setEnableShowPannelTitle控制标题栏是否显示
		mTextedit->append(
            tr("\nThe text display of the \"LooseThreeRow\" style is word wrap and not displays the title bar. "
               "You can also control whether the button is line breaking through SARibbonBar::setEnableWordWrap,"
               "and whether the title bar is displayed through SARibbonBar::setEnableShowPannelTitle"));
		mTextedit->append(tr("ribbonBar()->setRibbonStyle(SARibbonBar::RibbonStyleCompactThreeRow);"));
		break;
	case SARibbonBar::RibbonStyleCompactTwoRow:
		// cn:"CompactTwoRow"样式的文字显示是不换行的，不会显示标题栏，你也可以通过SARibbonBar::setEnableWordWrap来控制按钮是否换行显示，
		// 可以通过SARibbonBar::setEnableShowPannelTitle控制标题栏是否显示
		mTextedit->append(
            tr("\nThe text display of the \"CompactTwoRow\" style is not word wrap and not displays the title bar. "
               "You can also control whether the button is line breaking through SARibbonBar::setEnableWordWrap,"
               "and whether the title bar is displayed through SARibbonBar::setEnableShowPannelTitle"));
		mTextedit->append(tr("ribbonBar()->setRibbonStyle(SARibbonBar::RibbonStyleCompactTwoRow);"));
		break;
	default:
		break;
	}
}

void MainWindow::onActionHelpTriggered()
{
	QMessageBox::information(this,
                             tr("infomation"),
                             tr("\n ==============="
                                "\n SARibbonBar version:%1"
                                "\n Author:czy"
                                "\n Email:czy.t@163.com"
                                "\n ===============")
                                 .arg(SARibbonBar::versionString()));
}

void MainWindow::onActionWordWrapTriggered(bool b)
{
	ribbonBar()->setEnableWordWrap(b);
	mTextedit->append(tr("By using the SARibbonBar::setEnableWordWrap function, "
                         "you can set whether text breaks or not.\n"
                         "By default, the two line mode will not wrap, the three line mode will wrap.\n"
                         "You can force the two line mode to wrap, or the three line mode to not wrap"));
	// cn:通过SARibbonBar::setEnableWordWrap函数可以设置文字是否换行。\n
	// 默认情况下，两行模式都不会换行，三行模式下都会换行。\n
	// 可以强制设置两行模式也换行，或者三行模式不换行
}

/**
 * @brief 样式改变
 * @param index
 */
void MainWindow::onRibbonThemeComboBoxCurrentIndexChanged(int index)
{
	SARibbonTheme t = static_cast< SARibbonTheme >(mComboboxRibbonTheme->itemData(index).toInt());
	setRibbonTheme(t);
}

/**
 * @brief 切换所有action是否可见
 * @param on
 */
void MainWindow::onActionVisibleAllTriggered(bool on)
{
    const QList< QAction* > acts = ribbonBar()->allActions();
    for (QAction* a : acts) {
		if (a != mActionVisibleAll) {
			a->setVisible(on);
		}
	}
	ribbonBar()->updateRibbonGeometry();
    ribbonBar()->update();
}

/**
   @brief 居中对齐checkbox的槽
   @param checked
 */
void MainWindow::onCheckBoxAlignmentCenterClicked(bool checked)
{
	if (checked) {
		ribbonBar()->setRibbonAlignment(SARibbonAlignment::AlignCenter);
	} else {
		ribbonBar()->setRibbonAlignment(SARibbonAlignment::AlignLeft);
	}
}

void MainWindow::closeEvent(QCloseEvent* e)
{
	auto res = QMessageBox::question(this, tr("question"), tr("Confirm whether to exit"));
	if (res == QMessageBox::Yes) {
		e->accept();
	} else {
		e->ignore();
	}
}

void MainWindow::createCategoryMain(SARibbonCategory* page)
{
	//! 1
	//! pannel 1 start
	//!

	// 使用addPannel函数来创建SARibbonPannel，效果和new SARibbonPannel再addPannel一样
    SARibbonPannel* pannelStyle = page->addPannel(tr("ribbon style"));

    QAction* actSave = createAction(tr("Save"), QStringLiteral(":/icon/icon/save.svg"));
	connect(actSave, &QAction::triggered, this, [ this ](bool b) {
		Q_UNUSED(b);
		this->mTextedit->append("actSaveion clicked");
		this->setWindowModified(false);
	});
    // 快捷键设置示范，如果你想你的快捷键能在整个MainWindow生命周期都显示，你应该把这个action也添加到MainWindow中
    actSave->setShortcut(QKeySequence(QLatin1String("Ctrl+S")));
    addAction(actSave);

	pannelStyle->addLargeAction(actSave);

	QAction* actHideRibbon = createAction(tr("hide ribbon"), ":/icon/icon/hideRibbon.svg", "actHideRibbon");
	actHideRibbon->setCheckable(true);
	pannelStyle->addSmallAction(actHideRibbon);
	connect(actHideRibbon, &QAction::triggered, this, [ this ](bool b) { this->ribbonBar()->setMinimumMode(b); });
	connect(ribbonBar(), &SARibbonBar::ribbonModeChanged, this, [ actHideRibbon ](SARibbonBar::RibbonMode nowNode) {
		actHideRibbon->setChecked(nowNode == SARibbonBar::MinimumRibbonMode);
	});

    QAction* actShowHideButton = createAction(tr("show \nhide button"), ":/icon/icon/showHideButton.svg", "show hide button");
	actShowHideButton->setCheckable(true);
	pannelStyle->addSmallAction(actShowHideButton);  // wrod wrap was not effect in small button
	connect(actShowHideButton, &QAction::triggered, this, [ this ](bool b) {
		this->ribbonBar()->showMinimumModeButton(b);  // 显示ribbon最小化按钮
	});
	actShowHideButton->trigger();

	mActionWordWrap = createAction(tr("word wrap"), ":/icon/icon/wordwrap.svg");
    mActionWordWrap->setCheckable(true);
    mActionWordWrap->setChecked(ribbonBar()->isEnableWordWrap());
	pannelStyle->addSmallAction(mActionWordWrap);
	connect(mActionWordWrap, &QAction::triggered, this, &MainWindow::onActionWordWrapTriggered);

	QButtonGroup* g = new QButtonGroup(page);

	QRadioButton* r = new QRadioButton();
	r->setText(tr("use office style"));
	r->setObjectName(("use office style"));
	r->setWindowTitle(r->text());
	r->setChecked(true);
	pannelStyle->addSmallWidget(r);
	g->addButton(r, SARibbonBar::RibbonStyleLooseThreeRow);

	r = new QRadioButton();
	r->setObjectName(("use wps style"));
	r->setText(tr("use wps style"));
	r->setWindowTitle(r->text());
	r->setChecked(false);
	pannelStyle->addSmallWidget(r);
	g->addButton(r, SARibbonBar::RibbonStyleCompactThreeRow);

	r = new QRadioButton();
	r->setObjectName(("use office 2row style"));
	r->setText(tr("use office 2 row style"));
	r->setWindowTitle(r->text());
	r->setChecked(false);
	pannelStyle->addSmallWidget(r);
	g->addButton(r, SARibbonBar::RibbonStyleLooseTwoRow);

	r = new QRadioButton();
	r->setObjectName(("use wps 2row style"));
	r->setText(tr("use wps 2row style"));
	r->setWindowTitle(r->text());
	r->setChecked(false);
	pannelStyle->addSmallWidget(r);
	g->addButton(r, SARibbonBar::RibbonStyleCompactTwoRow);

//    connect(g, QOverload<int>::of(&QButtonGroup::buttonClicked), this, &MainWindow::onStyleClicked);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	connect(g, static_cast< void (QButtonGroup::*)(int) >(&QButtonGroup::idClicked), this, &MainWindow::onStyleClicked);
#else
	connect(g, static_cast< void (QButtonGroup::*)(int) >(&QButtonGroup::buttonClicked), this, &MainWindow::onStyleClicked);
#endif

	mComboboxRibbonTheme = new SARibbonComboBox();
	mComboboxRibbonTheme->setWindowTitle(tr("RibbonTheme"));
	mComboboxRibbonTheme->setObjectName("RibbonTheme");
	mComboboxRibbonTheme->addItem("Theme Win7", static_cast< int >(SARibbonTheme::RibbonThemeWindows7));
	mComboboxRibbonTheme->addItem("Theme Office2013", static_cast< int >(SARibbonTheme::RibbonThemeOffice2013));
	mComboboxRibbonTheme->addItem("Theme Office2016 Blue", static_cast< int >(SARibbonTheme::RibbonThemeOffice2016Blue));
	mComboboxRibbonTheme->addItem("Theme Office2021 Blue", static_cast< int >(SARibbonTheme::RibbonThemeOffice2021Blue));
	mComboboxRibbonTheme->addItem("Theme Dark", static_cast< int >(SARibbonTheme::RibbonThemeDark));
	mComboboxRibbonTheme->addItem("Theme Dark2", static_cast< int >(SARibbonTheme::RibbonThemeDark2));
	mComboboxRibbonTheme->setCurrentIndex(mComboboxRibbonTheme->findData(static_cast< int >(ribbonTheme())));
	connect(mComboboxRibbonTheme,
            QOverload< int >::of(&SARibbonComboBox::currentIndexChanged),
            this,
            &MainWindow::onRibbonThemeComboBoxCurrentIndexChanged);
	pannelStyle->addSmallWidget(mComboboxRibbonTheme);

	SARibbonCheckBox* checkBox = new SARibbonCheckBox(this);

	checkBox->setText(tr("Alignment Center"));
	checkBox->setObjectName("checkBoxAlignmentCenter");
	checkBox->setWindowTitle(checkBox->text());
	connect(checkBox, &SARibbonCheckBox::clicked, this, &MainWindow::onCheckBoxAlignmentCenterClicked);
	pannelStyle->addSmallWidget(checkBox);

	SARibbonPannel* pannelToolButtonStyle = page->addPannel(("sa ribbon toolbutton style"));

	SARibbonMenu* menu = new SARibbonMenu(this);
	QAction* a         = nullptr;
	{
		QIcon itemicon = QIcon(":/icon/icon/item.svg");
		for (int i = 0; i < 5; ++i) {
			a = menu->addAction(itemicon, tr("item %1").arg(i + 1));
            a->setObjectName(QString("menuItem %1").arg(i + 1));
		}
	}

	QAction* act = createAction(tr("test 1"), ":/icon/icon/test1.svg");
	QVariant temp("Test");
	act->setData(temp);
	act->setMenu(menu);
	act->setToolTip(tr("use QToolButton::MenuButtonPopup mode"));
	pannelToolButtonStyle->addSmallAction(act, QToolButton::MenuButtonPopup);

	act = createAction(tr("test 2"), ":/icon/icon/test2.svg");
	act->setMenu(menu);
	act->setToolTip(tr("use QToolButton::InstantPopup mode"));
	pannelToolButtonStyle->addSmallAction(act, QToolButton::InstantPopup);

	pannelToolButtonStyle->addSeparator();

	act = createAction(tr("Delayed\nPopup"), ":/icon/icon/folder-cog.svg");
	act->setMenu(menu);
	pannelToolButtonStyle->addLargeAction(act, QToolButton::DelayedPopup);

	connect(act, &QAction::triggered, this, &MainWindow::onDelayedPopupCheckabletriggered);

	act = createAction(tr("Menu Button Popup"), ":/icon/icon/folder-star.svg");
	act->setMenu(menu);
	pannelToolButtonStyle->addLargeAction(act, QToolButton::MenuButtonPopup);
	connect(act, &QAction::triggered, this, &MainWindow::onMenuButtonPopupCheckabletriggered);

	act = createAction(tr("Instant Popup"), ":/icon/icon/folder-stats.svg");
	act->setMenu(menu);
	pannelToolButtonStyle->addLargeAction(act, QToolButton::InstantPopup);
	connect(act, &QAction::triggered, this, &MainWindow::onInstantPopupCheckabletriggered);

	act = createAction(tr("Delayed Popup checkable"), ":/icon/icon/folder-table.svg");
	act->setCheckable(true);
	act->setMenu(menu);
	pannelToolButtonStyle->addLargeAction(act, QToolButton::DelayedPopup);
	connect(act, &QAction::triggered, this, &MainWindow::onDelayedPopupCheckableTest);

	act = createAction(tr("Menu Button Popup checkable"), ":/icon/icon/folder-checkmark.svg");
	act->setCheckable(true);
	act->setMenu(menu);
	pannelToolButtonStyle->addLargeAction(act, QToolButton::MenuButtonPopup);
	connect(act, &QAction::triggered, this, &MainWindow::onMenuButtonPopupCheckableTest);

	act = createAction(tr("disable action"), ":/icon/icon/disable.svg");
	act->setCheckable(true);
	act->setMenu(menu);
	act->setEnabled(false);
	pannelToolButtonStyle->addLargeAction(act);

	QAction* optAct = new QAction(this);
	connect(optAct, &QAction::triggered, this, [ this ](bool on) {
		Q_UNUSED(on);
		QMessageBox::information(this, tr("Option Action Triggered"), tr("Option Action Triggered"));
	});
	pannelToolButtonStyle->setOptionAction(optAct);
}

void MainWindow::createCategoryOther(SARibbonCategory* page)
{
    SARibbonMenu* menu = new SARibbonMenu(this);
    QAction* a         = nullptr;
    {
        QIcon itemicon = QIcon(":/icon/icon/item.svg");
        for (int i = 0; i < 5; ++i) {
            a = menu->addAction(itemicon, tr("item %1").arg(i + 1));
            a->setObjectName(QString("menu2Item %1").arg(i + 1));
        }
    }
    //! 2
    //! pannel 2 start
    //!
    SARibbonPannel* pannel2 = page->addPannel(("pannel 2"));

    QAction* actShowContext = createAction(tr("show Context"), ":/icon/icon/showContext.svg");
    actShowContext->setCheckable(true);
    pannel2->addLargeAction(actShowContext);
    connect(actShowContext, &QAction::triggered, this, &MainWindow::onShowContextCategory);

    QAction* actDeleteContext = createAction(tr("delete Context"), ":/icon/icon/deleteContext.svg");
    pannel2->addLargeAction(actDeleteContext);
    connect(actDeleteContext, &QAction::triggered, this, [ this ](bool on) {
        Q_UNUSED(on);
        if (this->mContextCategory) {
            this->ribbonBar()->destroyContextCategory(this->mContextCategory);
            this->mContextCategory = nullptr;
        }
    });

    QAction* act = createAction(tr("Word\nWrap"), ":/icon/icon/setText.svg");
    pannel2->addLargeAction(act);
    connect(act, &QAction::triggered, this, [ this ](bool on) {
        Q_UNUSED(on);
        this->mTextedit->append(tr("Text can be manually wrapped(use \\n), and will appear as 1 line in the case of "
                                   "SARibbonBar::setEnableWordWrap (false)"));  // cn:文本中手动换行
    });

    act = createAction(tr("Word \nWrap"), ":/icon/icon/setText.svg");
    act->setMenu(menu);
    pannel2->addLargeAction(act);
    connect(act, &QAction::triggered, this, [ this ](bool on) {
        Q_UNUSED(on);
        this->mTextedit->append(tr("Text can be manually wrapped(use \\n), and will appear as 1 line in the case of "
                                   "SARibbonBar::setEnableWordWrap (false)"));  // cn:文本中手动换行
    });
    //! 3
    //! pannel 3 start -> widget test
    //!

    SARibbonPannel* pannelWidgetTest = page->addPannel(tr("widget test"));
    pannelWidgetTest->setObjectName(QStringLiteral(u"pannelWidgetTest"));

    SARibbonComboBox* com = new SARibbonComboBox(this);
    com->setObjectName("SARibbonComboBox test");
    com->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    com->setWindowTitle(tr("SARibbonComboBox test"));
    for (int i = 0; i < 40; ++i) {
        com->addItem(QString("SARibbonComboBox test%1").arg(i + 1));
    }
    com->setEditable(true);
    pannelWidgetTest->addSmallWidget(com);

    com = new SARibbonComboBox(this);
    com->setObjectName("ComboBox Editable");
    com->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    com->setWindowTitle("ComboBox Editable");
    for (int i = 0; i < 40; ++i) {
        com->addItem(QString("item %1").arg(i + 1));
    }
    pannelWidgetTest->addSmallWidget(com);

    SARibbonLineEdit* lineEdit = new SARibbonLineEdit(this);

    lineEdit->setObjectName("Line Edit");
    lineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    lineEdit->setWindowTitle("Line Edit");
    lineEdit->setText("SARibbonLineEdit");
    pannelWidgetTest->addSmallWidget(lineEdit);
    QWidget* w = lineEdit->parentWidget();

    while (w) {
        qDebug() << w->metaObject()->className();
        w = w->parentWidget();
    }

    pannelWidgetTest->addSeparator();

    QCalendarWidget* calendarWidget = new QCalendarWidget(this);
    calendarWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    calendarWidget->setObjectName(("calendarWidget"));
    calendarWidget->setWindowTitle("calendarWidget");
    pannelWidgetTest->addLargeWidget(calendarWidget);
    act = new QAction(this);
    connect(act, &QAction::triggered, this, [ this ](bool on) {
        Q_UNUSED(on);
        QMessageBox::information(this, tr("Option Action Triggered"), tr("Option Action Triggered"));
    });
    pannelWidgetTest->setOptionAction(act);

    pannelWidgetTest->setVisible(true);
}

void MainWindow::createContextCategory()
{
	SARibbonBar* ribbon = ribbonBar();
    mContextCategory    = ribbon->addContextCategory(("context2"), QColor(), 2);
    mContextCategory->addCategoryPage(("context2 Page1"));
    mContextCategory->addCategoryPage(("context2 Page2"));
}

/**
 * @brief 创建RightButtonGroup
 *
 * RightButtonGroup实在ribbonbar右边的工具栏，可以放置一些快捷图标
 * @param rightBar
 */
void MainWindow::createRightButtonGroup()
{
	SARibbonBar* ribbon = ribbonBar();
	if (!ribbon) {
		return;
	}
	SARibbonButtonGroupWidget* rightBar = ribbon->rightButtonGroup();
	QAction* actionHelp                 = createAction(tr("help"), ":/icon/icon/help.svg");
	mActionVisibleAll                   = createAction(tr("Visible"), ":/icon/icon/visible-true.svg");
	mActionVisibleAll->setCheckable(true);
	mActionVisibleAll->setChecked(true);
	connect(actionHelp, &QAction::triggered, this, &MainWindow::onActionHelpTriggered);
	connect(mActionVisibleAll, &QAction::triggered, this, &MainWindow::onActionVisibleAllTriggered);
	rightBar->addAction(actionHelp);
	rightBar->addAction(mActionVisibleAll);
}

/**
 * @brief 创建右上角系统最大、最小化按钮栏的图标工具
 */
void MainWindow::createWindowButtonGroupBar()
{
	SARibbonSystemButtonBar* wbar = windowButtonBar();
	if (!wbar) {
		return;
	}
	QAction* a = wbar->addAction(tr("Login"), QIcon(), Qt::ToolButtonTextOnly);
	connect(a, &QAction::triggered, this, [ this ]() { this->mTextedit->append("Login triggered"); });
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
	mTextedit->append(QString("MenuButtonPopupCheckableTest : %1").arg(b));
}

void MainWindow::onDelayedPopupCheckableTest(bool b)
{
	mTextedit->append(QString("DelayedPopupCheckableTest : %1").arg(b));
}

void MainWindow::onMenuButtonPopupCheckabletriggered(bool b)
{
	mTextedit->append(QString("MenuButtonPopupCheckabletriggered : %1").arg(b));
}

void MainWindow::onInstantPopupCheckabletriggered(bool b)
{
	mTextedit->append(QString("InstantPopupCheckabletriggered : %1").arg(b));
}

void MainWindow::onDelayedPopupCheckabletriggered(bool b)
{
	Q_UNUSED(b);
	mTextedit->append(tr("The SARibbonToolButton::setPopupMode(QToolButton::DelayedPopup) method "
                         "can be used to set the menu pop-up method to delayed pop-up. "
                         "This also demonstrates manually setting text wrapping"));
	// cn:使用SARibbonToolButton::setPopupMode(QToolButton::DelayedPopup)方法可以设置菜单弹出方式为延迟弹出，这里也演示了手动设置文本的换行
}
