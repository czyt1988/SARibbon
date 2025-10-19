#include "mainwindow.h"
#if !SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
#include "SAFramelessHelper.h"
#endif
#include "SARibbonApplicationButton.h"
#include "SARibbonBar.h"
#include "SARibbonButtonGroupWidget.h"
#include "SARibbonCategory.h"
#include <QCheckBox>
#include "SARibbonColorToolButton.h"
#include <QComboBox>
#include "SARibbonCustomizeDialog.h"
#include "SARibbonCustomizeWidget.h"
#include "SARibbonGallery.h"

#include "SARibbonMenu.h"
#include "SARibbonPanel.h"
#include "SARibbonQuickAccessBar.h"
#include "SARibbonToolButton.h"
#include "SARibbonCtrlContainer.h"
#include "colorWidgets/SAColorGridWidget.h"
#include "colorWidgets/SAColorPaletteGridWidget.h"
#include "SARibbonSystemButtonBar.h"
#include "SARibbonApplicationWidget.h"
#include "ApplicationWidget.h"
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

/**
 * @brief 构造函数，初始化主窗口和Ribbon界面
 * @param par 父窗口指针
 *
 * cn:构造函数中初始化了主窗口的基本组件，包括中央文本编辑区、状态栏和Ribbon工具栏
 * en:In the constructor, basic components of the main window are initialized, including the central text editor, status bar, and Ribbon toolbar
 */
MainWindow::MainWindow(QWidget* par) : SARibbonMainWindow(par)
{

    PRINT_COST_START();
    setWindowTitle(("ribbon mainwindow test[*]"));
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
    //! cn: SARibbonBar的Category和Panel，以及对应的Action都应该设置ObjectName，因为如果要自定义action，这些ObjectName是必不可少的
    //! en: The category , panel and actions of SARibbonBar, should be set with Object Names, as these Object Names are essential for customizing actions
    categoryMain->setObjectName(("categoryMain"));
    createCategoryMain(categoryMain);
    PRINT_COST("new main page");

    //! cn:这里演示了另外一种添加标签页的方式，先创建SARibbonCategory指针，并通过SARibbonBar::addCategoryPage函数把SARibbonCategory指针传递给SARibbonBar
    //! en:Here is another way to add a tab: first create a SARibbonCategory pointer and pass it to SARibbonBar through the SARibbonBar::addCategoryPage function
    SARibbonCategory* categoryOther = new SARibbonCategory();
    categoryOther->setCategoryName(tr("Other"));
    categoryOther->setObjectName(("categoryOther"));
    createCategoryOther(categoryOther);
    ribbon->addCategoryPage(categoryOther);
    PRINT_COST("add other page");

    // 添加删除标签页
    SARibbonCategory* categoryDelete = new SARibbonCategory();
    categoryDelete->setCategoryName(("Delete"));
    categoryDelete->setObjectName(("categoryDelete"));
    ribbon->addCategoryPage(categoryDelete);
    createCategoryDelete(categoryDelete);
    PRINT_COST("add category delete page");

    // 添加尺寸标签页
    SARibbonCategory* categorySize = new SARibbonCategory();
    categorySize->setCategoryName(("Size(example long category)"));
    categorySize->setObjectName(("categorySize"));
    ribbon->addCategoryPage(categorySize);
    createCategorySize(categorySize);
    PRINT_COST("add category size page");

    // 添加颜色标签页
    SARibbonCategory* categoryColor = new SARibbonCategory();
    categoryColor->setCategoryName(("Color"));
    categoryColor->setObjectName(("categoryColor"));
    ribbon->addCategoryPage(categoryColor);
    createCategoryColor(categoryColor);
    PRINT_COST("add category color page");

    createContextCategory1();
    PRINT_COST("add context1 category page");

    createContextCategory2();
    PRINT_COST("add context2 category page");

    createQuickAccessBar();
    PRINT_COST("add quick access bar");

    //! cn:
    //! 创建RightButtonGroup,RightButtonGroup类似一个在右上角的工具栏，给用户放置一些快捷图标，例如关于、帮助等图标，
    //! RightButtonGroup在SARibbonBar创建时就会构建一个默认的SARibbonButtonGroupWidget，可以通过SARibbonBar::rightButtonGroup函数获取
    //! en:
    //! Create a RightButtonGroup, which is similar to a toolbar in the upper right corner,
    //! providing users with shortcut icons such as About, Help, etc. RightButtonGroup will build a default SARibbonButtonGroupWidget when creating SARibbonBar,
    //! which can be obtained through the SARibbonBar::rightButtonGroup function
    createRightButtonGroup();
    PRINT_COST("add right bar");

    //! cn:
    //! 这里演示了如何在系统窗口最小化最大化关闭按钮旁边添加其他按钮
    createWindowButtonGroupBar();

    //! cn:
    //! actionManager可以管理所有的action，并给SARibbon的自定义窗口使用,
    //! actionManager必须在ribbon的action都创建完成后创建，如果在之前就创建好，后加入ribbon的action需要手动管理到actionManager里，
    //! actionManager也可以管理不在ribbonBar里的action
    //! en:
    //! The ActionManager can manage all actions and be used for SARibbon's custom window.
    //! The ActionManager must be created after all the actions in the ribbon are created.
    //! If the actions are created before, the actions added to the ribbon need to be manually managed in the ActionManager.
    //! The ActionManager can also manage actions not in the ribbon bar
    createActionsManager();

    setMinimumWidth(500);
    setWindowIcon(QIcon(":/icon/icon/SA.svg"));

    connect(ribbon, &SARibbonBar::currentRibbonTabChanged, this, [ this ](int v) {
        mTextedit->append(QString("SARibbonBar::currentRibbonTabChanged(%1)").arg(v));
    });
    mChangeTitleBkColorTimer.setInterval(5000);
    mChangeTitleBkColorTimer.start();
    connect(&mChangeTitleBkColorTimer, &QTimer::timeout, this, &MainWindow::onChangedTitleTimeout);
    //! 全屏显示
    showMaximized();
}

/**
 * @brief 创建ApplicationButton
 *
 * cn:ApplicationButton是Ribbon界面左上角的按钮，通常用于显示文件相关操作，如新建、打开、保存等
 * en:ApplicationButton is the button in the upper left corner of the Ribbon interface, usually used to display
 * file-related operations such as New, Open, Save, etc.
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
#ifdef USE_APPLICATION_NORMAL_MENU
    //! cn: SARibbonMenu和QMenu的操作是一样的
    //! en: The operations of SARibbonMenu and QMenu are the same
    if (!mMenuApplicationBtn) {
        mMenuApplicationBtn = new SARibbonMenu(this);
        mMenuApplicationBtn->addAction(createAction("test1", ":/icon/icon/action.svg"));
        mMenuApplicationBtn->addAction(createAction("test2", ":/icon/icon/action2.svg"));
        mMenuApplicationBtn->addAction(createAction("test3", ":/icon/icon/action3.svg"));
        mMenuApplicationBtn->addAction(createAction("test4", ":/icon/icon/action4.svg"));
    }
    SARibbonApplicationButton* appBtn = qobject_cast< SARibbonApplicationButton* >(btn);
    if (!appBtn) {
        return;
    }
    appBtn->setMenu(mMenuApplicationBtn);
#else
    // 这里演示用SARibbonApplicationWidget实现一个类似office的弹出式窗口
    mAppWidget = new ApplicationWidget(this);
    mAppWidget->hide();
    connect(btn, &QAbstractButton::clicked, this, [ this ](bool c) {
        Q_UNUSED(c);
        this->mAppWidget->show();
    });
#endif
}

/**
 * @brief 显示ContextCategory1
 * @param on 是否显示
 *
 * cn:ContextCategory是一种特殊的标签页，通常用于特定上下文环境，比如当选择某个对象时才显示相关操作
 * en:ContextCategory is a special tab that is usually displayed for specific context environments, such as showing
 * related operations when an object is selected
 */
void MainWindow::onShowContextCategory(bool on)
{
    if (mContextCategory == nullptr) {
        createContextCategory1();
    }
    if (on) {
        this->ribbonBar()->showContextCategory(mContextCategory);
    } else {
        this->ribbonBar()->hideContextCategory(mContextCategory);
    }
}

/**
 * @brief 切换Ribbon样式
 * @param id 样式ID
 *
 * cn:Ribbon支持多种样式，包括宽松/紧凑和两行/三行模式的组合，用户可以根据需要切换
 * en:Ribbon supports multiple styles, including combinations of loose/compact and two-row/three-row modes, which users can switch as needed
 */
void MainWindow::onStyleClicked(int id)
{

    SARibbonBar::RibbonStyles ribbonStyle = static_cast< SARibbonBar::RibbonStyles >(id);
    ribbonBar()->setRibbonStyle(ribbonStyle);
    mActionWordWrap->setChecked(ribbonBar()->isEnableWordWrap());
    switch (ribbonStyle) {
    case SARibbonBar::RibbonStyleLooseThreeRow:
        // cn:"LooseThreeRow"样式的文字显示是换行的，同时也会显示标题栏，你也可以通过SARibbonBar::setEnableWordWrap来控制按钮是否换行显示，
        // 可以通过SARibbonBar::setEnableShowPanelTitle控制标题栏是否显示
        mTextedit->append(
            tr("\nThe text display of the \"LooseThreeRow\" style is word wrap and also displays the title bar. "
               "You can also control whether the button is line breaking through SARibbonBar::setEnableWordWrap,"
               "and whether the title bar is displayed through SARibbonBar::setEnableShowPanelTitle"));
        mTextedit->append(tr("ribbonBar()->setRibbonStyle(SARibbonBar::RibbonStyleLooseThreeRow);"));
        break;
    case SARibbonBar::RibbonStyleLooseTwoRow:
        // cn:"LooseThreeRow"样式的文字显示是不换行的，同时也会显示标题栏，你也可以通过SARibbonBar::setEnableWordWrap来控制按钮是否换行显示，
        // 可以通过SARibbonBar::setEnableShowPanelTitle控制标题栏是否显示
        mTextedit->append(
            tr("\nThe text display of the \"LooseTwoRow\" style is not word wrap and also displays the title bar. "
               "You can also control whether the button is line breaking through SARibbonBar::setEnableWordWrap,"
               "and whether the title bar is displayed through SARibbonBar::setEnableShowPanelTitle"));
        mTextedit->append(tr("ribbonBar()->setRibbonStyle(SARibbonBar::RibbonStyleLooseTwoRow);"));
        break;
    case SARibbonBar::RibbonStyleCompactThreeRow:
        // cn:"CompactThreeRow"样式的文字显示是换行的，不会显示标题栏，你也可以通过SARibbonBar::setEnableWordWrap来控制按钮是否换行显示，
        // 可以通过SARibbonBar::setEnableShowPanelTitle控制标题栏是否显示
        mTextedit->append(
            tr("\nThe text display of the \"LooseThreeRow\" style is word wrap and not displays the title bar. "
               "You can also control whether the button is line breaking through SARibbonBar::setEnableWordWrap,"
               "and whether the title bar is displayed through SARibbonBar::setEnableShowPanelTitle"));
        mTextedit->append(tr("ribbonBar()->setRibbonStyle(SARibbonBar::RibbonStyleCompactThreeRow);"));
        break;
    case SARibbonBar::RibbonStyleCompactTwoRow:
        // cn:"CompactTwoRow"样式的文字显示是不换行的，不会显示标题栏，你也可以通过SARibbonBar::setEnableWordWrap来控制按钮是否换行显示，
        // 可以通过SARibbonBar::setEnableShowPanelTitle控制标题栏是否显示
        mTextedit->append(
            tr("\nThe text display of the \"CompactTwoRow\" style is not word wrap and not displays the title bar. "
               "You can also control whether the button is line breaking through SARibbonBar::setEnableWordWrap,"
               "and whether the title bar is displayed through SARibbonBar::setEnableShowPanelTitle"));
        mTextedit->append(tr("ribbonBar()->setRibbonStyle(SARibbonBar::RibbonStyleCompactTwoRow);"));
        break;
    default:
        break;
    }
}

/**
 * @brief 自定义按钮槽
 *
 * cn:这里演示了如何使用@ref SARibbonCustomizeWidget 来实现界面的自定义
 * en:This demonstrates how to use @ref SARibbonCustomizeWidget to implement interface customization
 */
void MainWindow::onActionCustomizeTriggered()
{
    if (nullptr == mWidgetForCustomize) {
        mWidgetForCustomize =
            new SARibbonCustomizeWidget(this, this, Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint | Qt::Dialog);
        mWidgetForCustomize->setWindowModality(Qt::ApplicationModal);  // 设置阻塞类型
        mWidgetForCustomize->setAttribute(Qt::WA_ShowModal, true);     // 属性设置 true:模态 false:非模态
        mWidgetForCustomize->setupActionsManager(mActionsManager);
    }
    mWidgetForCustomize->show();
    mWidgetForCustomize->applys();
}

/**
 * @brief 自定义按钮槽
 *
 * cn:这里演示了如何使用@ref SARibbonCustomizeDialog 来实现界面的自定义,同时演示了如何把自定义的界面保存为xml文件
 * en:This demonstrates how to use @ref SARibbonCustomizeDialog to implement interface customization, and also
 * demonstrates how to save the customized interface as an xml file
 */
void MainWindow::onActionCustomizeAndSaveTriggered()
{
    SARibbonCustomizeDialog dlg(this);
    dlg.setupActionsManager(mActionsManager);
    // 如果启动时未应用上次修改，先应用再读取,保持本地数据和ui一致
    if (!mHasApplyCustomizeXmlFile) {
        auto res = QMessageBox::question(
            this, tr("question"), tr("Apply the last modification?\nIf not, local data will be reset"));
        if (res == QMessageBox::Yes) {
            onActionLoadCustomizeXmlFileTriggered();
            return;
        } else {
            QFile::remove("customize.xml");
            dlg.clear();

            mHasApplyCustomizeXmlFile = true;
        }
    }

    dlg.fromXml("customize.xml");
    if (SARibbonCustomizeDialog::Accepted == dlg.exec()) {
        // 先apply
        if (dlg.isCached())
            dlg.applys();

        // 无更改直接退出
        if (!dlg.isApplied()) {
            mTextedit->append("no change to save");
            return;
        }

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
            mTextedit->append("write xml:");
            mTextedit->append(str);
        }
    }
}

/**
 * @brief 自定义按钮槽
 *
 * cn:这里演示了如何使用@ref SARibbonCustomizeWidget 嵌入到QDialog中实现界面的自定义,
 * 这样是为了让@ref SARibbonCustomizeWidget更好的和自己的软件设置页面结合
 * en:This demonstrates how to embed @ref SARibbonCustomizeWidget into a QDialog to implement interface customization,
 * This is to better integrate @ref SARibbonCustomizeWidget with your own software settings page
 */
void MainWindow::onActionCustomizeAndSaveWithApplyTriggered()
{
    // 如果启动时未应用上次修改，先应用再读取,保持本地数据和ui一致
    if (!mHasApplyCustomizeXmlFile) {
        auto res = QMessageBox::question(
            this, tr("question"), tr("Apply the last modification?\nIf not, local data will be reset"));
        if (res == QMessageBox::Yes) {
            onActionLoadCustomizeXmlFileTriggered();
            return;
        } else {
            QFile::remove("customize.xml");
            mHasApplyCustomizeXmlFile = true;
        }
    }

    QDialog dlg;
    QVBoxLayout* main = new QVBoxLayout;
    dlg.setLayout(main);
    SARibbonCustomizeWidget* widgetForCustomize = new SARibbonCustomizeWidget(this, &dlg);
    widgetForCustomize->setupActionsManager(mActionsManager);

    main->addWidget(widgetForCustomize, 1);

    QDialogButtonBox* buttonBox =
        new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel | QDialogButtonBox::Apply);

    main->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    connect(buttonBox, &QDialogButtonBox::clicked, &dlg, [ = ](QAbstractButton* button) {
        auto role = buttonBox->buttonRole(button);
        if (role == QDialogButtonBox::ApplyRole)  // apply
        {
            if (widgetForCustomize->isCached()) {
                widgetForCustomize->applys();
                mTextedit->append("change applied");
            } else {
                mTextedit->append("no change to apply");
            }
        }
    });

    widgetForCustomize->fromXml("customize.xml");
    if (QDialog::Accepted == dlg.exec()) {
        // 先apply
        if (widgetForCustomize->isCached())
            widgetForCustomize->applys();

        // 无更改直接退出
        if (!widgetForCustomize->isApplied()) {
            mTextedit->append("no change to save");
            return;
        }

        QByteArray str;
        QXmlStreamWriter xml(&str);
        xml.setAutoFormatting(true);
        xml.setAutoFormattingIndent(2);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)  // QXmlStreamWriter always encodes XML in UTF-8.
        xml.setCodec("utf-8");
#endif
        xml.writeStartDocument();
        bool isok = widgetForCustomize->toXml(&xml);
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
            mTextedit->append("write xml:");
            mTextedit->append(str);
        }
    } else {
        // 清除所有动作
        widgetForCustomize->clear();
        mTextedit->append("all changes clear, the applied changes will take no effect afer restart");
    }
}

/**
 * @brief 帮助信息按钮槽函数
 *
 * cn:显示SARibbon库的版本信息和作者联系方式
 * en:Display SARibbon library version information and author contact information
 */
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

/**
 * @brief 移除ApplicationButton
 * @param b 是否移除
 *
 * cn:演示如何动态添加或移除ApplicationButton
 * en:Demonstrate how to dynamically add or remove ApplicationButton
 */
void MainWindow::onActionRemoveAppBtnTriggered(bool b)
{
    if (b) {
        ribbonBar()->setApplicationButton(nullptr);
    } else {
        SARibbonApplicationButton* actionRemoveAppBtn = new SARibbonApplicationButton();
        actionRemoveAppBtn->setText(tr("&File"));
        this->ribbonBar()->setApplicationButton(actionRemoveAppBtn);
        createRibbonApplicationButton();
    }
}

/**
 * @brief 使用QSS样式表
 *
 * cn:演示如何通过QSS文件自定义Ribbon界面样式
 * en:Demonstrate how to customize Ribbon interface styles through QSS files
 */
void MainWindow::onActionUseQssTriggered()
{
    QFile f("ribbon.qss");
    if (!f.exists()) {
        QString fdir = QFileDialog::getOpenFileName(this, tr("select qss file"));
        if (fdir.isEmpty()) {
            return;
        }
        f.setFileName(fdir);
    }
    if (!f.open(QIODevice::ReadWrite)) {
        return;
    }
    QString qss(f.readAll());
    mTextedit->setText(qss);
    this->ribbonBar()->setStyleSheet(qss);
}

/**
 * @brief 加载自定义XML配置文件
 *
 * cn:从XML文件加载之前保存的自定义配置
 * en:Load previously saved custom configuration from XML file
 */
void MainWindow::onActionLoadCustomizeXmlFileTriggered()
{
    // 只能调用一次
    if (!mHasApplyCustomizeXmlFile) {
        if (!QFile::exists("customize.xml")) {
            mHasApplyCustomizeXmlFile = true;
            return;
        }
        QFile f("customize.xml");
        qDebug() << "size of customize.xml : " << f.size();
        if (f.size() <= 0) {
            mHasApplyCustomizeXmlFile = true;
            return;
        }
        mHasApplyCustomizeXmlFile = sa_apply_customize_from_xml_file("customize.xml", ribbonBar(), mActionsManager);
    }
}

/**
 * @brief 窗口按钮显示控制
 * @param b 是否显示窗口按钮
 *
 * cn:控制窗口的最小化、最大化和关闭按钮的显示
 * en:Control the display of window minimize, maximize, and close buttons
 */
void MainWindow::onActionWindowFlagNormalButtonTriggered(bool b)
{
    Qt::WindowFlags f = windowFlags();
    f.setFlag(Qt::WindowCloseButtonHint, b);
    f.setFlag(Qt::WindowMinMaxButtonsHint, b);
    setWindowFlags(f);
}

/**
 * @brief 字体组合框当前字体改变槽函数
 * @param f 新的字体
 *
 * cn:当用户在字体组合框中选择新字体时，更新Ribbon界面的字体
 * en:When the user selects a new font in the font combobox, update the font of the Ribbon interface
 */
void MainWindow::onFontComWidgetCurrentFontChanged(const QFont& f)
{
    qDebug() << "set font:" << f;
    ribbonBar()->setFont(f);
    update();
}

/**
 * @brief 增大字体按钮槽函数
 *
 * cn:增大Ribbon界面字体大小
 * en:Increase the font size of the Ribbon interface
 */
void MainWindow::onActionFontLargerTriggered()
{
    QFont f = font();
    f.setPointSize(f.pointSize() + 1);
    qDebug() << "before set ribbonBar font:" << ribbonBar()->font();
    ribbonBar()->setFont(f);
    update();
    qDebug() << "set font:" << f;
}

/**
 * @brief 减小字体按钮槽函数
 *
 * cn:减小Ribbon界面字体大小
 * en:Decrease the font size of the Ribbon interface
 */
void MainWindow::onActionFontSmallerTriggered()
{
    QFont f = font();
    f.setPointSize(f.pointSize() - 1);
    qDebug() << "before set ribbonBar font:" << ribbonBar()->font();
    ribbonBar()->setFont(f);
    update();
    qDebug() << "set font:" << f;
}

/**
 * @brief 文字换行控制
 * @param b 是否启用文字换行
 *
 * cn:控制Ribbon按钮上的文字是否换行显示
 * en:Control whether text on Ribbon buttons is displayed with line breaks
 */
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
 * @brief 按钮组动作触发槽函数
 * @param act 触发的动作
 *
 * cn:处理按钮组中按钮的点击事件，用于设置标题对齐方式
 * en:Handle button click events in the button group, used to set title alignment
 */
void MainWindow::onButtonGroupActionTriggered(QAction* act)
{
    QVariant v = act->property("align");
    if (v.isValid()) {
        Qt::Alignment al = static_cast< Qt::Alignment >(v.toInt());
        if (!ribbonBar()) {
            return;
        }
        ribbonBar()->setWindowTitleAligment(al);
    }
}

/**
 * @brief 颜色按钮颜色点击槽函数
 * @param c 选择的颜色
 * @param on 是否选中
 *
 * cn:处理颜色按钮的颜色选择事件
 * en:Handle color selection events for color buttons
 */
void MainWindow::onColorButtonColorClicked(const QColor& c, bool on)
{
    Q_UNUSED(on);
    mTextedit->append(QString("color click %1").arg(c.name()));
}

/**
 * @brief Ribbon主题改变槽函数
 * @param index 主题索引
 *
 * cn:切换Ribbon界面主题
 * en:Switch Ribbon interface theme
 */
void MainWindow::onRibbonThemeComboBoxCurrentIndexChanged(int index)
{
    SARibbonTheme t = static_cast< SARibbonTheme >(mComboboxRibbonTheme->itemData(index).toInt());
    setRibbonTheme(t);
}

/**
 * @brief 隐藏动作按钮槽函数
 * @param on 是否隐藏
 *
 * cn:批量控制一组动作的可见性
 * en:Batch control the visibility of a group of actions
 */
void MainWindow::onActionHideActionTriggered(bool on)
{
    mActionWordWrap->setVisible(on);
    mActionDisable->setVisible(on);
    mActionUnlock->setVisible(on);
    mActionSetTextTest->setVisible(on);
    mActionShowTest->setVisible(on);
    mActionHideAction2->setVisible(on);
    mActionHideAction4->setVisible(on);
    mActionHideShowTextAct2->setVisible(on);
    mActionHideShowTextAct3->setVisible(on);
    mActionHideShowTextAct4->setVisible(on);
    ribbonBar()->updateRibbonGeometry();
}

/**
 * @brief 切换所有动作可见性
 * @param on 是否可见
 *
 * cn:控制所有注册到动作管理器的动作的可见性
 * en:Control the visibility of all actions registered to the action manager
 */
void MainWindow::onActionVisibleAllTriggered(bool on)
{
    const QList< QAction* > acts = mActionsManager->allActions();
    for (QAction* a : acts) {
        if (a != mActionVisibleAll) {
            a->setVisible(on);
        }
    }
    ribbonBar()->updateRibbonGeometry();
}

/**
@brief 居中对齐checkbox的槽
@param checked 是否选中

cn:控制Ribbon工具栏内容是否居中对齐
en:Control whether Ribbon toolbar content is center aligned
*/
void MainWindow::onCheckBoxAlignmentCenterClicked(bool checked)
{
    if (checked) {
        ribbonBar()->setRibbonAlignment(SARibbonAlignment::AlignCenter);
    } else {
        ribbonBar()->setRibbonAlignment(SARibbonAlignment::AlignLeft);
    }
}

/**
 * @brief Ribbon标题栏高度改变槽函数
 * @param h 新的高度
 *
 * cn:调整Ribbon标题栏高度
 * en:Adjust Ribbon title bar height
 */
void MainWindow::onSpinBoxRibbonTitleHeightChanged(int h)
{
    ribbonBar()->setTitleBarHeight(h);
}

/**
 * @brief Ribbon标签栏高度改变槽函数
 * @param h 新的高度
 *
 * cn:调整Ribbon标签栏高度
 * en:Adjust Ribbon tab bar height
 */
void MainWindow::onSpinBoxRibbonTabHeightChanged(int h)
{
    ribbonBar()->setTabBarHeight(h);
}

/**
 * @brief Ribbon分类区域高度改变槽函数
 * @param h 新的高度
 *
 * cn:调整Ribbon分类区域高度
 * en:Adjust Ribbon category area height
 */
void MainWindow::onSpinBoxRibbonCategoryHeightChanged(int h)
{
    ribbonBar()->setCategoryHeight(h);
}

/**
 * @brief Ribbon面板标题高度改变槽函数
 * @param h 新的高度
 *
 * cn:调整Ribbon面板标题高度
 * en:Adjust Ribbon panel title height
 */
void MainWindow::onSpinBoxRibbonPanelTtitleHeightChanged(int h)
{
    ribbonBar()->setPanelTitleHeight(h);
}

/**
 * @brief Ribbon面板间距改变槽函数
 * @param h 新的间距
 *
 * cn:调整Ribbon面板之间的间距
 * en:Adjust the spacing between Ribbon panels
 */
void MainWindow::onSpinBoxRibbonPanelSpacingChanged(int h)
{
    ribbonBar()->setPanelSpacing(h);
}

/**
 * @brief Ribbon面板工具按钮图标大小改变槽函数
 * @param h 新的图标大小
 *
 * cn:调整Ribbon面板中工具按钮的图标大小
 * en:Adjust the icon size of tool buttons in Ribbon panels
 */
void MainWindow::onSpinBoxRibbonPanelToolBtnIconSizeChanged(int h)
{
    ribbonBar()->setPanelToolButtonIconSize(QSize(h, h));
}

/**
 * @brief 工具按钮最大宽高比改变槽函数
 * @param h 新的宽高比
 *
 * cn:调整工具按钮的最大宽高比
 * en:Adjust the maximum aspect ratio of tool buttons
 */
void MainWindow::onDoubleSpinBoxToolbuttonMaximumAspectRatioChanged(double h)
{
    ribbonBar()->setButtonMaximumAspectRatio(h);
}

/**
 * @brief 此函数用来演示标题栏颜色改变
 *
 * cn:周期性地改变标题栏背景颜色以演示动态效果
 * en:Periodically change the title bar background color to demonstrate dynamic effects
 */
void MainWindow::onTitleBackgroundBrushChangedTimeout()
{
    static bool s_setNoBrush = true;
    s_setNoBrush             = (!s_setNoBrush);
    if (s_setNoBrush) {
        ribbonBar()->setWindowTitleTextColor(Qt::black);
        ribbonBar()->setWindowTitleBackgroundBrush(Qt::NoBrush);
    } else {
        ribbonBar()->setWindowTitleTextColor(Qt::white);
        ribbonBar()->setWindowTitleBackgroundBrush(Qt::red);
    }
    ribbonBar()->repaint();
}

/**
 * @brief 周期性改变标题颜色
 *
 * cn:定时器槽函数，用于周期性地改变标题栏颜色
 * en:Timer slot function used to periodically change the title bar color
 */
void MainWindow::onChangedTitleTimeout()
{
    static int s_a = 0;
    auto ribbon    = ribbonBar();
    if (!ribbon) {
        return;
    }
    ++s_a;
    if (s_a % 2 == 0) {
        ribbon->setWindowTitleBackgroundBrush(QColor(222, 79, 79));
        ribbon->setWindowTitleTextColor(Qt::white);
        ribbon->update();
    } else {
        ribbon->setWindowTitleBackgroundBrush(Qt::NoBrush);
        ribbon->setWindowTitleTextColor(Qt::blue);
        ribbon->update();
    }
}

/**
 * @brief 创建其它动作，这些动作并不在SARibbonBar管理
 *
 * cn:创建一些不在Ribbon界面中直接显示的动作，但可以通过自定义功能使用
 * en:Create some actions that are not directly displayed in the Ribbon interface, but can be used through customization features
 */
void MainWindow::createOtherActions()
{
    mOtherAction1     = new QAction(("text action1"), this);
    mOtherAction2     = new QAction(("text action2"), this);
    mOtherAction3     = new QAction(("text action3"), this);
    mOtherAction4     = new QAction(("text action4"), this);
    mOtherAction5     = new QAction(("text action5"), this);
    mOtherActionIcon1 = new QAction(QIcon(":/icon/icon/layout.svg"), ("action with icon"), this);
}

/**
 * @brief 窗口关闭事件处理
 * @param e 关闭事件
 *
 * cn:在窗口关闭前询问用户确认
 * en:Ask user for confirmation before closing the window
 */
void MainWindow::closeEvent(QCloseEvent* e)
{
    auto res = QMessageBox::question(this, tr("question"), tr("Confirm whether to exit"));
    if (res == QMessageBox::Yes) {
        e->accept();
    } else {
        e->ignore();
    }
}

/**
 * @brief 创建主分类页面内容
 * @param page 主分类页面指针
 *
 * cn:构建主分类页面的各个面板和控件，展示Ribbon的基本用法
 * en:Build panels and controls for the main category page, demonstrating basic usage of Ribbon
 */
void MainWindow::createCategoryMain(SARibbonCategory* page)
{
    //! 1
    //! panel 1 start
    //!

    // 使用addPanel函数来创建SARibbonPanel，效果和new SARibbonPanel再addPanel一样
    SARibbonPanel* panelStyle = page->addPanel(tr("ribbon style"));

    //! 这里演示如何创建一个大按钮,并创建快捷键
    QAction* actionSave = createAction(tr("Save"), ":/icon/icon/save.svg");
    connect(actionSave, &QAction::triggered, this, [ this ](bool b) {
        Q_UNUSED(b);
        this->mTextedit->append("actSaveion clicked");
        this->setWindowModified(false);
        // 更新状态后，需要手动调用ribbonbar刷新重绘标题，目前ribbonbar不会自动检测WindowModified状态
        this->ribbonBar()->repaint();
    });
    // 快捷键设置示范，如果你想你的快捷键能在整个MainWindow生命周期都显示，你应该把这个action也添加到MainWindow中
    actionSave->setShortcut(QKeySequence(QLatin1String("Ctrl+S")));
    // 这里调用addAction，是为了把actionSave添加到主窗口，这样，只要主窗口在最前面，就能响应快捷键，否则，只有main tab显示才会显示快捷键
    addAction(actionSave);
    panelStyle->addLargeAction(actionSave);

    //! hide ribbon action
    //! 注意
    //! 这里你可以看到文字设置时加了换行，在ribbon中，如果在多行文本模式下，会让文字主动换行，
    //! 如果设置为单行文本，这个换行会忽略
    QAction* actHideRibbon = createAction(tr("hide ribbon"), ":/icon/icon/hideRibbon.svg", "actHideRibbon");
    actHideRibbon->setCheckable(true);
    panelStyle->addSmallAction(actHideRibbon);
    connect(actHideRibbon, &QAction::triggered, this, [ this ](bool b) { this->ribbonBar()->setMinimumMode(b); });
    connect(ribbonBar(), &SARibbonBar::ribbonModeChanged, this, [ actHideRibbon ](SARibbonBar::RibbonMode nowNode) {
        actHideRibbon->setChecked(nowNode == SARibbonBar::MinimumRibbonMode);
    });

    //! 这里演示如何显示把ribbon内置的最小化ribbon按钮显示和隐藏
    //! ribbon内置的最小化ribbon按钮位于右侧的工具栏
    QAction* actShowHideButton =
        createAction(tr("show \nhide button"), ":/icon/icon/showHideButton.svg", "show hide button");
    actShowHideButton->setCheckable(true);
    actShowHideButton->setChecked(ribbonBar()->haveShowMinimumModeButton());
    panelStyle->addSmallAction(actShowHideButton);  // wrod wrap was not effect in small button
    connect(actShowHideButton, &QAction::triggered, this, [ this ](bool b) {
        this->ribbonBar()->showMinimumModeButton(b);  // 显示ribbon最小化按钮
    });

    //! 这里演示如何设置ribbon按钮文字换行
    mActionWordWrap = createAction(tr("word wrap"), ":/icon/icon/wordwrap.svg");
    mActionWordWrap->setCheckable(true);
    mActionWordWrap->setChecked(ribbonBar()->isEnableWordWrap());
    panelStyle->addSmallAction(mActionWordWrap);
    connect(mActionWordWrap, &QAction::triggered, this, &MainWindow::onActionWordWrapTriggered);

    //! 显示标题图标
    QAction* actShowTitleIcon =
        createAction(tr("show\n title icon"), ":/icon/icon/showHideButton.svg", "show hide title icon");
    actShowTitleIcon->setCheckable(true);
    actShowTitleIcon->setChecked(ribbonBar()->isTitleIconVisible());
    connect(actShowTitleIcon, &QAction::triggered, this, [ this ](bool b) {
        this->ribbonBar()->setTitleIconVisible(b);  // 显示ribbon最小化按钮
    });
    panelStyle->addSmallAction(actShowTitleIcon);

    //! 这里演示如何设置让Application Button占用标题栏和tab栏
    QAction* actSetAppBtnExpand = createAction(tr("App Btn\n Expand"), ":/icon/icon/layout.svg");
    actSetAppBtnExpand->setCheckable(true);
    actSetAppBtnExpand->setChecked(ribbonBar()->isApplicationButtonVerticalExpansion());
    panelStyle->addSmallAction(actSetAppBtnExpand);
    connect(actSetAppBtnExpand, &QAction::triggered, this, [ this, actShowTitleIcon ](bool on) {
        ribbonBar()->setApplicationButtonVerticalExpansion(on);
        if (on) {
            // 设置application button扩展，title icon是会被强制隐藏
            actShowTitleIcon->setChecked(false);
        }
    });

    panelStyle->addSeparator();

    QButtonGroup* buttongroup = new QButtonGroup(page);

    QRadioButton* r = new QRadioButton();
    r->setText(tr("use office style"));
    r->setObjectName(("use office style"));
    r->setWindowTitle(r->text());
    r->setChecked(true);
    panelStyle->addSmallWidget(r);
    buttongroup->addButton(r, SARibbonBar::RibbonStyleLooseThreeRow);

    r = new QRadioButton();
    r->setObjectName(("use wps style"));
    r->setText(tr("use wps style"));
    r->setWindowTitle(r->text());
    r->setChecked(false);
    panelStyle->addSmallWidget(r);
    buttongroup->addButton(r, SARibbonBar::RibbonStyleCompactThreeRow);

    r = new QRadioButton();
    r->setObjectName(("use office 2row style"));
    r->setText(tr("use office 2 row style"));
    r->setWindowTitle(r->text());
    r->setChecked(false);
    panelStyle->addSmallWidget(r);
    buttongroup->addButton(r, SARibbonBar::RibbonStyleLooseTwoRow);

    r = new QRadioButton();
    r->setObjectName(("use wps 2row style"));
    r->setText(tr("use wps 2row style"));
    r->setWindowTitle(r->text());
    r->setChecked(false);
    panelStyle->addSmallWidget(r);
    buttongroup->addButton(r, SARibbonBar::RibbonStyleCompactTwoRow);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    connect(buttongroup, static_cast< void (QButtonGroup::*)(int) >(&QButtonGroup::idClicked), this, &MainWindow::onStyleClicked);
#else
    connect(buttongroup,
            static_cast< void (QButtonGroup::*)(int) >(&QButtonGroup::buttonClicked),
            this,
            &MainWindow::onStyleClicked);
#endif

    mComboboxRibbonTheme = new QComboBox();
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
            QOverload< int >::of(&QComboBox::currentIndexChanged),
            this,
            &MainWindow::onRibbonThemeComboBoxCurrentIndexChanged);
    panelStyle->addSmallWidget(mComboboxRibbonTheme);

    QCheckBox* checkBox = new QCheckBox(this);

    checkBox->setText(tr("Alignment Center"));
    checkBox->setObjectName("checkBoxAlignmentCenter");
    checkBox->setWindowTitle(checkBox->text());
    connect(checkBox, &QCheckBox::clicked, this, &MainWindow::onCheckBoxAlignmentCenterClicked);
    panelStyle->addSmallWidget(checkBox);

    SARibbonPanel* panelToolButtonStyle = page->addPanel(("sa ribbon toolbutton style"));

    SARibbonMenu* menu = new SARibbonMenu(this);
    QAction* a         = nullptr;
    {
        QIcon itemicon = QIcon(":/icon/icon/item.svg");
        for (int i = 0; i < 5; ++i) {
            a = menu->addAction(itemicon, tr("item %1").arg(i + 1));
            a->setObjectName(QStringLiteral("item %1").arg(i + 1));
        }
    }

    QAction* act = createAction(tr("test 1"), ":/icon/icon/test1.svg");
    QVariant temp("Test");
    act->setData(temp);
    act->setMenu(menu);
    act->setToolTip(tr("use QToolButton::MenuButtonPopup mode"));
    panelToolButtonStyle->addSmallAction(act, QToolButton::MenuButtonPopup);

    act = createAction(tr("test 2"), ":/icon/icon/test2.svg");
    act->setMenu(menu);
    act->setToolTip(tr("use QToolButton::InstantPopup mode"));
    panelToolButtonStyle->addSmallAction(act, QToolButton::InstantPopup);

    panelToolButtonStyle->addSeparator();

    act = createAction(tr("Delayed\nPopup"), ":/icon/icon/folder-cog.svg");
    act->setMenu(menu);
    panelToolButtonStyle->addLargeAction(act, QToolButton::DelayedPopup);

    connect(act, &QAction::triggered, this, &MainWindow::onDelayedPopupCheckabletriggered);

    act = createAction(tr("Menu Button Popup"), ":/icon/icon/folder-star.svg");
    act->setMenu(menu);
    panelToolButtonStyle->addLargeAction(act, QToolButton::MenuButtonPopup);
    connect(act, &QAction::triggered, this, &MainWindow::onMenuButtonPopupCheckabletriggered);

    act = createAction(tr("Instant Popup"), ":/icon/icon/folder-stats.svg");
    act->setMenu(menu);
    panelToolButtonStyle->addLargeAction(act, QToolButton::InstantPopup);
    connect(act, &QAction::triggered, this, &MainWindow::onInstantPopupCheckabletriggered);

    act = createAction(tr("Delayed Popup checkable"), ":/icon/icon/folder-table.svg");
    act->setCheckable(true);
    act->setMenu(menu);
    panelToolButtonStyle->addLargeAction(act, QToolButton::DelayedPopup);
    connect(act, &QAction::triggered, this, &MainWindow::onDelayedPopupCheckableTest);

    act = createAction(tr("Menu Button Popup checkable"), ":/icon/icon/folder-checkmark.svg");
    act->setCheckable(true);
    act->setMenu(menu);
    panelToolButtonStyle->addLargeAction(act, QToolButton::MenuButtonPopup);
    connect(act, &QAction::triggered, this, &MainWindow::onMenuButtonPopupCheckableTest);

    act = createAction(tr("disable action"), ":/icon/icon/disable.svg");
    act->setCheckable(true);
    act->setMenu(menu);
    act->setEnabled(false);
    panelToolButtonStyle->addLargeAction(act);

    QAction* optAct = new QAction(this);
    connect(optAct, &QAction::triggered, this, [ this ](bool on) {
        Q_UNUSED(on);
        QMessageBox::information(this, tr("Option Action Triggered"), tr("Option Action Triggered"));
    });
    panelToolButtonStyle->setOptionAction(optAct);
    //! 2
    //! panel 2 start
    //!
    SARibbonPanel* panel2 = page->addPanel(("panel 2"));

    QAction* actShowContext = createAction(tr("show Context"), ":/icon/icon/showContext.svg");
    actShowContext->setCheckable(true);
    panel2->addLargeAction(actShowContext);
    connect(actShowContext, &QAction::triggered, this, &MainWindow::onShowContextCategory);

    QAction* actDeleteContext = createAction(tr("delete Context"), ":/icon/icon/deleteContext.svg");
    panel2->addLargeAction(actDeleteContext);
    connect(actDeleteContext, &QAction::triggered, this, [ this, act ](bool on) {
        Q_UNUSED(on);
        if (this->mContextCategory) {
            this->ribbonBar()->destroyContextCategory(this->mContextCategory);
            this->mContextCategory = nullptr;
            act->setDisabled(true);
        }
    });

    act = createAction(tr("Word\nWrap"), ":/icon/icon/setText.svg");
    panel2->addLargeAction(act);
    connect(act, &QAction::triggered, this, [ this ](bool on) {
        Q_UNUSED(on);
        this->mTextedit->append(tr("Text can be manually wrapped(use \\n), and will appear as 1 line in the case of "
                                   "SARibbonBar::setEnableWordWrap (false)"));  // cn:文本中手动换行
    });

    act = createAction(tr("Word \nWrap"), ":/icon/icon/setText.svg");
    act->setMenu(menu);
    panel2->addLargeAction(act);
    connect(act, &QAction::triggered, this, [ this ](bool on) {
        Q_UNUSED(on);
        this->mTextedit->append(tr("Text can be manually wrapped(use \\n), and will appear as 1 line in the case of "
                                   "SARibbonBar::setEnableWordWrap (false)"));  // cn:文本中手动换行
    });

    act = createAction(QString(), ":/icon/icon/setText.svg", "ToolButtonIconOnly");
    panel2->addLargeAction(act);
    // 通过此函数获取addAction后生成的ToolButton
    if (SARibbonToolButton* toolbutton = panel2->lastAddActionButton()) {
        toolbutton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    }

    act = createAction(tr("Text Only"), ":/icon/icon/setText.svg", "ToolButtonTextOnly");
    panel2->addLargeAction(act);
    // 通过此函数获取addAction后生成的ToolButton
    if (SARibbonToolButton* toolbutton = panel2->lastAddActionButton()) {
        toolbutton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    }
    //! 3
    //! panel 3 start -> widget test
    //!

    SARibbonPanel* panelWidgetTest = page->addPanel(tr("widget test"));
    panelWidgetTest->setObjectName(QStringLiteral(u"panelWidgetTest"));

    QComboBox* com1 = new QComboBox(this);
    com1->setObjectName("QComboBox test");
    for (int i = 0; i < 40; ++i) {
        com1->addItem(QString("QComboBox test%1").arg(i + 1));
    }
    com1->setEditable(true);
    panelWidgetTest->addSmallWidget(com1);

    QComboBox* com2 = new QComboBox(this);
    com2->setObjectName("ComboBox Editable");
    for (int i = 0; i < 40; ++i) {
        com2->addItem(QString("item %1").arg(i + 1));
    }
    panelWidgetTest->addSmallWidget(com2);

    QLineEdit* lineEdit = new QLineEdit(this);
    lineEdit->setObjectName("Line Edit");
    lineEdit->setText("LineEdit");
    panelWidgetTest->addSmallWidget(lineEdit);

    panelWidgetTest->addSeparator();

    QCalendarWidget* calendarWidget = new QCalendarWidget(this);
    calendarWidget->setObjectName(("calendarWidget"));
    panelWidgetTest->addLargeWidget(calendarWidget);
    optAct = new QAction(this);
    connect(optAct, &QAction::triggered, this, [ this ](bool on) {
        Q_UNUSED(on);
        QMessageBox::information(this, tr("Option Action Triggered"), tr("Option Action Triggered"));
    });
    panelWidgetTest->setOptionAction(optAct);

    panelWidgetTest->setVisible(true);
}

/**
 * @brief 创建其他分类页面内容
 * @param page 其他分类页面指针
 *
 * cn:构建其他分类页面的内容，展示更多Ribbon控件的用法
 * en:Build content for other category pages, demonstrating more Ribbon control usages
 */
void MainWindow::createCategoryOther(SARibbonCategory* page)
{

    SARibbonPanel* panel1 = new SARibbonPanel(tr("panel one"));
    panel1->setObjectName("CategoryOther-panel1");
    page->addPanel(panel1);
    // 按钮组1
    SARibbonButtonGroupWidget* btnGroup1 = new SARibbonButtonGroupWidget(panel1);
    btnGroup1->setObjectName("SARibbonButtonGroupWidget1");

    btnGroup1->addAction(createAction(tr("Decrease Margin"), ":/icon/icon/Decrease-Margin.svg"));
    btnGroup1->addAction(createAction(tr("Decrease Indent"), ":/icon/icon/Decrease-Indent.svg"));
    btnGroup1->addAction(createAction(tr("Wrap Image Left"), ":/icon/icon/Wrap-Image Left.svg"));
    btnGroup1->addAction(createAction(tr("Wrap Image Right"), ":/icon/icon/Wrap-Image Right.svg"));
    panel1->addWidget(btnGroup1, SARibbonPanelItem::Medium);

    // 按钮组2
    SARibbonButtonGroupWidget* btnGroup2 = new SARibbonButtonGroupWidget(panel1);
    btnGroup2->setObjectName("SARibbonButtonGroupWidget2");

    QAction* titleAlgnmentRight = createAction(tr("Align Right"), ":/icon/icon/Align-Right.svg");
    titleAlgnmentRight->setProperty("align", (int)Qt::AlignRight | Qt::AlignVCenter);
    btnGroup2->addAction(titleAlgnmentRight);
    QAction* titleAlgnmentLeft = createAction(tr("Align Left"), ":/icon/icon/Align-Left.svg");
    titleAlgnmentLeft->setProperty("align", (int)Qt::AlignLeft | Qt::AlignVCenter);
    btnGroup2->addAction(titleAlgnmentLeft);
    QAction* titleAlgnmentCenter = createAction(tr("Align Center"), ":/icon/icon/Align-Center.svg");
    titleAlgnmentCenter->setProperty("align", (int)Qt::AlignCenter);
    btnGroup2->addAction(titleAlgnmentCenter);
    panel1->addWidget(btnGroup2, SARibbonPanelItem::Medium);
    connect(btnGroup2, &SARibbonButtonGroupWidget::actionTriggered, this, &MainWindow::onButtonGroupActionTriggered);

    // Gallery
    SARibbonGallery* gallery = panel1->addGallery();
    QList< QAction* > galleryActions;
    auto lambdaCreateGalleryAction = [ this ](const QString& text, const QString& iconurl) -> QAction* {
        QAction* act = this->createAction(text, iconurl);
        this->connect(act, &QAction::triggered, this, [ this, text ]() {
            if (this->mTextedit) {
                this->mTextedit->append(QString("%1 triggered").arg(text));
            }
        });
        return act;
    };
    galleryActions.append(lambdaCreateGalleryAction("Document File", ":/gallery-icon/icon/gallery/Document-File.svg"));
    galleryActions.append(lambdaCreateGalleryAction("Download File", ":/gallery-icon/icon/gallery/Download-File.svg"));
    galleryActions.append(lambdaCreateGalleryAction("Drive File Four Word", ":/gallery-icon/icon/gallery/Drive-File.svg"));
    galleryActions.append(lambdaCreateGalleryAction("Dropbox File", ":/gallery-icon/icon/gallery/Dropbox-File.svg"));
    galleryActions.append(lambdaCreateGalleryAction("Email File", ":/gallery-icon/icon/gallery/Email-File.svg"));
    galleryActions.append(lambdaCreateGalleryAction("Encode File", ":/gallery-icon/icon/gallery/Encode-File.svg"));
    galleryActions.append(lambdaCreateGalleryAction("Favorit File", ":/gallery-icon/icon/gallery/Favorit-File.svg"));
    galleryActions.append(lambdaCreateGalleryAction("File Error", ":/gallery-icon/icon/gallery/File-Error.svg"));
    galleryActions.append(lambdaCreateGalleryAction("File Read Only", ":/gallery-icon/icon/gallery/File-Readonly.svg"));
    galleryActions.append(lambdaCreateGalleryAction("File Settings", ":/gallery-icon/icon/gallery/File-Settings.svg"));
    galleryActions.append(
        lambdaCreateGalleryAction("Presentation File", ":/gallery-icon/icon/gallery/Presentation-File.svg"));
    SARibbonGalleryGroup* group1 = gallery->addCategoryActions(tr("Files"), galleryActions);
    group1->setGalleryGroupStyle(SARibbonGalleryGroup::IconWithWordWrapText);
    group1->setGridMinimumWidth(80);
    galleryActions.clear();
    galleryActions.append(lambdaCreateGalleryAction("Photoshop", ":/gallery-icon/icon/gallery/Photoshop.svg"));
    galleryActions.append(
        lambdaCreateGalleryAction("Internet-Explorer", ":/gallery-icon/icon/gallery/Internet-Explorer.svg"));
    galleryActions.append(lambdaCreateGalleryAction("Illustrator", ":/gallery-icon/icon/gallery/Illustrator.svg"));
    galleryActions.append(lambdaCreateGalleryAction("Google-Maps", ":/gallery-icon/icon/gallery/Google-Maps.svg"));
    galleryActions.append(lambdaCreateGalleryAction("Adobe", ":/gallery-icon/icon/gallery/Adobe.svg"));
    galleryActions.append(lambdaCreateGalleryAction("Word", ":/gallery-icon/icon/gallery/Word.svg"));
    SARibbonGalleryGroup* group2 = gallery->addCategoryActions(tr("Apps"), galleryActions);
    group2->setGridMinimumWidth(80);
    gallery->setCurrentViewGroup(group1);

    QAction* optAct = new QAction(this);

    optAct->setObjectName(("debug"));
    panel1->setOptionAction(optAct);

    SARibbonPanel* panel2 = new SARibbonPanel(tr("panel two"));
    panel2->setObjectName("CategoryOther-panel2");
    page->addPanel(panel2);
    QAction* actionRemoveAppBtn = createAction(tr("remove application button"), ":/icon/icon/remove-app-btn.svg");
    actionRemoveAppBtn->setCheckable(true);
    connect(actionRemoveAppBtn, &QAction::toggled, this, &MainWindow::onActionRemoveAppBtnTriggered);
    panel2->addLargeAction(actionRemoveAppBtn);

    QAction* actionLongText =
        createAction(tr("show very long text in a button,balabalabala etc"), ":/icon/icon/long-text.svg", "long-text");
    panel2->addLargeAction(actionLongText);

    SARibbonPanel* panelStyle = new SARibbonPanel(tr("style"));
    panelStyle->setObjectName("CategoryOther-panelStyle");
    page->addPanel(panelStyle);

    QAction* actionUseQss = createAction(tr("use qss"), ":/icon/icon/useqss.svg");
    connect(actionUseQss, &QAction::triggered, this, &MainWindow::onActionUseQssTriggered);
    panelStyle->addSmallAction(actionUseQss);

    QAction* actionLoadCustomizeXmlFile = createAction(tr("load customize from xml file"), ":/icon/icon/useCustomize.svg");
    connect(actionLoadCustomizeXmlFile, &QAction::triggered, this, &MainWindow::onActionLoadCustomizeXmlFileTriggered);
    panelStyle->addSmallAction(actionLoadCustomizeXmlFile);

    QAction* actionWindowFlagNormalButton = createAction(tr("window normal button"), ":/icon/icon/windowsflag-normal.svg");
    actionWindowFlagNormalButton->setCheckable(true);
    actionWindowFlagNormalButton->setChecked(true);
    panelStyle->addSmallAction(actionWindowFlagNormalButton);
    connect(actionWindowFlagNormalButton, &QAction::triggered, this, &MainWindow::onActionWindowFlagNormalButtonTriggered);

    SARibbonPanel* panelUtf8 = new SARibbonPanel(QStringLiteral(u"中文显示测试"));
    panelUtf8->setObjectName("CategoryOther-panelUtf8");
    page->addPanel(panelUtf8);

    QAction* actionChangeText =
        createAction(QStringLiteral(u"改变显示为英文"), ":/icon/icon/chinese-char.svg", "actionChangeText");
    actionChangeText->setCheckable(true);
    actionChangeText->setChecked(false);
    panelUtf8->addLargeAction(actionChangeText);
    connect(actionChangeText, &QAction::triggered, this, [ panelUtf8, actionChangeText ](bool on) {
        if (on) {
            panelUtf8->setPanelName(QStringLiteral(u"show chinese char"));
            actionChangeText->setText(QStringLiteral(u"show in chinese"));
        } else {
            panelUtf8->setPanelName(QStringLiteral(u"中文显示测试"));
            actionChangeText->setText(QStringLiteral(u"改变显示为英文"));
        }
    });

    SARibbonPanel* panelContextCategory = new SARibbonPanel(("Context Category"));
    page->addPanel(panelContextCategory);
    QAction* a1 = createAction("Context Category 1", ":/icon/icon/ContextCategory.svg");
    a1->setCheckable(true);
    connect(a1, &QAction::triggered, this, [ this ](bool c) {
        this->ribbonBar()->setContextCategoryVisible(mContextCategory, c);
    });

    QAction* a2 = createAction("Context Category 2", ":/icon/icon/ContextCategory.svg");
    a2->setCheckable(true);
    connect(a2, &QAction::triggered, this, [ this ](bool c) {
        this->ribbonBar()->setContextCategoryVisible(mContextCategory2, c);
    });
    panelContextCategory->addLargeAction(a1);
    panelContextCategory->addLargeAction(a2);
}

/**
 * @brief 创建删除分类页面内容
 * @param page 删除分类页面指针
 *
 * cn:演示如何动态创建和删除Ribbon分类页面
 * en:Demonstrate how to dynamically create and delete Ribbon category pages
 */
void MainWindow::createCategoryDelete(SARibbonCategory* page)
{
    SARibbonPanel* panel1 = new SARibbonPanel(("panel 1"));
    SARibbonPanel* panel2 = new SARibbonPanel(("panel 2"));

    QAction* actionRemovePanne = createAction(tr("remove panel"), ":/icon/icon/remove.svg");

    connect(actionRemovePanne, &QAction::triggered, this, [ page, panel2 ]() { page->removePanel(panel2); });
    panel1->addLargeAction(actionRemovePanne);

    QAction* actionRemoveCategory = createAction(tr("remove this category"), ":/icon/icon/removePage.svg");

    connect(actionRemoveCategory, &QAction::triggered, this, [ this, page, actionRemoveCategory ]() {
        this->ribbonBar()->removeCategory(page);
        page->hide();
        page->deleteLater();
        actionRemoveCategory->setDisabled(true);
    });
    panel2->addLargeAction(actionRemoveCategory);

    page->addPanel(panel1);
    page->addPanel(panel2);
}

/**
 * @brief 构建尺寸页
 * @param page 尺寸页指针
 *
 * cn:构建尺寸调整页面，展示字体和尺寸相关控件的使用
 * en:Build size adjustment page, demonstrating the use of font and size related controls
 */
void MainWindow::createCategorySize(SARibbonCategory* page)
{
    //=========================
    // panel ： Font
    //=========================
    SARibbonPanel* panel                       = page->addPanel(tr("Font"));
    SARibbonButtonGroupWidget* groupFontFamile = new SARibbonButtonGroupWidget(panel);
    groupFontFamile->setObjectName(QStringLiteral(u"group1"));
    QLabel* labelFontSize = new QLabel(groupFontFamile);
    labelFontSize->setText(tr("select font"));
    labelFontSize->setObjectName(QStringLiteral(u"labelFontSize"));
    groupFontFamile->addWidget(labelFontSize);
    QFontComboBox* fontComWidget = new QFontComboBox(groupFontFamile);
    fontComWidget->setObjectName(QStringLiteral(u"fontComboBox"));
    connect(fontComWidget, &QFontComboBox::currentFontChanged, this, &MainWindow::onFontComWidgetCurrentFontChanged);
    //! 给窗口设置WindowTitle，这样在自定义窗口中，也能显示这个action对应的名字
    //! 你也可以给group1设置一个icon
    //! setWindowTitle应该在addWidget之前
    groupFontFamile->setWindowTitle(tr("Font ComboBox"));
    groupFontFamile->addWidget(fontComWidget);
    QAction* actWidgetFontFamile = panel->addWidget(groupFontFamile, SARibbonPanelItem::Medium);
    //! 设置objectname是为了让actionManager正确管理action，如果不设置这个action对应的objectname，
    //! SARibbonPanel::addWidget会自动生成一个objectname，action.{widgetobjectName},因此你要保证你添加的widget有objectName
    actWidgetFontFamile->setObjectName(QStringLiteral("Size.Font.FontFamile"));

    SARibbonButtonGroupWidget* groupFontTools = new SARibbonButtonGroupWidget(panel);
    groupFontTools->setObjectName(QStringLiteral(u"group2"));
    groupFontTools->addAction(createAction("Bold", ":/icon/icon/bold.svg"));
    groupFontTools->addAction(createAction("Italic", ":/icon/icon/Italic.svg"));
    groupFontTools->addSeparator();
    groupFontTools->addAction(createAction("left alignment", ":/icon/icon/al-left.svg"));
    groupFontTools->addAction(createAction("center alignment", ":/icon/icon/al-center.svg"));
    groupFontTools->addAction(createAction("right alignment", ":/icon/icon/al-right.svg"));
    groupFontTools->addAction(createAction("line up on both sides", ":/icon/icon/al-bothside.svg"));
    //! 给窗口设置WindowTitle，这样在自定义窗口中，也能显示这个action对应的名字
    //! 你也可以给group1设置一个icon
    //! setWindowTitle应该在addWidget之前
    groupFontTools->setWindowTitle(tr("Font Tools"));
    QAction* actWidgetFontTools = panel->addWidget(groupFontTools, SARibbonPanelItem::Medium);
    //! 设置objectname是为了让actionManager正确管理action，如果不设置这个action对应的objectname，
    //! SARibbonPanel::addWidget会自动生成一个objectname，action.{widgetobjectName},因此你要保证你添加的widget有objectName
    actWidgetFontTools->setObjectName(QStringLiteral("Size.Font.FontTools"));

    panel->addSeparator();

    QAction* actLargerFontSize = createAction(tr("Larger"), ":/icon/icon/largerFont.svg", "actLargerFontSize");
    panel->addLargeAction(actLargerFontSize);

    QAction* actSmallFontSize = createAction(tr("Smaller"), ":/icon/icon/smallFont.svg", "actSmallFontSize");
    panel->addLargeAction(actSmallFontSize);

    connect(actLargerFontSize, &QAction::triggered, this, &MainWindow::onActionFontLargerTriggered);
    connect(actSmallFontSize, &QAction::triggered, this, &MainWindow::onActionFontSmallerTriggered);

    //=========================
    // panel ： Font Size
    //=========================
    panel   = new SARibbonPanel(tr("Font Size"));
    QFont f = font();
    f.setPointSize(14);
    panel->setFont(f);
    QAction* actLargeFontText = createAction(tr("Larger Font Text"), ":/icon/icon/file.svg", "actLargeFontText");
    panel->addLargeAction(actLargeFontText);
    page->addPanel(panel);

    // panel 3
    // 这里演示了一个窗口,带有gridlayoout
    panel          = new SARibbonPanel(tr("Grid"));
    QWidget* w     = new QWidget();
    QGridLayout* g = new QGridLayout(w);

    QLabel* lab0                       = new QLabel("Ribbon Title Height:");
    QSpinBox* spinboxRibbonTitleHeight = new QSpinBox(w);
    spinboxRibbonTitleHeight->setRange(10, 99);

    QLabel* lab1                     = new QLabel("Ribbon Tab Height:");
    QSpinBox* spinboxRibbonTabHeight = new QSpinBox(w);
    spinboxRibbonTabHeight->setRange(10, 99);

    QLabel* lab2                          = new QLabel("Ribbon Category Height:");
    QSpinBox* spinboxRibbonCategoryHeight = new QSpinBox(w);
    spinboxRibbonCategoryHeight->setRange(30, 300);

    QLabel* lab3                             = new QLabel("Ribbon Panel Title:");
    QSpinBox* spinboxRibbonPanelTtitleHeight = new QSpinBox(w);
    spinboxRibbonPanelTtitleHeight->setRange(5, 99);

    QLabel* lab4                        = new QLabel("Ribbon Panel Spacing:");
    QSpinBox* spinboxRibbonPanelSpacing = new QSpinBox(w);
    spinboxRibbonPanelSpacing->setRange(0, 25);

    QLabel* lab5                                = new QLabel("Panel ToolButton Icon Size:");
    QSpinBox* spinboxRibbonPanelToolBtnIconSize = new QSpinBox(w);
    spinboxRibbonPanelToolBtnIconSize->setRange(1, 99);

    QLabel* lab6                                              = new QLabel("Toolbutton Max Aspect Ratio:");
    QDoubleSpinBox* doublespinboxToolbuttonMaximumAspectRatio = new QDoubleSpinBox(w);
    doublespinboxToolbuttonMaximumAspectRatio->setRange(0.5, 99);
    doublespinboxToolbuttonMaximumAspectRatio->setSingleStep(0.2);

    g->addWidget(lab0, 0, 0);
    g->addWidget(spinboxRibbonTitleHeight, 0, 1);
    g->addWidget(lab1, 1, 0);
    g->addWidget(spinboxRibbonTabHeight, 1, 1);
    g->addWidget(lab2, 0, 2);
    g->addWidget(spinboxRibbonCategoryHeight, 0, 3);
    g->addWidget(lab3, 1, 2);
    g->addWidget(spinboxRibbonPanelTtitleHeight, 1, 3);
    g->addWidget(lab4, 0, 4);
    g->addWidget(spinboxRibbonPanelSpacing, 0, 5);
    g->addWidget(lab5, 1, 4);
    g->addWidget(spinboxRibbonPanelToolBtnIconSize, 1, 5);
    g->addWidget(lab6, 0, 6);
    g->addWidget(doublespinboxToolbuttonMaximumAspectRatio, 0, 7);
    spinboxRibbonTitleHeight->setValue(ribbonBar()->titleBarHeight());
    spinboxRibbonTabHeight->setValue(ribbonBar()->tabBarHeight());
    spinboxRibbonCategoryHeight->setValue(ribbonBar()->categoryHeight());
    spinboxRibbonPanelTtitleHeight->setValue(ribbonBar()->panelTitleHeight());
    spinboxRibbonPanelSpacing->setValue(ribbonBar()->panelSpacing());
    spinboxRibbonPanelToolBtnIconSize->setValue(ribbonBar()->panelToolButtonIconSize().width());
    doublespinboxToolbuttonMaximumAspectRatio->setValue(ribbonBar()->buttonMaximumAspectRatio());
#if QT_VERSION_MAJOR >= 6
    connect(spinboxRibbonTitleHeight, &QSpinBox::valueChanged, this, &MainWindow::onSpinBoxRibbonTitleHeightChanged);
    connect(spinboxRibbonTabHeight, &QSpinBox::valueChanged, this, &MainWindow::onSpinBoxRibbonTabHeightChanged);
    connect(spinboxRibbonCategoryHeight, &QSpinBox::valueChanged, this, &MainWindow::onSpinBoxRibbonCategoryHeightChanged);
    connect(spinboxRibbonPanelTtitleHeight, &QSpinBox::valueChanged, this, &MainWindow::onSpinBoxRibbonPanelTtitleHeightChanged);
    connect(spinboxRibbonPanelSpacing, &QSpinBox::valueChanged, this, &MainWindow::onSpinBoxRibbonPanelSpacingChanged);
    connect(spinboxRibbonPanelToolBtnIconSize,
            &QSpinBox::valueChanged,
            this,
            &MainWindow::onSpinBoxRibbonPanelToolBtnIconSizeChanged);
    connect(doublespinboxToolbuttonMaximumAspectRatio,
            &QDoubleSpinBox::valueChanged,
            this,
            &MainWindow::onDoubleSpinBoxToolbuttonMaximumAspectRatioChanged);
#else
    connect(spinboxRibbonTitleHeight,
            QOverload< int >::of(&QSpinBox::valueChanged),
            this,
            &MainWindow::onSpinBoxRibbonTitleHeightChanged);
    connect(spinboxRibbonTabHeight,
            QOverload< int >::of(&QSpinBox::valueChanged),
            this,
            &MainWindow::onSpinBoxRibbonTabHeightChanged);
    connect(spinboxRibbonCategoryHeight,
            QOverload< int >::of(&QSpinBox::valueChanged),
            this,
            &MainWindow::onSpinBoxRibbonCategoryHeightChanged);
    connect(spinboxRibbonPanelTtitleHeight,
            QOverload< int >::of(&QSpinBox::valueChanged),
            this,
            &MainWindow::onSpinBoxRibbonPanelTtitleHeightChanged);
    connect(spinboxRibbonPanelSpacing,
            QOverload< int >::of(&QSpinBox::valueChanged),
            this,
            &MainWindow::onSpinBoxRibbonPanelSpacingChanged);
    connect(spinboxRibbonPanelToolBtnIconSize,
            QOverload< int >::of(&QSpinBox::valueChanged),
            this,
            &MainWindow::onSpinBoxRibbonPanelToolBtnIconSizeChanged);
    connect(doublespinboxToolbuttonMaximumAspectRatio,
            QOverload< double >::of(&QDoubleSpinBox::valueChanged),
            this,
            &MainWindow::onDoubleSpinBoxToolbuttonMaximumAspectRatioChanged);
#endif
    panel->addLargeWidget(w);
    page->addPanel(panel);
}

/**
 * @brief 创建颜色分类页面内容
 * @param page 颜色分类页面指针
 *
 * cn:构建颜色相关页面，展示颜色选择控件的使用
 * en:Build color-related page, demonstrating the use of color selection controls
 */
void MainWindow::createCategoryColor(SARibbonCategory* page)
{
    SARibbonPanel* panel = page->addPanel(tr("color"));

    auto fpCreateBtn = [ this, panel ](const QColor& defaultColor = Qt::red) -> SARibbonColorToolButton* {
        SARibbonColorToolButton* colorButton = new SARibbonColorToolButton(panel);
        colorButton->setColor(defaultColor);
        colorButton->setupStandardColorMenu();
        this->connect(colorButton, &SARibbonColorToolButton::colorClicked, this, &MainWindow::onColorButtonColorClicked);
        return colorButton;
    };
    // No Icon No text
    SARibbonColorToolButton* colorButton = fpCreateBtn();
    colorButton->setObjectName("ColorFillToIcon-NoIconNoText");
    colorButton->setColorStyle(SARibbonColorToolButton::ColorFillToIcon);
    panel->addSmallWidget(colorButton);

    // No Icon have text
    colorButton = fpCreateBtn(Qt::blue);
    colorButton->setColorStyle(SARibbonColorToolButton::ColorFillToIcon);
    colorButton->setObjectName("ColorFillToIcon-NoIconHaveText");
    colorButton->setText("No Icon have text");
    panel->addSmallWidget(colorButton);

    // have Icon No text
    colorButton = fpCreateBtn(QColor());
    colorButton->setIcon(QIcon(":/icon/icon/long-text.svg"));
    panel->addSmallWidget(colorButton);

    // have Icon have text
    colorButton = fpCreateBtn(Qt::red);
    colorButton->setIcon(QIcon(":/icon/icon/long-text.svg"));
    colorButton->setText("have Icon have text");
    panel->addSmallWidget(colorButton);

    colorButton = fpCreateBtn(QColor());
    colorButton->setButtonType(SARibbonToolButton::LargeButton);
    colorButton->setObjectName("ColorFillToIcon-LargeColorButton");
    colorButton->setColorStyle(SARibbonColorToolButton::ColorFillToIcon);
    colorButton->setText("Large Color Button");
    panel->addLargeWidget(colorButton);

    colorButton = fpCreateBtn(QColor());
    colorButton->setButtonType(SARibbonToolButton::LargeButton);
    colorButton->setIcon(QIcon(":/icon/icon/long-text.svg"));
    colorButton->setObjectName("ColorUnderIcon-LargeColorButton");
    colorButton->setText("Large Color Button");
    panel->addLargeWidget(colorButton);
    //
    panel->addSeparator();
    SAColorPaletteGridWidget* pw = new SAColorPaletteGridWidget(SA::getStandardColorList(), panel);
    pw->setFactor({ 75, 120 });
    panel->addLargeWidget(pw);
}

/**
 * @brief 创建上下文标签
 *
 * cn:创建上下文相关的分类页面，这类页面通常在特定条件下才显示
 * en:Create context-related category pages, which are usually displayed under specific conditions
 */
void MainWindow::createContextCategory1()
{
    // 上下文标签
    SARibbonBar* ribbon = ribbonBar();
    mContextCategory    = ribbon->addContextCategory(tr("context"), QColor(), 1);

    SARibbonCategory* contextCategoryPage1 = mContextCategory->addCategoryPage(tr("Page1"));
    createContextCategoryPage1(contextCategoryPage1);

    SARibbonCategory* contextCategoryPage2 = mContextCategory->addCategoryPage(tr("Page2"));
    createContextCategoryPage2(contextCategoryPage2);
}

/**
 * @brief 创建第二个上下文标签
 *
 * cn:创建另一个上下文标签，演示多个上下文标签的管理
 * en:Create another context tag, demonstrating the management of multiple context tags
 */
void MainWindow::createContextCategory2()
{
    SARibbonBar* ribbon = ribbonBar();
    mContextCategory2   = ribbon->addContextCategory(("context2"), QColor(), 2);
    mContextCategory2->addCategoryPage(("context2 Page1"));
    mContextCategory2->addCategoryPage(("context2 Page2"));
}

/**
 * @brief 创建第一个上下文分类页面内容
 * @param page 第一个上下文分类页面指针
 *
 * cn:构建第一个上下文分类页面的内容，演示上下文页面的特有功能
 * en:Build content for the first context category page, demonstrating the unique features of context pages
 */
void MainWindow::createContextCategoryPage1(SARibbonCategory* page)
{
    SARibbonPanel* panel = page->addPanel(tr("show and hide test"));

    QAction* actionHideActions = createAction("hide action", ":/icon/icon/hidePanel.svg");
    actionHideActions->setCheckable(true);
    actionHideActions->setChecked(true);
    panel->addLargeAction(actionHideActions);
    connect(actionHideActions, &QAction::triggered, this, &MainWindow::onActionHideActionTriggered);

    mActionDisable = createAction(tr("Disable"), ":/icon/icon/enableTest.svg");

    mActionDisable->setDisabled(true);
    panel->addLargeAction(mActionDisable);
    connect(mActionDisable, &QAction::triggered, this, [ this ](bool b) {
        Q_UNUSED(b);
        mActionDisable->setDisabled(true);
    });

    mActionUnlock = createAction(tr("unlock"), ":/icon/icon/unlock.svg");
    mActionUnlock->setShortcut(QKeySequence(QLatin1String("Ctrl+E")));
    mActionUnlock->setShortcutContext(Qt::ApplicationShortcut);
    panel->addLargeAction(mActionUnlock);
    connect(mActionUnlock, &QAction::triggered, this, [ this ](bool b) {
        Q_UNUSED(b);
        mActionDisable->setEnabled(true);
        mActionDisable->setText(("Enabled"));
    });
    auto act = createAction(tr("1"), ":/icon/icon/unlock.svg");
    act->setToolTip(tr("very short string"));
    panel->addLargeAction(act);

    mActionSetTextTest = createAction("set text", ":/icon/icon/setText.svg");

    mActionSetTextTest->setCheckable(true);
    mActionSetTextTest->setShortcut(QKeySequence(QLatin1String("Ctrl+D")));
    mActionSetTextTest->setShortcutContext(Qt::ApplicationShortcut);
    panel->addLargeAction(mActionSetTextTest);

    connect(mActionSetTextTest, &QAction::toggled, this, [ this ](bool b) {
        if (b) {
            mActionSetTextTest->setText(QStringLiteral(u"setText测试"));
        } else {
            mActionSetTextTest->setText(QStringLiteral(u"set text"));
        }
    });
    // 隐藏panel
    mActionShowTest = createAction("show beside panel", ":/icon/icon/show.svg");

    mActionShowTest->setCheckable(true);
    panel->addLargeAction(mActionShowTest);

    mPanelVisbileExample = page->addPanel(tr("show/hide"));
    // 重复添加
    mPanelVisbileExample->addLargeAction(mActionSetTextTest);

    connect(mActionShowTest, &QAction::toggled, this, [ this ](bool b) {
        mPanelVisbileExample->setVisible(!b);
        if (b) {
            mActionShowTest->setText(tr("hide beside panel"));
        } else {
            mActionShowTest->setText(tr("show beside panel"));
        }
        ribbonBar()->repaint();
    });

    SARibbonPanel* panel3 = page->addPanel(("show/hide action test"));

    mActionHideAction2 = createAction("hide action 2", ":/icon/icon/action.svg");
    mActionHideAction4 = createAction("hide action 4", ":/icon/icon/action.svg");
    mActionHideAction2->setCheckable(true);
    mActionHideAction2->setChecked(true);
    mActionHideAction4->setCheckable(true);
    mActionHideAction4->setChecked(true);
    mActionHideShowTextAct2 = createAction("action2", ":/icon/icon/action2.svg");
    mActionHideShowTextAct3 = createAction("action3", ":/icon/icon/action3.svg");
    mActionHideShowTextAct4 = createAction("action4", ":/icon/icon/action4.svg");
    panel3->addLargeAction(mActionHideAction2);
    panel3->addLargeAction(mActionHideAction4);
    panel3->addSmallAction(mActionHideShowTextAct2);
    panel3->addSmallAction(mActionHideShowTextAct3);
    panel3->addSmallAction(mActionHideShowTextAct4);
    connect(mActionHideAction2, &QAction::triggered, this, [ this ](bool b) {
        if (b) {
            mActionHideShowTextAct2->setVisible(true);
            mActionHideAction2->setText(tr("hide action2"));
        } else {
            mActionHideShowTextAct2->setVisible(false);
            mActionHideAction2->setText(tr("show action2"));
        }
    });
    connect(mActionHideAction4, &QAction::triggered, this, [ this ](bool b) {
        if (b) {
            mActionHideShowTextAct4->setVisible(true);
            mActionHideAction4->setText(tr("hide action4"));
        } else {
            mActionHideShowTextAct4->setVisible(false);
            mActionHideAction4->setText(tr("show action4"));
        }
    });
    //
    SARibbonPanel* panel4                 = page->addPanel(("widget"));
    QSpinBox* spinbox                     = new QSpinBox(this);
    SARibbonCtrlContainer* ctrlContainer1 = new SARibbonCtrlContainer(panel4);
    ctrlContainer1->setContainerWidget(spinbox);
    ctrlContainer1->setText(tr("spinbox:"));
    ctrlContainer1->setEnableShowIcon(false);
    ctrlContainer1->setMaximumHeight(fontMetrics().lineSpacing() * 1.5);
    panel4->addMediumWidget(ctrlContainer1);

    QLineEdit* linedit                    = new QLineEdit(this);
    SARibbonCtrlContainer* ctrlContainer2 = new SARibbonCtrlContainer(panel4);
    ctrlContainer2->setContainerWidget(linedit);
    ctrlContainer2->setText(tr("linedit:"));
    ctrlContainer2->setEnableShowIcon(false);
    ctrlContainer2->setMaximumHeight(fontMetrics().lineSpacing() * 1.5);
    panel4->addMediumWidget(ctrlContainer2);
}

/**
 * @brief 创建第二个上下文分类页面内容
 * @param page 第二个上下文分类页面指针
 *
 * cn:构建第二个上下文分类页面的内容，展示特殊布局和控件组合
 * en:Build content for the second context category page, demonstrating special layouts and control combinations
 */
void MainWindow::createContextCategoryPage2(SARibbonCategory* page)
{
    SARibbonPanel* panel1 = page->addPanel(("删除CategoryPage测试"));

    QAction* actionDeleteThisCategory = createAction("delete this category", ":/icon/icon/delete.svg");
    panel1->addLargeAction(actionDeleteThisCategory);
    connect(actionDeleteThisCategory, &QAction::triggered, this, [ this, page ]() {
        this->ribbonBar()->removeCategory(page);
        page->deleteLater();
    });
    SARibbonPanel* panelLayout = page->addPanel(("特殊布局"));

    panelLayout->addAction(
        createAction("Large", ":/icon/icon/layout.svg", "@Large1"), QToolButton::InstantPopup, SARibbonPanelItem::Large);
    panelLayout->addAction(
        createAction("Small", ":/icon/icon/layout.svg", "@Small1"), QToolButton::InstantPopup, SARibbonPanelItem::Small);
    panelLayout->addAction(
        createAction("Small", ":/icon/icon/layout.svg", "@Small2"), QToolButton::InstantPopup, SARibbonPanelItem::Small);
    panelLayout->addSeparator();
    panelLayout->addAction(
        createAction("Small", ":/icon/icon/layout.svg", "@Small3"), QToolButton::InstantPopup, SARibbonPanelItem::Small);
    panelLayout->addAction(
        createAction("Small", ":/icon/icon/layout.svg", "@Small4"), QToolButton::InstantPopup, SARibbonPanelItem::Small);
    panelLayout->addAction(
        createAction("Small", ":/icon/icon/layout.svg", "@Small5"), QToolButton::InstantPopup, SARibbonPanelItem::Small);
    panelLayout->addAction(
        createAction("Small", ":/icon/icon/layout.svg", "@Small6"), QToolButton::InstantPopup, SARibbonPanelItem::Small);
    panelLayout->addSeparator();
    panelLayout->addAction(
        createAction("Large", ":/icon/icon/layout.svg", "@Large2"), QToolButton::InstantPopup, SARibbonPanelItem::Large);
    panelLayout->addAction(createAction("Medium", ":/icon/icon/layout.svg", "@Medium1"),
                           QToolButton::InstantPopup,
                           SARibbonPanelItem::Medium);
    panelLayout->addAction(createAction("Medium", ":/icon/icon/layout.svg", "@Medium2"),
                           QToolButton::InstantPopup,
                           SARibbonPanelItem::Medium);
    panelLayout->addAction(
        createAction("Small", ":/icon/icon/layout.svg", "@Small7"), QToolButton::InstantPopup, SARibbonPanelItem::Small);
    panelLayout->addAction(createAction("Medium", ":/icon/icon/layout.svg", "@Medium3"),
                           QToolButton::InstantPopup,
                           SARibbonPanelItem::Medium);
    panelLayout->addAction(
        createAction("Large", ":/icon/icon/layout.svg", "@Large3"), QToolButton::InstantPopup, SARibbonPanelItem::Large);
    panelLayout->addAction(createAction("Medium", ":/icon/icon/layout.svg", "@Medium4"),
                           QToolButton::InstantPopup,
                           SARibbonPanelItem::Medium);
    panelLayout->addAction(createAction("Medium", ":/icon/icon/layout.svg", "@Medium5"),
                           QToolButton::InstantPopup,
                           SARibbonPanelItem::Medium);
    panelLayout->addAction(
        createAction("Large", ":/icon/icon/layout.svg", "@Large4"), QToolButton::InstantPopup, SARibbonPanelItem::Large);
    panelLayout->addSeparator();
    panelLayout->addAction(createAction("Medium", ":/icon/icon/layout.svg", "@Medium6"),
                           QToolButton::InstantPopup,
                           SARibbonPanelItem::Medium);
    panelLayout->addAction(
        createAction("Large", ":/icon/icon/layout.svg", "@Large5"), QToolButton::InstantPopup, SARibbonPanelItem::Large);
    panelLayout->addAction(createAction("Medium", ":/icon/icon/layout.svg", "@Medium7"),
                           QToolButton::InstantPopup,
                           SARibbonPanelItem::Medium);
    panelLayout->addAction(
        createAction("Small", ":/icon/icon/layout.svg", "@Small8"), QToolButton::InstantPopup, SARibbonPanelItem::Small);
}

/**
 * @brief 创建快速访问工具栏
 *
 * cn:构建快速访问工具栏，放置常用的操作按钮
 * en:Build quick access toolbar, placing commonly used operation buttons
 */
void MainWindow::createQuickAccessBar()
{
    SARibbonQuickAccessBar* quickAccessBar = ribbonBar()->quickAccessBar();

    quickAccessBar->addAction(createAction("save", ":/icon/icon/save.svg", "save-quickbar"));
    quickAccessBar->addSeparator();

    QAction* actionUndo = createAction("undo", ":/icon/icon/undo.svg");
    actionUndo->setShortcut(QKeySequence("Ctrl+Shift+z"));
    actionUndo->setShortcutContext(Qt::ApplicationShortcut);
    quickAccessBar->addAction(actionUndo);

    QAction* actionRedo = createAction("redo", ":/icon/icon/redo.svg");
    actionRedo->setShortcut(QKeySequence("Ctrl+z"));
    actionRedo->setShortcutContext(Qt::ApplicationShortcut);
    quickAccessBar->addAction(actionRedo);

    connect(actionUndo, &QAction::triggered, this, [ this ]() { mTextedit->append("undo"); });
    connect(actionRedo, &QAction::triggered, this, [ this ]() { mTextedit->append("redo"); });

    quickAccessBar->addSeparator();

    QMenu* m1 = new QMenu("Presentation File 1", this);
    m1->setIcon(QIcon(":/icon/icon/presentationFile.svg"));
    for (int i = 0; i < 10; ++i) {
        m1->addAction(createAction(QString("file%1 - 1").arg(i + 1), ":/icon/icon/file.svg"));
    }
    quickAccessBar->addMenuAction(m1->menuAction());

    QMenu* m2 = new QMenu("Presentation File 2", this);
    m2->setIcon(QIcon(":/icon/icon/presentationFile.svg"));
    for (int i = 0; i < 10; ++i) {
        m2->addAction(createAction(QString("file%1 - 2").arg(i + 1), ":/icon/icon/file.svg"));
    }
    quickAccessBar->addMenuAction(m2, QToolButton::MenuButtonPopup);

    QAction* customize = createAction("customize", ":/icon/icon/customize0.svg", "customize2");
    quickAccessBar->addAction(customize);
    connect(customize, &QAction::triggered, this, &MainWindow::onActionCustomizeTriggered);

    QAction* actionCustomizeAndSave = createAction("customize and save", ":/icon/icon/customize.svg");
    quickAccessBar->addAction(actionCustomizeAndSave);
    connect(actionCustomizeAndSave, &QAction::triggered, this, &MainWindow::onActionCustomizeAndSaveTriggered);

    QAction* actionCustomizeAndSaveWithApply = createAction("customize and save with apply", ":/icon/icon/customize.svg");
    quickAccessBar->addAction(actionCustomizeAndSaveWithApply);
    connect(actionCustomizeAndSaveWithApply, &QAction::triggered, this, &MainWindow::onActionCustomizeAndSaveWithApplyTriggered);

    mSearchEditor = new QLineEdit(this);
    mSearchEditor->setMinimumWidth(150);
    mSearchEditor->setPlaceholderText("Search");
    quickAccessBar->addWidget(mSearchEditor);
    connect(mSearchEditor, &QLineEdit::editingFinished, this, [ this ]() {
        this->mTextedit->append(this->mSearchEditor->text());
    });
}

/**
 * @brief 创建RightButtonGroup
 *
 * cn:RightButtonGroup实在ribbonbar右边的工具栏，可以放置一些快捷图标
 * en:RightButtonGroup is a toolbar on the right side of the ribbonbar where you can place some shortcut icons
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
 *
 * cn:在系统窗口按钮旁边添加自定义工具按钮
 * en:Add custom tool buttons next to system window buttons
 */
void MainWindow::createWindowButtonGroupBar()
{
    SARibbonSystemButtonBar* wbar = windowButtonBar();
    if (!wbar) {
        return;
    }
    QAction* actionLogin = new QAction(QIcon(), tr("Login"), this);
    QAction* actionHelp  = createAction(tr("help"), ":/icon/icon/help.svg");
    connect(actionLogin, &QAction::triggered, this, [ this ]() { this->mTextedit->append("Login triggered"); });
    connect(actionHelp, &QAction::triggered, this, &MainWindow::onActionHelpTriggered);
    wbar->addAction(actionLogin);
    wbar->addAction(actionHelp);
}

/**
 * @brief 创建ActionsManager（Create ActionsManager）
 *
 * cn:创建ActionsManager，实现actions的管理以及SARibbonBar的自定义
 * en:Create ActionsManager to manage actions and customize SARibbonBar
 *
 *
 */
void MainWindow::createActionsManager()
{
    // 添加其他的action，这些action并不在ribbon管理范围，主要用于SARibbonCustomizeWidget自定义用
    createOtherActions();
    mTagForActionText = SARibbonActionsManager::UserDefineActionTag + 1;
    mTagForActionIcon = SARibbonActionsManager::UserDefineActionTag + 2;

    mActionsManager = new SARibbonActionsManager(ribbonBar());  // 申明过程已经自动注册所有action

    // 以下注册特别的action
    mActionsManager->registeAction(mOtherAction1, SARibbonActionsManager::CommonlyUsedActionTag);
    mActionsManager->registeAction(mOtherAction3, SARibbonActionsManager::CommonlyUsedActionTag);
    mActionsManager->registeAction(mOtherAction5, SARibbonActionsManager::CommonlyUsedActionTag);
    mActionsManager->registeAction(mOtherActionIcon1, SARibbonActionsManager::CommonlyUsedActionTag);

    mActionsManager->registeAction(mOtherAction1, mTagForActionText);
    mActionsManager->registeAction(mOtherAction2, mTagForActionText);
    mActionsManager->registeAction(mOtherAction3, mTagForActionText);
    mActionsManager->registeAction(mOtherAction4, mTagForActionText);
    mActionsManager->registeAction(mOtherAction5, mTagForActionText);

    mActionsManager->registeAction(mOtherActionIcon1, mTagForActionIcon);

    mActionsManager->setTagName(SARibbonActionsManager::CommonlyUsedActionTag, tr("in common use"));  //
    mActionsManager->setTagName(mTagForActionText, tr("no icon action"));
    mActionsManager->setTagName(mTagForActionIcon, tr("have icon action"));
}

/**
 * @brief 创建动作
 * @param text 动作文本
 * @param iconurl 图标路径
 * @param objName 对象名称
 * @return 创建的动作指针
 *
 * cn:创建带有文本、图标和对象名称的动作
 * en:Create an action with text, icon, and object name
 */
QAction* MainWindow::createAction(const QString& text, const QString& iconurl, const QString& objName)
{
    QAction* act = new QAction(this);
    act->setText(text);
    act->setIcon(QIcon(iconurl));
    act->setObjectName(objName);
    return act;
}

/**
 * @brief 创建动作（重载版本）
 * @param text 动作文本
 * @param iconurl 图标路径
 * @return 创建的动作指针
 *
 * cn:创建带有文本和图标的动作，对象名称使用文本内容
 * en:Create an action with text and icon, using the text content as the object name
 */
QAction* MainWindow::createAction(const QString& text, const QString& iconurl)
{
    QAction* act = new QAction(this);
    act->setText(text);
    act->setIcon(QIcon(iconurl));
    act->setObjectName(text);
    return act;
}

/**
 * @brief 菜单按钮弹出可检查测试
 * @param b 是否选中
 *
 * cn:测试菜单按钮弹出模式下的可检查功能
 * en:Test the checkable function in menu button popup mode
 */
void MainWindow::onMenuButtonPopupCheckableTest(bool b)
{
    mTextedit->append(QString("MenuButtonPopupCheckableTest : %1").arg(b));
}

/**
 * @brief 即时弹出可检查测试
 * @param b 是否选中
 *
 * cn:测试即时弹出模式下的可检查功能
 * en:Test the checkable function in instant popup mode
 */
void MainWindow::onInstantPopupCheckableTest(bool b)
{
    mTextedit->append(QString("InstantPopupCheckableTest : %1").arg(b));
}

/**
 * @brief 延迟弹出可检查测试
 * @param b 是否选中
 *
 * cn:测试延迟弹出模式下的可检查功能
 * en:Test the checkable function in delayed popup mode
 */
void MainWindow::onDelayedPopupCheckableTest(bool b)
{
    mTextedit->append(QString("DelayedPopupCheckableTest : %1").arg(b));
}

/**
 * @brief 菜单按钮弹出可检查触发
 * @param b 是否选中
 *
 * cn:处理菜单按钮弹出模式下可检查动作的触发事件
 * en:Handle trigger events for checkable actions in menu button popup mode
 */
void MainWindow::onMenuButtonPopupCheckabletriggered(bool b)
{
    mTextedit->append(QString("MenuButtonPopupCheckabletriggered : %1").arg(b));
}

/**
 * @brief 即时弹出可检查触发
 * @param b 是否选中
 *
 * cn:处理即时弹出模式下可检查动作的触发事件
 * en:Handle trigger events for checkable actions in instant popup mode
 */
void MainWindow::onInstantPopupCheckabletriggered(bool b)
{
    mTextedit->append(QString("InstantPopupCheckabletriggered : %1").arg(b));
}

/**
 * @brief 延迟弹出可检查触发
 * @param b 是否选中
 *
 * cn:处理延迟弹出模式下可检查动作的触发事件，同时演示文本换行功能
 * en:Handle trigger events for checkable actions in delayed popup mode, while demonstrating text wrapping functionality
 */
void MainWindow::onDelayedPopupCheckabletriggered(bool b)
{
    Q_UNUSED(b);
    mTextedit->append(tr("The SARibbonToolButton::setPopupMode(QToolButton::DelayedPopup) method "
                         "can be used to set the menu pop-up method to delayed pop-up. "
                         "This also demonstrates manually setting text wrapping"));
    // cn:使用SARibbonToolButton::setPopupMode(QToolButton::DelayedPopup)方法可以设置菜单弹出方式为延迟弹出，这里也演示了手动设置文本的换行
}
