#include "mainwindow.h"
#include "ui_mainwindow.h"
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

/**
 * \if ENGLISH
 * @brief Macro for starting performance timing measurement
 * @details This macro creates a QElapsedTimer and starts it for measuring code execution time.
 *          It's useful for profiling and debugging performance issues.
 * \endif
 *
 * \if CHINESE
 * @brief 用于开始性能计时的宏
 * @details 此宏创建一个QElapsedTimer并启动它，用于测量代码执行时间。
 *          对于性能分析和调试非常有用。
 * \endif
 */
#define PRINT_COST_START()                                                                                             \
    QElapsedTimer __TMP_COST;                                                                                          \
    __TMP_COST.start();                                                                                                \
    int __TMP_LASTTIMES = 0

/**
 * \if ENGLISH
 * @brief Macro for printing elapsed time cost
 * @param STR Description string for the timing point
 * @details This macro prints the elapsed time since the last timing point and the total elapsed time.
 *          It outputs to both the debug console and the UI text browser.
 * \endif
 *
 * \if CHINESE
 * @brief 用于打印已用时间的宏
 * @param STR 计时点的描述字符串
 * @details 此宏打印自上一个计时点以来的已用时间和总已用时间。
 *          它同时输出到调试控制台和UI文本浏览器。
 * \endif
 */
#define PRINT_COST(STR)                                                                                                   \
    do {                                                                                                                  \
        int ___TMP_INT = __TMP_COST.elapsed();                                                                            \
        qDebug() << STR << " cost " << ___TMP_INT - __TMP_LASTTIMES << " ms (" << ___TMP_INT << ")";                      \
        ui->textBrowser->append(QString("%1 cost %2 ms(%3)").arg(STR).arg(___TMP_INT - __TMP_LASTTIMES).arg(___TMP_INT)); \
        __TMP_LASTTIMES = ___TMP_INT;                                                                                     \
    } while (0)

/**
 * \if ENGLISH
 * @brief Constructor, initializes the main window and Ribbon interface
 * @param parent Parent window pointer
 * \endif
 *
 * \if CHINESE
 * @brief 构造函数，初始化主窗口和Ribbon界面
 * @param parent 父窗口指针
 * \endif
 */
MainWindow::MainWindow(QWidget* parent) : SARibbonMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initUI();
}

/**
 * \if ENGLISH
 * @brief Destructor
 * @details Cleans up the UI resources
 * \endif
 *
 * \if CHINESE
 * @brief 析构函数
 * @details 清理UI资源
 * \endif
 */
MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * \if ENGLISH
 * @brief Initialize the user interface
 * @details This function sets up the main window's Ribbon interface, including:
 *          - Creating the Ribbon bar with categories and panels
 *          - Setting up the Application button
 *          - Creating context categories
 *          - Setting up the quick access bar and right button group
 *          - Connecting signals and slots for various UI controls
 * \endif
 *
 * \if CHINESE
 * @brief 初始化用户界面
 * @details 此函数设置主窗口的Ribbon界面，包括：
 *          - 创建带有分类和面板的Ribbon栏
 *          - 设置应用程序按钮
 *          - 创建上下文分类
 *          - 设置快速访问栏和右侧按钮组
 *          - 连接各种UI控件的信号和槽
 * \endif
 */
void MainWindow::initUI()
{
    PRINT_COST_START();
    setWindowTitle(("ribbon mainwindow test[*]"));
    setWindowModified(true);

    SARibbonBar* ribbonBar = this->ribbonBar();
    setContentsMargins(1, 0, 1, 0);
    ribbonBar->setContentsMargins(4, 0, 4, 0);

    connect(ribbonBar, &SARibbonBar::actionTriggered, this, [ this ](QAction* triggeredAction) {
        ui->textBrowser->append(QString("action object name=%1 triggered").arg(triggeredAction->objectName()));
    });

    createRibbonApplicationButton();

    SARibbonCategory* categoryMain = ribbonBar->addCategoryPage(tr("&Main"));
    categoryMain->setObjectName(("categoryMain"));
    createCategoryMain(categoryMain);
    PRINT_COST("new main page");

    SARibbonCategory* categoryOther = new SARibbonCategory();
    categoryOther->setCategoryName(tr("Other"));
    categoryOther->setObjectName(("categoryOther"));
    createCategoryOther(categoryOther);
    ribbonBar->addCategoryPage(categoryOther);
    PRINT_COST("add other page");

    SARibbonCategory* categoryDelete = new SARibbonCategory();
    categoryDelete->setCategoryName(("Delete"));
    categoryDelete->setObjectName(("categoryDelete"));
    ribbonBar->addCategoryPage(categoryDelete);
    createCategoryDelete(categoryDelete);
    PRINT_COST("add category delete page");

    SARibbonCategory* categorySize = new SARibbonCategory();
    categorySize->setCategoryName(("Size(example long category)"));
    categorySize->setObjectName(("categorySize"));
    ribbonBar->addCategoryPage(categorySize);
    createCategorySize(categorySize);
    PRINT_COST("add category size page");

    SARibbonCategory* categoryColor = new SARibbonCategory();
    categoryColor->setCategoryName(("Color"));
    categoryColor->setObjectName(("categoryColor"));
    ribbonBar->addCategoryPage(categoryColor);
    createCategoryColor(categoryColor);
    PRINT_COST("add category color page");

    createContextCategory1();
    PRINT_COST("add context1 category page");

    createContextCategory2();
    PRINT_COST("add context2 category page");

    createQuickAccessBar();
    PRINT_COST("add quick access bar");

    createRightButtonGroup();
    PRINT_COST("add right bar");

    createWindowButtonGroupBar();

    createActionsManager();

    setMinimumWidth(500);
    setWindowIcon(QIcon(":/icon/icon/SA.svg"));

#if QT_VERSION_MAJOR >= 6
    connect(ui->spinBoxRibbonTitleHeight, &QSpinBox::valueChanged, this, &MainWindow::onSpinBoxRibbonTitleHeightChanged);
    connect(ui->spinBoxRibbonTabHeight, &QSpinBox::valueChanged, this, &MainWindow::onSpinBoxRibbonTabHeightChanged);
    connect(ui->spinBoxRibbonCategoryHeight, &QSpinBox::valueChanged, this, &MainWindow::onSpinBoxRibbonCategoryHeightChanged);
    connect(ui->spinBoxPanelTtitleHeight, &QSpinBox::valueChanged, this, &MainWindow::onSpinBoxRibbonPanelTtitleHeightChanged);
    connect(ui->spinBoxPanelSpacing, &QSpinBox::valueChanged, this, &MainWindow::onSpinBoxRibbonPanelSpacingChanged);
    connect(ui->spinBoxPanelSmallIconSize, &QSpinBox::valueChanged, this, &MainWindow::onSpinBoxPanelSmallIconSizeChanged);
    connect(ui->spinBoxPanelLargeIconSize, &QSpinBox::valueChanged, this, &MainWindow::onSpinBoxPanelLargeIconSizeChanged);
    connect(ui->doubleSpinBoxToolbuttonMaximumAspectRatio,
            &QDoubleSpinBox::valueChanged,
            this,
            &MainWindow::onDoubleSpinBoxToolbuttonMaximumAspectRatioChanged);
#else
    connect(ui->spinBoxRibbonTitleHeight,
            QOverload< int >::of(&QSpinBox::valueChanged),
            this,
            &MainWindow::onSpinBoxRibbonTitleHeightChanged);
    connect(ui->spinBoxRibbonTabHeight,
            QOverload< int >::of(&QSpinBox::valueChanged),
            this,
            &MainWindow::onSpinBoxRibbonTabHeightChanged);
    connect(ui->spinBoxRibbonCategoryHeight,
            QOverload< int >::of(&QSpinBox::valueChanged),
            this,
            &MainWindow::onSpinBoxRibbonCategoryHeightChanged);
    connect(ui->spinBoxPanelTtitleHeight,
            QOverload< int >::of(&QSpinBox::valueChanged),
            this,
            &MainWindow::onSpinBoxRibbonPanelTtitleHeightChanged);
    connect(ui->spinBoxPanelSpacing,
            QOverload< int >::of(&QSpinBox::valueChanged),
            this,
            &MainWindow::onSpinBoxRibbonPanelSpacingChanged);
    connect(ui->spinBoxPanelSmallIconSize,
            QOverload< int >::of(&QSpinBox::valueChanged),
            this,
            &MainWindow::onSpinBoxPanelSmallIconSizeChanged);
    connect(ui->spinBoxPanelLargeIconSize,
            QOverload< int >::of(&QSpinBox::valueChanged),
            this,
            &MainWindow::onSpinBoxPanelLargeIconSizeChanged);
    connect(ui->doubleSpinBoxToolbuttonMaximumAspectRatio,
            QOverload< double >::of(&QDoubleSpinBox::valueChanged),
            this,
            &MainWindow::onDoubleSpinBoxToolbuttonMaximumAspectRatioChanged);
#endif
    connect(ribbonBar, &SARibbonBar::currentRibbonTabChanged, this, [ this ](int tabIndex) {
        ui->textBrowser->append(QString("SARibbonBar::currentRibbonTabChanged(%1)").arg(tabIndex));
    });

    ui->spinBoxRibbonTitleHeight->setValue(this->ribbonBar()->titleBarHeight());
    ui->spinBoxRibbonTabHeight->setValue(this->ribbonBar()->tabBarHeight());
    ui->spinBoxRibbonCategoryHeight->setValue(this->ribbonBar()->categoryHeight());
    ui->spinBoxPanelTtitleHeight->setValue(this->ribbonBar()->panelTitleHeight());
    ui->spinBoxPanelSpacing->setValue(this->ribbonBar()->panelSpacing());
    ui->spinBoxPanelSmallIconSize->setValue(this->ribbonBar()->panelSmallIconSize().width());
    ui->spinBoxPanelLargeIconSize->setValue(this->ribbonBar()->panelLargeIconSize().width());
    ui->doubleSpinBoxToolbuttonMaximumAspectRatio->setValue(this->ribbonBar()->buttonMaximumAspectRatio());

    mChangeTitleBkColorTimer.setInterval(5000);
    mChangeTitleBkColorTimer.start();
    connect(&mChangeTitleBkColorTimer, &QTimer::timeout, this, &MainWindow::onChangedTitleTimeout);
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
    QTimer::singleShot(0, this, &QWidget::showMaximized);
#else
    showMaximized();
#endif
}

/**
 * \if ENGLISH
 * @brief Create the Application Button for the Ribbon interface
 * @details The Application Button is located in the upper left corner of the Ribbon interface.
 *          It typically displays file-related operations such as New, Open, Save, etc.
 *          By default, SARibbonBar creates a SARibbonApplicationButton. Users can create
 *          their own application button and set it via SARibbonBar::setApplicationButton().
 *          Passing a nullptr will remove the application button.
 * \endif
 *
 * \if CHINESE
 * @brief 创建Ribbon界面的应用程序按钮
 * @details 应用程序按钮位于Ribbon界面的左上角，通常用于显示文件相关操作，如新建、打开、保存等。
 *          默认情况下，SARibbonBar会创建一个SARibbonApplicationButton。用户可以创建
 *          自己的应用程序按钮，并通过SARibbonBar::setApplicationButton()设置。
 *          传入nullptr将移除应用程序按钮。
 * \endif
 */
void MainWindow::createRibbonApplicationButton()
{
    SARibbonBar* currentRibbonBar = ribbonBar();
    if (!currentRibbonBar) {
        return;
    }
    QAbstractButton* applicationButton = currentRibbonBar->applicationButton();
    if (!applicationButton) {
        applicationButton = new SARibbonApplicationButton(this);
        currentRibbonBar->setApplicationButton(applicationButton);
    }
    applicationButton->setText(("&File"));
#ifdef USE_APPLICATION_NORMAL_MENU
    if (!mMenuApplicationBtn) {
        mMenuApplicationBtn = new SARibbonMenu(this);
        mMenuApplicationBtn->addAction(createAction("test1", ":/icon/icon/action.svg"));
        mMenuApplicationBtn->addAction(createAction("test2", ":/icon/icon/action2.svg"));
        mMenuApplicationBtn->addAction(createAction("test3", ":/icon/icon/action3.svg"));
        mMenuApplicationBtn->addAction(createAction("test4", ":/icon/icon/action4.svg"));
    }
    SARibbonApplicationButton* ribbonAppBtn = qobject_cast< SARibbonApplicationButton* >(applicationButton);
    if (!ribbonAppBtn) {
        return;
    }
    ribbonAppBtn->setMenu(mMenuApplicationBtn);
#else
    mAppWidget = new ApplicationWidget(this);
    mAppWidget->hide();
    connect(applicationButton, &QAbstractButton::clicked, this, [ this ](bool checked) {
        Q_UNUSED(checked);
        this->mAppWidget->show();
    });
#endif
}

/**
 * \if ENGLISH
 * @brief Show or hide the Context Category
 * @param visible Whether to show the context category
 * @details Context Category is a special tab that is usually displayed for specific context environments,
 *          such as showing related operations when an object is selected.
 * \endif
 *
 * \if CHINESE
 * @brief 显示或隐藏上下文分类
 * @param visible 是否显示上下文分类
 * @details 上下文分类是一种特殊的标签页，通常用于特定上下文环境，比如当选择某个对象时才显示相关操作。
 * \endif
 */
void MainWindow::onShowContextCategory(bool visible)
{
    if (mContextCategory == nullptr) {
        createContextCategory1();
    }
    if (visible) {
        this->ribbonBar()->showContextCategory(mContextCategory);
    } else {
        this->ribbonBar()->hideContextCategory(mContextCategory);
    }
}

/**
 * \if ENGLISH
 * @brief Handle Ribbon style button click
 * @param styleId The style ID corresponding to the clicked button
 * @details SARibbon supports multiple styles, including combinations of loose/compact and two-row/three-row modes.
 *          Users can switch between styles as needed.
 * \endif
 *
 * \if CHINESE
 * @brief 处理Ribbon样式按钮点击
 * @param styleId 点击按钮对应的样式ID
 * @details SARibbon支持多种样式，包括宽松/紧凑和两行/三行模式的组合，用户可以根据需要切换。
 * \endif
 */
void MainWindow::onStyleClicked(int styleId)
{
    SARibbonBar::RibbonStyles ribbonStyle = static_cast< SARibbonBar::RibbonStyles >(styleId);
    ribbonBar()->setRibbonStyle(ribbonStyle);
    mActionWordWrap->setChecked(ribbonBar()->isEnableWordWrap());
    switch (ribbonStyle) {
    case SARibbonBar::RibbonStyleLooseThreeRow:
        ui->textBrowser->append(
            tr("\nThe text display of the \"LooseThreeRow\" style is word wrap and also displays the title bar. "
               "You can also control whether the button is line breaking through SARibbonBar::setEnableWordWrap,"
               "and whether the title bar is displayed through SARibbonBar::setEnableShowPanelTitle"));
        ui->textBrowser->append(tr("ribbonBar()->setRibbonStyle(SARibbonBar::RibbonStyleLooseThreeRow);"));
        break;
    case SARibbonBar::RibbonStyleLooseTwoRow:
        ui->textBrowser->append(
            tr("\nThe text display of the \"LooseTwoRow\" style is not word wrap and also displays the title bar. "
               "You can also control whether the button is line breaking through SARibbonBar::setEnableWordWrap,"
               "and whether the title bar is displayed through SARibbonBar::setEnableShowPanelTitle"));
        ui->textBrowser->append(tr("ribbonBar()->setRibbonStyle(SARibbonBar::RibbonStyleLooseTwoRow);"));
        break;
    case SARibbonBar::RibbonStyleCompactThreeRow:
        ui->textBrowser->append(
            tr("\nThe text display of the \"LooseThreeRow\" style is word wrap and not displays the title bar. "
               "You can also control whether the button is line breaking through SARibbonBar::setEnableWordWrap,"
               "and whether the title bar is displayed through SARibbonBar::setEnableShowPanelTitle"));
        ui->textBrowser->append(tr("ribbonBar()->setRibbonStyle(SARibbonBar::RibbonStyleCompactThreeRow);"));
        break;
    case SARibbonBar::RibbonStyleCompactTwoRow:
        ui->textBrowser->append(
            tr("\nThe text display of the \"CompactTwoRow\" style is not word wrap and not displays the title bar. "
               "You can also control whether the button is line breaking through SARibbonBar::setEnableWordWrap,"
               "and whether the title bar is displayed through SARibbonBar::setEnableShowPanelTitle"));
        ui->textBrowser->append(tr("ribbonBar()->setRibbonStyle(SARibbonBar::RibbonStyleCompactTwoRow);"));
        break;
    default:
        break;
    }
}

/**
 * \if ENGLISH
 * @brief Handle customize action trigger
 * @details This demonstrates how to use SARibbonCustomizeWidget to implement interface customization.
 *          The customize widget allows users to modify the ribbon layout.
 * \endif
 *
 * \if CHINESE
 * @brief 处理自定义动作触发
 * @details 这里演示了如何使用SARibbonCustomizeWidget来实现界面的自定义。
 *          自定义窗口允许用户修改Ribbon布局。
 * \endif
 */
void MainWindow::onActionCustomizeTriggered()
{
    if (nullptr == mWidgetForCustomize) {
        mWidgetForCustomize =
            new SARibbonCustomizeWidget(this, this, Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint | Qt::Dialog);
        mWidgetForCustomize->setWindowModality(Qt::ApplicationModal);
        mWidgetForCustomize->setAttribute(Qt::WA_ShowModal, true);
        mWidgetForCustomize->setupActionsManager(mActionsManager);
    }
    mWidgetForCustomize->show();
    mWidgetForCustomize->applys();
}

/**
 * \if ENGLISH
 * @brief Handle customize and save action trigger
 * @details This demonstrates how to use SARibbonCustomizeDialog to implement interface customization,
 *          and also demonstrates how to save the customized interface as an XML file.
 * \endif
 *
 * \if CHINESE
 * @brief 处理自定义并保存动作触发
 * @details 这里演示了如何使用SARibbonCustomizeDialog来实现界面的自定义，
 *          同时演示了如何把自定义的界面保存为XML文件。
 * \endif
 */
void MainWindow::onActionCustomizeAndSaveTriggered()
{
    SARibbonCustomizeDialog customizeDialog(this);
    customizeDialog.setupActionsManager(mActionsManager);
    if (!mHasApplyCustomizeXmlFile) {
        auto userResponse = QMessageBox::question(
            this, tr("question"), tr("Apply the last modification?\nIf not, local data will be reset"));
        if (userResponse == QMessageBox::Yes) {
            onActionLoadCustomizeXmlFileTriggered();
            return;
        } else {
            QFile::remove("customize.xml");
            customizeDialog.clear();
            mHasApplyCustomizeXmlFile = true;
        }
    }

    customizeDialog.fromXml("customize.xml");
    if (SARibbonCustomizeDialog::Accepted == customizeDialog.exec()) {
        if (customizeDialog.isCached())
            customizeDialog.applys();

        if (!customizeDialog.isApplied()) {
            ui->textBrowser->append("no change to save");
            return;
        }

        QByteArray xmlContent;
        QXmlStreamWriter xmlWriter(&xmlContent);
        xmlWriter.setAutoFormatting(true);
        xmlWriter.setAutoFormattingIndent(2);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        xmlWriter.setCodec("utf-8");
#endif
        xmlWriter.writeStartDocument();
        bool writeSuccess = customizeDialog.toXml(&xmlWriter);
        xmlWriter.writeEndDocument();
        if (writeSuccess) {
            QFile xmlFile("customize.xml");
            if (xmlFile.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate)) {
                QTextStream textStream(&xmlFile);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                textStream.setCodec("utf-8");
#endif
                textStream << xmlContent;
                textStream.flush();
            }
            ui->textBrowser->append("write xml:");
            ui->textBrowser->append(xmlContent);
        }
    }
}

/**
 * \if ENGLISH
 * @brief Handle customize and save with apply action trigger
 * @details This demonstrates how to embed SARibbonCustomizeWidget into a QDialog to implement interface customization.
 *          This approach allows better integration with your own software settings page.
 * \endif
 *
 * \if CHINESE
 * @brief 处理自定义并保存（带应用）动作触发
 * @details 这里演示了如何将SARibbonCustomizeWidget嵌入到QDialog中实现界面的自定义，
 *          这样可以让SARibbonCustomizeWidget更好地和自己的软件设置页面结合。
 * \endif
 */
void MainWindow::onActionCustomizeAndSaveWithApplyTriggered()
{
    if (!mHasApplyCustomizeXmlFile) {
        auto userResponse = QMessageBox::question(
            this, tr("question"), tr("Apply the last modification?\nIf not, local data will be reset"));
        if (userResponse == QMessageBox::Yes) {
            onActionLoadCustomizeXmlFileTriggered();
            return;
        } else {
            QFile::remove("customize.xml");
            mHasApplyCustomizeXmlFile = true;
        }
    }

    QDialog customizeDialog;
    QVBoxLayout* dialogLayout = new QVBoxLayout;
    customizeDialog.setLayout(dialogLayout);
    SARibbonCustomizeWidget* customizeWidget = new SARibbonCustomizeWidget(this, &customizeDialog);
    customizeWidget->setupActionsManager(mActionsManager);

    dialogLayout->addWidget(customizeWidget, 1);

    QDialogButtonBox* buttonBox =
        new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel | QDialogButtonBox::Apply);

    dialogLayout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, &customizeDialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &customizeDialog, &QDialog::reject);
    connect(buttonBox, &QDialogButtonBox::clicked, &customizeDialog, [ = ](QAbstractButton* clickedButton) {
        auto buttonRole = buttonBox->buttonRole(clickedButton);
        if (buttonRole == QDialogButtonBox::ApplyRole) {
            if (customizeWidget->isCached()) {
                customizeWidget->applys();
                ui->textBrowser->append("change applied");
            } else {
                ui->textBrowser->append("no change to apply");
            }
        }
    });

    customizeWidget->fromXml("customize.xml");
    if (QDialog::Accepted == customizeDialog.exec()) {
        if (customizeWidget->isCached())
            customizeWidget->applys();

        if (!customizeWidget->isApplied()) {
            ui->textBrowser->append("no change to save");
            return;
        }

        QByteArray xmlContent;
        QXmlStreamWriter xmlWriter(&xmlContent);
        xmlWriter.setAutoFormatting(true);
        xmlWriter.setAutoFormattingIndent(2);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        xmlWriter.setCodec("utf-8");
#endif
        xmlWriter.writeStartDocument();
        bool writeSuccess = customizeWidget->toXml(&xmlWriter);
        xmlWriter.writeEndDocument();
        if (writeSuccess) {
            QFile xmlFile("customize.xml");
            if (xmlFile.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate)) {
                QTextStream textStream(&xmlFile);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                textStream.setCodec("utf-8");
#endif
                textStream << xmlContent;
                textStream.flush();
            }
            ui->textBrowser->append("write xml:");
            ui->textBrowser->append(xmlContent);
        }
    } else {
        customizeWidget->clear();
        ui->textBrowser->append("all changes clear, the applied changes will take no effect afer restart");
    }
}

/**
 * \if ENGLISH
 * @brief Handle help action trigger
 * @details Display SARibbon library version information and author contact information.
 * \endif
 *
 * \if CHINESE
 * @brief 处理帮助动作触发
 * @details 显示SARibbon库的版本信息和作者联系方式。
 * \endif
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
 * \if ENGLISH
 * @brief Handle remove application button action trigger
 * @param removeButton Whether to remove the application button
 * @details Demonstrate how to dynamically add or remove the Application Button.
 * \endif
 *
 * \if CHINESE
 * @brief 处理移除应用程序按钮动作触发
 * @param removeButton 是否移除应用程序按钮
 * @details 演示如何动态添加或移除ApplicationButton。
 * \endif
 */
void MainWindow::onActionRemoveAppBtnTriggered(bool removeButton)
{
    if (removeButton) {
        ribbonBar()->setApplicationButton(nullptr);
    } else {
        SARibbonApplicationButton* newAppButton = new SARibbonApplicationButton();
        newAppButton->setText(tr("&File"));
        this->ribbonBar()->setApplicationButton(newAppButton);
        createRibbonApplicationButton();
    }
}

/**
 * \if ENGLISH
 * @brief Handle use QSS action trigger
 * @details Demonstrate how to customize Ribbon interface styles through QSS files.
 * \endif
 *
 * \if CHINESE
 * @brief 处理使用QSS动作触发
 * @details 演示如何通过QSS文件自定义Ribbon界面样式。
 * \endif
 */
void MainWindow::onActionUseQssTriggered()
{
    QFile qssFile("ribbon.qss");
    if (!qssFile.exists()) {
        QString selectedPath = QFileDialog::getOpenFileName(this, tr("select qss file"));
        if (selectedPath.isEmpty()) {
            return;
        }
        qssFile.setFileName(selectedPath);
    }
    if (!qssFile.open(QIODevice::ReadWrite)) {
        return;
    }
    QString qssContent(qssFile.readAll());
    ui->textBrowser->setText(qssContent);
    this->ribbonBar()->setStyleSheet(qssContent);
}

/**
 * \if ENGLISH
 * @brief Handle load customize XML file action trigger
 * @details Load previously saved custom configuration from XML file.
 * \endif
 *
 * \if CHINESE
 * @brief 处理加载自定义XML文件动作触发
 * @details 从XML文件加载之前保存的自定义配置。
 * \endif
 */
void MainWindow::onActionLoadCustomizeXmlFileTriggered()
{
    if (!mHasApplyCustomizeXmlFile) {
        if (!QFile::exists("customize.xml")) {
            mHasApplyCustomizeXmlFile = true;
            return;
        }
        QFile xmlFile("customize.xml");
        qDebug() << "size of customize.xml : " << xmlFile.size();
        if (xmlFile.size() <= 0) {
            mHasApplyCustomizeXmlFile = true;
            return;
        }
        mHasApplyCustomizeXmlFile = sa_apply_customize_from_xml_file("customize.xml", ribbonBar(), mActionsManager);
    }
}

/**
 * \if ENGLISH
 * @brief Handle window flag normal button action trigger
 * @param showButtons Whether to show window buttons
 * @details Control the display of window minimize, maximize, and close buttons.
 * \endif
 *
 * \if CHINESE
 * @brief 处理窗口标志普通按钮动作触发
 * @param showButtons 是否显示窗口按钮
 * @details 控制窗口的最小化、最大化和关闭按钮的显示。
 * \endif
 */
void MainWindow::onActionWindowFlagNormalButtonTriggered(bool showButtons)
{
    Qt::WindowFlags currentFlags = windowFlags();
    currentFlags.setFlag(Qt::WindowCloseButtonHint, showButtons);
    currentFlags.setFlag(Qt::WindowMinMaxButtonsHint, showButtons);
    setWindowFlags(currentFlags);
}

/**
 * \if ENGLISH
 * @brief Handle font combobox current font changed
 * @param newFont The newly selected font
 * @details When the user selects a new font in the font combobox, update the font of the Ribbon interface.
 * \endif
 *
 * \if CHINESE
 * @brief 处理字体组合框当前字体改变
 * @param newFont 新选择的字体
 * @details 当用户在字体组合框中选择新字体时，更新Ribbon界面的字体。
 * \endif
 */
void MainWindow::onFontComWidgetCurrentFontChanged(const QFont& newFont)
{
    qDebug() << "set font:" << newFont;
    ribbonBar()->setFont(newFont);
    update();
}

/**
 * \if ENGLISH
 * @brief Handle font larger action trigger
 * @details Increase the font size of the Ribbon interface.
 * \endif
 *
 * \if CHINESE
 * @brief 处理增大字体动作触发
 * @details 增大Ribbon界面字体大小。
 * \endif
 */
void MainWindow::onActionFontLargerTriggered()
{
    QFont currentFont = font();
    currentFont.setPointSize(currentFont.pointSize() + 1);
    qDebug() << "before set ribbonBar font:" << ribbonBar()->font();
    ribbonBar()->setFont(currentFont);
    update();
    qDebug() << "set font:" << currentFont;
}

/**
 * \if ENGLISH
 * @brief Handle font smaller action trigger
 * @details Decrease the font size of the Ribbon interface.
 * \endif
 *
 * \if CHINESE
 * @brief 处理减小字体动作触发
 * @details 减小Ribbon界面字体大小。
 * \endif
 */
void MainWindow::onActionFontSmallerTriggered()
{
    QFont currentFont = font();
    currentFont.setPointSize(currentFont.pointSize() - 1);
    qDebug() << "before set ribbonBar font:" << ribbonBar()->font();
    ribbonBar()->setFont(currentFont);
    update();
    qDebug() << "set font:" << currentFont;
}

/**
 * \if ENGLISH
 * @brief Handle word wrap action trigger
 * @param enableWordWrap Whether to enable word wrap
 * @details Control whether text on Ribbon buttons is displayed with line breaks.
 * \endif
 *
 * \if CHINESE
 * @brief 处理文字换行动作触发
 * @param enableWordWrap 是否启用文字换行
 * @details 控制Ribbon按钮上的文字是否换行显示。
 * \endif
 */
void MainWindow::onActionWordWrapTriggered(bool enableWordWrap)
{
    ribbonBar()->setEnableWordWrap(enableWordWrap);
    ui->textBrowser->append(tr("By using the SARibbonBar::setEnableWordWrap function, "
                               "you can set whether text breaks or not.\n"
                               "By default, the two line mode will not wrap, the three line mode will wrap.\n"
                               "You can force the two line mode to wrap, or the three line mode to not wrap"));
}

/**
 * \if ENGLISH
 * @brief Handle button group action trigger
 * @param triggeredAction The triggered action
 * @details Handle button click events in the button group, used to set title alignment.
 * \endif
 *
 * \if CHINESE
 * @brief 处理按钮组动作触发
 * @param triggeredAction 触发的动作
 * @details 处理按钮组中按钮的点击事件，用于设置标题对齐方式。
 * \endif
 */
void MainWindow::onButtonGroupActionTriggered(QAction* triggeredAction)
{
    QVariant alignmentVariant = triggeredAction->property("align");
    if (alignmentVariant.isValid()) {
        Qt::Alignment alignment = static_cast< Qt::Alignment >(alignmentVariant.toInt());
        if (!ribbonBar()) {
            return;
        }
        ribbonBar()->setWindowTitleAligment(alignment);
    }
}

/**
 * \if ENGLISH
 * @brief Handle color button color clicked
 * @param selectedColor The selected color
 * @param clicked Whether the button was clicked
 * @details Handle color selection events for color buttons.
 * \endif
 *
 * \if CHINESE
 * @brief 处理颜色按钮颜色点击
 * @param selectedColor 选择的颜色
 * @param clicked 是否点击
 * @details 处理颜色按钮的颜色选择事件。
 * \endif
 */
void MainWindow::onColorButtonColorClicked(const QColor& selectedColor, bool clicked)
{
    Q_UNUSED(clicked);
    ui->textBrowser->append(QString("color click %1").arg(selectedColor.name()));
}

/**
 * \if ENGLISH
 * @brief Handle Ribbon theme combobox current index changed
 * @param themeIndex The index of the selected theme
 * @details Switch Ribbon interface theme.
 * \endif
 *
 * \if CHINESE
 * @brief 处理Ribbon主题组合框当前索引改变
 * @param themeIndex 选择的主题索引
 * @details 切换Ribbon界面主题。
 * \endif
 */
void MainWindow::onRibbonThemeComboBoxCurrentIndexChanged(int themeIndex)
{
    SARibbonTheme selectedTheme = static_cast< SARibbonTheme >(mComboboxRibbonTheme->itemData(themeIndex).toInt());
    setRibbonTheme(selectedTheme);
}

/**
 * \if ENGLISH
 * @brief Handle hide action trigger
 * @param hideActions Whether to hide the actions
 * @details Batch control the visibility of a group of actions.
 * \endif
 *
 * \if CHINESE
 * @brief 处理隐藏动作触发
 * @param hideActions 是否隐藏动作
 * @details 批量控制一组动作的可见性。
 * \endif
 */
void MainWindow::onActionHideActionTriggered(bool hideActions)
{
    mActionWordWrap->setVisible(hideActions);
    mActionDisable->setVisible(hideActions);
    mActionUnlock->setVisible(hideActions);
    mActionSetTextTest->setVisible(hideActions);
    mActionShowTest->setVisible(hideActions);
    mActionHideAction2->setVisible(hideActions);
    mActionHideAction4->setVisible(hideActions);
    mActionHideShowTextAct2->setVisible(hideActions);
    mActionHideShowTextAct3->setVisible(hideActions);
    mActionHideShowTextAct4->setVisible(hideActions);
    ribbonBar()->updateRibbonGeometry();
}

/**
 * \if ENGLISH
 * @brief Handle visible all action trigger
 * @param visibleAll Whether to make all actions visible
 * @details Control the visibility of all actions registered to the action manager.
 * \endif
 *
 * \if CHINESE
 * @brief 处理全部可见动作触发
 * @param visibleAll 是否全部可见
 * @details 控制所有注册到动作管理器的动作的可见性。
 * \endif
 */
void MainWindow::onActionVisibleAllTriggered(bool visibleAll)
{
    const QList< QAction* > allActions = mActionsManager->allActions();
    for (QAction* currentAction : allActions) {
        if (currentAction != mActionVisibleAll) {
            currentAction->setVisible(visibleAll);
        }
    }
    ribbonBar()->updateRibbonGeometry();
}

/**
 * \if ENGLISH
 * @brief Handle alignment center checkbox clicked
 * @param centerAligned Whether to center align
 * @details Control whether Ribbon toolbar content is center aligned.
 * \endif
 *
 * \if CHINESE
 * @brief 处理居中对齐复选框点击
 * @param centerAligned 是否居中对齐
 * @details 控制Ribbon工具栏内容是否居中对齐。
 * \endif
 */
void MainWindow::onCheckBoxAlignmentCenterClicked(bool centerAligned)
{
    if (centerAligned) {
        ribbonBar()->setRibbonAlignment(SARibbonAlignment::AlignCenter);
    } else {
        ribbonBar()->setRibbonAlignment(SARibbonAlignment::AlignLeft);
    }
}

/**
 * \if ENGLISH
 * @brief Handle Ribbon title height spinbox value changed
 * @param newHeight The new title bar height
 * @details Adjust Ribbon title bar height.
 * \endif
 *
 * \if CHINESE
 * @brief 处理Ribbon标题高度数值框值改变
 * @param newHeight 新的标题栏高度
 * @details 调整Ribbon标题栏高度。
 * \endif
 */
void MainWindow::onSpinBoxRibbonTitleHeightChanged(int newHeight)
{
    ribbonBar()->setTitleBarHeight(newHeight);
}

/**
 * \if ENGLISH
 * @brief Handle Ribbon tab height spinbox value changed
 * @param newHeight The new tab bar height
 * @details Adjust Ribbon tab bar height.
 * \endif
 *
 * \if CHINESE
 * @brief 处理Ribbon标签高度数值框值改变
 * @param newHeight 新的标签栏高度
 * @details 调整Ribbon标签栏高度。
 * \endif
 */
void MainWindow::onSpinBoxRibbonTabHeightChanged(int newHeight)
{
    ribbonBar()->setTabBarHeight(newHeight);
}

/**
 * \if ENGLISH
 * @brief Handle Ribbon category height spinbox value changed
 * @param newHeight The new category area height
 * @details Adjust Ribbon category area height.
 * \endif
 *
 * \if CHINESE
 * @brief 处理Ribbon分类高度数值框值改变
 * @param newHeight 新的分类区域高度
 * @details 调整Ribbon分类区域高度。
 * \endif
 */
void MainWindow::onSpinBoxRibbonCategoryHeightChanged(int newHeight)
{
    ribbonBar()->setCategoryHeight(newHeight);
}

/**
 * \if ENGLISH
 * @brief Handle Ribbon panel title height spinbox value changed
 * @param newHeight The new panel title height
 * @details Adjust Ribbon panel title height.
 * \endif
 *
 * \if CHINESE
 * @brief 处理Ribbon面板标题高度数值框值改变
 * @param newHeight 新的面板标题高度
 * @details 调整Ribbon面板标题高度。
 * \endif
 */
void MainWindow::onSpinBoxRibbonPanelTtitleHeightChanged(int newHeight)
{
    ribbonBar()->setPanelTitleHeight(newHeight);
}

/**
 * \if ENGLISH
 * @brief Handle Ribbon panel spacing spinbox value changed
 * @param newSpacing The new spacing between panels
 * @details Adjust the spacing between Ribbon panels.
 * \endif
 *
 * \if CHINESE
 * @brief 处理Ribbon面板间距数值框值改变
 * @param newSpacing 新的面板间距
 * @details 调整Ribbon面板之间的间距。
 * \endif
 */
void MainWindow::onSpinBoxRibbonPanelSpacingChanged(int newSpacing)
{
    ribbonBar()->setPanelSpacing(newSpacing);
}

/**
 * \if ENGLISH
 * @brief Handle panel small icon size spinbox value changed
 * @param newSize The new small icon size
 * @details Adjust the icon size of small tool buttons in Ribbon panels.
 * \endif
 *
 * \if CHINESE
 * @brief 处理面板小图标大小数值框值改变
 * @param newSize 新的小图标大小
 * @details 调整Ribbon面板中工具按钮的小图标大小。
 * \endif
 */
void MainWindow::onSpinBoxPanelSmallIconSizeChanged(int newSize)
{
    ribbonBar()->setPanelSmallIconSize(QSize(newSize, newSize));
    ribbonBar()->update();
}

/**
 * \if ENGLISH
 * @brief Handle panel large icon size spinbox value changed
 * @param newSize The new large icon size
 * @details Adjust the icon size of large tool buttons in Ribbon panels.
 * \endif
 *
 * \if CHINESE
 * @brief 处理面板大图标大小数值框值改变
 * @param newSize 新的大图标大小
 * @details 调整Ribbon面板中工具按钮的大图标大小。
 * \endif
 */
void MainWindow::onSpinBoxPanelLargeIconSizeChanged(int newSize)
{
    ribbonBar()->setPanelLargeIconSize(QSize(newSize, newSize));
    ribbonBar()->update();
}

/**
 * \if ENGLISH
 * @brief Handle tool button maximum aspect ratio spinbox value changed
 * @param newRatio The new maximum aspect ratio
 * @details Adjust the maximum aspect ratio of tool buttons.
 * \endif
 *
 * \if CHINESE
 * @brief 处理工具按钮最大宽高比数值框值改变
 * @param newRatio 新的最大宽高比
 * @details 调整工具按钮的最大宽高比。
 * \endif
 */
void MainWindow::onDoubleSpinBoxToolbuttonMaximumAspectRatioChanged(double newRatio)
{
    ribbonBar()->setButtonMaximumAspectRatio(newRatio);
}

/**
 * \if ENGLISH
 * @brief Handle title background brush changed timeout
 * @details Periodically change the title bar background color to demonstrate dynamic effects.
 * \endif
 *
 * \if CHINESE
 * @brief 处理标题背景画刷改变超时
 * @details 周期性地改变标题栏背景颜色以演示动态效果。
 * \endif
 */
void MainWindow::onTitleBackgroundBrushChangedTimeout()
{
    static bool s_useNoBrush = true;
    s_useNoBrush             = (!s_useNoBrush);
    if (s_useNoBrush) {
        ribbonBar()->setWindowTitleTextColor(Qt::black);
        ribbonBar()->setWindowTitleBackgroundBrush(Qt::NoBrush);
    } else {
        ribbonBar()->setWindowTitleTextColor(Qt::white);
        ribbonBar()->setWindowTitleBackgroundBrush(Qt::red);
    }
    ribbonBar()->repaint();
}

/**
 * \if ENGLISH
 * @brief Handle changed title timeout
 * @details Timer slot function used to periodically change the title bar color.
 * \endif
 *
 * \if CHINESE
 * @brief 处理标题改变超时
 * @details 定时器槽函数，用于周期性地改变标题栏颜色。
 * \endif
 */
void MainWindow::onChangedTitleTimeout()
{
    static int s_toggleCounter = 0;
    auto currentRibbonBar      = ribbonBar();
    if (!currentRibbonBar) {
        return;
    }
    ++s_toggleCounter;
    if (s_toggleCounter % 2 == 0) {
        currentRibbonBar->setWindowTitleBackgroundBrush(QColor(222, 79, 79));
        currentRibbonBar->setWindowTitleTextColor(Qt::white);
        currentRibbonBar->update();
    } else {
        currentRibbonBar->setWindowTitleBackgroundBrush(Qt::NoBrush);
        currentRibbonBar->setWindowTitleTextColor(Qt::blue);
        currentRibbonBar->update();
    }
}

/**
 * \if ENGLISH
 * @brief Create other actions
 * @details Create some actions that are not directly displayed in the Ribbon interface,
 *          but can be used through customization features.
 * \endif
 *
 * \if CHINESE
 * @brief 创建其他动作
 * @details 创建一些不在Ribbon界面中直接显示的动作，但可以通过自定义功能使用。
 * \endif
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
 * \if ENGLISH
 * @brief Handle close event
 * @param closeEvent The close event
 * @details Ask user for confirmation before closing the window.
 * \endif
 *
 * \if CHINESE
 * @brief 处理关闭事件
 * @param closeEvent 关闭事件
 * @details 在窗口关闭前询问用户确认。
 * \endif
 */
void MainWindow::closeEvent(QCloseEvent* closeEvent)
{
    auto userResponse = QMessageBox::question(this, tr("question"), tr("Confirm whether to exit"));
    if (userResponse == QMessageBox::Yes) {
        closeEvent->accept();
    } else {
        closeEvent->ignore();
    }
}

/**
 * \if ENGLISH
 * @brief Handle save action trigger
 * @details Save action demonstration - shows how to handle save operations in Ribbon.
 * \endif
 *
 * \if CHINESE
 * @brief 处理保存动作触发
 * @details 保存动作演示 - 展示如何在Ribbon中处理保存操作。
 * \endif
 */
void MainWindow::onSaveActionTriggered()
{
    ui->textBrowser->append("save action clicked");
    setWindowModified(false);
    ribbonBar()->repaint();
}

/**
 * \if ENGLISH
 * @brief Handle hide ribbon action trigger
 * @param hide Whether to hide the ribbon
 * @details Toggle between minimum and normal ribbon mode.
 * \endif
 *
 * \if CHINESE
 * @brief 处理隐藏Ribbon动作触发
 * @param hide 是否隐藏Ribbon
 * @details 在最小模式和正常模式之间切换。
 * \endif
 */
void MainWindow::onHideRibbonActionTriggered(bool hide)
{
    ribbonBar()->setMinimumMode(hide);
}

/**
 * \if ENGLISH
 * @brief Handle show/hide button action trigger
 * @param show Whether to show the button
 * @details Show or hide the built-in minimum mode button.
 * \endif
 *
 * \if CHINESE
 * @brief 处理显示/隐藏按钮动作触发
 * @param show 是否显示按钮
 * @details 显示或隐藏内置的最小化模式按钮。
 * \endif
 */
void MainWindow::onShowHideButtonActionTriggered(bool show)
{
    ribbonBar()->showMinimumModeButton(show);
}

/**
 * \if ENGLISH
 * @brief Handle show title icon action trigger
 * @param show Whether to show the title icon
 * @details Show or hide the title icon in the ribbon bar.
 * \endif
 *
 * \if CHINESE
 * @brief 处理显示标题图标动作触发
 * @param show 是否显示标题图标
 * @details 显示或隐藏Ribbon栏中的标题图标。
 * \endif
 */
void MainWindow::onShowTitleIconActionTriggered(bool show)
{
    ribbonBar()->setTitleIconVisible(show);
}

/**
 * \if ENGLISH
 * @brief Handle application button expand action trigger
 * @param expand Whether to expand the application button
 * @details Set whether the Application Button occupies the title bar and tab bar.
 * \endif
 *
 * \if CHINESE
 * @brief 处理应用程序按钮展开动作触发
 * @param expand 是否展开应用程序按钮
 * @details 设置应用程序按钮是否占用标题栏和标签栏。
 * \endif
 */
void MainWindow::onAppButtonExpandActionTriggered(bool expand)
{
    ribbonBar()->setApplicationButtonVerticalExpansion(expand);
}

/**
 * \if ENGLISH
 * @brief Handle show context action trigger
 * @param show Whether to show the context category
 * @details Show or hide the context category.
 * \endif
 *
 * \if CHINESE
 * @brief 处理显示上下文动作触发
 * @param show 是否显示上下文分类
 * @details 显示或隐藏上下文分类。
 * \endif
 */
void MainWindow::onShowContextActionTriggered(bool show)
{
    onShowContextCategory(show);
}

/**
 * \if ENGLISH
 * @brief Handle delete context action trigger
 * @details Delete the context category.
 * \endif
 *
 * \if CHINESE
 * @brief 处理删除上下文动作触发
 * @details 删除上下文分类。
 * \endif
 */
void MainWindow::onDeleteContextActionTriggered()
{
    if (mContextCategory) {
        ribbonBar()->destroyContextCategory(mContextCategory);
        mContextCategory = nullptr;
        if (mActionDeleteContext) {
            mActionDeleteContext->setDisabled(true);
        }
    }
}

/**
 * \if ENGLISH
 * @brief Handle word wrap demo action trigger
 * @details Demonstrate text wrapping functionality.
 * \endif
 *
 * \if CHINESE
 * @brief 处理文字换行演示动作触发
 * @details 演示文字换行功能。
 * \endif
 */
void MainWindow::onWordWrapDemoActionTriggered()
{
    ui->textBrowser->append(tr("Text can be manually wrapped(use \\n), and will appear as 1 line in the case of "
                               "SARibbonBar::setEnableWordWrap (false)"));
}

/**
 * \if ENGLISH
 * @brief Handle word wrap with menu action trigger
 * @details Demonstrate text wrapping with menu functionality.
 * \endif
 *
 * \if CHINESE
 * @brief 处理带菜单的文字换行动作触发
 * @details 演示带菜单的文字换行功能。
 * \endif
 */
void MainWindow::onWordWrapWithMenuActionTriggered()
{
    ui->textBrowser->append(tr("Text can be manually wrapped(use \\n), and will appear as 1 line in the case of "
                               "SARibbonBar::setEnableWordWrap (false)"));
}

/**
 * \if ENGLISH
 * @brief Handle remove panel action trigger
 * @param category The category containing the panel
 * @param panel The panel to remove
 * @details Remove a panel from a category.
 * \endif
 *
 * \if CHINESE
 * @brief 处理移除面板动作触发
 * @param category 包含面板的分类
 * @param panel 要移除的面板
 * @details 从分类中移除面板。
 * \endif
 */
void MainWindow::onRemovePanelActionTriggered(SARibbonCategory* category, SARibbonPanel* panel)
{
    if (category && panel) {
        category->removePanel(panel);
    }
}

/**
 * \if ENGLISH
 * @brief Handle remove category action trigger
 * @param category The category to remove
 * @details Remove a category from the ribbon bar.
 * \endif
 *
 * \if CHINESE
 * @brief 处理移除分类动作触发
 * @param category 要移除的分类
 * @details 从Ribbon栏中移除分类。
 * \endif
 */
void MainWindow::onRemoveCategoryActionTriggered(SARibbonCategory* category)
{
    if (category) {
        ribbonBar()->removeCategory(category);
        category->hide();
        category->deleteLater();
    }
}

/**
 * \if ENGLISH
 * @brief Handle insert panel at start action trigger
 * @param category The category to insert the panel into
 * @details Insert a new panel at the start position (index 0).
 * \endif
 *
 * \if CHINESE
 * @brief 处理在开头插入面板动作触发
 * @param category 要插入面板的分类
 * @details 在起始位置（索引0）插入新面板。
 * \endif
 */
void MainWindow::onInsertPanelAtStartActionTriggered(SARibbonCategory* category)
{
    if (!category)
        return;
    SARibbonPanel* newPanel = new SARibbonPanel(("Inserted at 0"));
    static int insertCount  = 0;
    ++insertCount;
    QAction* newAction = new QAction("Inserted at 0\n Action " + QString::number(insertCount), newPanel);
    newPanel->addLargeAction(newAction);
    SARibbonToolButton* toolButton = newPanel->actionToRibbonToolButton(newAction);
    toolButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    category->insertPanel(newPanel, 0);
}

/**
 * \if ENGLISH
 * @brief Handle insert panel at end action trigger
 * @param category The category to insert the panel into
 * @details Insert a new panel at the end position.
 * \endif
 *
 * \if CHINESE
 * @brief 处理在末尾插入面板动作触发
 * @param category 要插入面板的分类
 * @details 在末尾位置插入新面板。
 * \endif
 */
void MainWindow::onInsertPanelAtEndActionTriggered(SARibbonCategory* category)
{
    if (!category)
        return;
    SARibbonPanel* newPanel = new SARibbonPanel(("Inserted at end"));
    static int insertCount  = 0;
    ++insertCount;
    QAction* newAction = new QAction("Inserted at end\n Action " + QString::number(insertCount), newPanel);
    newPanel->addLargeAction(newAction);
    SARibbonToolButton* toolButton = newPanel->actionToRibbonToolButton(newAction);
    toolButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    category->insertPanel(newPanel, category->panelCount());
}

/**
 * \if ENGLISH
 * @brief Handle insert panel at negative one action trigger
 * @param category The category to insert the panel into
 * @details Insert a new panel at position -1 (which appends to the end).
 * \endif
 *
 * \if CHINESE
 * @brief 处理在-1位置插入面板动作触发
 * @param category 要插入面板的分类
 * @details 在-1位置插入新面板（相当于追加到末尾）。
 * \endif
 */
void MainWindow::onInsertPanelAtNegativeOneActionTriggered(SARibbonCategory* category)
{
    if (!category)
        return;
    SARibbonPanel* newPanel = new SARibbonPanel(("Inserted at -1"));
    static int insertCount  = 0;
    ++insertCount;
    QAction* newAction = new QAction("Inserted at -1\n Action " + QString::number(insertCount), newPanel);
    newPanel->addLargeAction(newAction);
    SARibbonToolButton* toolButton = newPanel->actionToRibbonToolButton(newAction);
    toolButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    category->insertPanel(newPanel, -1);
}

/**
 * \if ENGLISH
 * @brief Handle unlock action trigger
 * @details Unlock the disabled action.
 * \endif
 *
 * \if CHINESE
 * @brief 处理解锁动作触发
 * @details 解锁被禁用的动作。
 * \endif
 */
void MainWindow::onUnlockActionTriggered()
{
    mActionDisable->setEnabled(true);
    mActionDisable->setText(("Enabled"));
}

/**
 * \if ENGLISH
 * @brief Handle set text action toggled
 * @param checked Whether the action is checked
 * @details Toggle the text of the action between Chinese and English.
 * \endif
 *
 * \if CHINESE
 * @brief 处理设置文本动作切换
 * @param checked 动作是否被选中
 * @details 在中文和英文之间切换动作的文本。
 * \endif
 */
void MainWindow::onSetTextActionToggled(bool checked)
{
    if (checked) {
        mActionSetTextTest->setText(QStringLiteral(u"setText测试"));
    } else {
        mActionSetTextTest->setText(QStringLiteral(u"set text"));
    }
}

/**
 * \if ENGLISH
 * @brief Handle show panel action toggled
 * @param checked Whether the action is checked
 * @details Toggle the visibility of the example panel.
 * \endif
 *
 * \if CHINESE
 * @brief 处理显示面板动作切换
 * @param checked 动作是否被选中
 * @details 切换示例面板的可见性。
 * \endif
 */
void MainWindow::onShowPanelActionToggled(bool checked)
{
    mPanelVisbileExample->setVisible(!checked);
    if (checked) {
        mActionShowTest->setText(tr("hide beside panel"));
    } else {
        mActionShowTest->setText(tr("show beside panel"));
    }
    ribbonBar()->repaint();
}

/**
 * \if ENGLISH
 * @brief Handle hide action 2 trigger
 * @param checked Whether the action is checked
 * @details Toggle the visibility of action 2.
 * \endif
 *
 * \if CHINESE
 * @brief 处理隐藏动作2触发
 * @param checked 动作是否被选中
 * @details 切换动作2的可见性。
 * \endif
 */
void MainWindow::onHideAction2Triggered(bool checked)
{
    if (checked) {
        mActionHideShowTextAct2->setVisible(true);
        mActionHideAction2->setText(tr("hide action2"));
    } else {
        mActionHideShowTextAct2->setVisible(false);
        mActionHideAction2->setText(tr("show action2"));
    }
}

/**
 * \if ENGLISH
 * @brief Handle hide action 4 trigger
 * @param checked Whether the action is checked
 * @details Toggle the visibility of action 4.
 * \endif
 *
 * \if CHINESE
 * @brief 处理隐藏动作4触发
 * @param checked 动作是否被选中
 * @details 切换动作4的可见性。
 * \endif
 */
void MainWindow::onHideAction4Triggered(bool checked)
{
    if (checked) {
        mActionHideShowTextAct4->setVisible(true);
        mActionHideAction4->setText(tr("hide action4"));
    } else {
        mActionHideShowTextAct4->setVisible(false);
        mActionHideAction4->setText(tr("show action4"));
    }
}

/**
 * \if ENGLISH
 * @brief Handle delete this category action trigger
 * @param category The category to delete
 * @details Delete the specified category from the ribbon bar.
 * \endif
 *
 * \if CHINESE
 * @brief 处理删除此分类动作触发
 * @param category 要删除的分类
 * @details 从Ribbon栏中删除指定的分类。
 * \endif
 */
void MainWindow::onDeleteThisCategoryActionTriggered(SARibbonCategory* category)
{
    if (category) {
        ribbonBar()->removeCategory(category);
        category->deleteLater();
    }
}

/**
 * \if ENGLISH
 * @brief Handle undo action trigger
 * @details Undo operation demonstration.
 * \endif
 *
 * \if CHINESE
 * @brief 处理撤销动作触发
 * @details 撤销操作演示。
 * \endif
 */
void MainWindow::onUndoActionTriggered()
{
    ui->textBrowser->append("undo");
}

/**
 * \if ENGLISH
 * @brief Handle redo action trigger
 * @details Redo operation demonstration.
 * \endif
 *
 * \if CHINESE
 * @brief 处理重做动作触发
 * @details 重做操作演示。
 * \endif
 */
void MainWindow::onRedoActionTriggered()
{
    ui->textBrowser->append("redo");
}

/**
 * \if ENGLISH
 * @brief Handle search editor editing finished
 * @details Process the search text when editing is finished.
 * \endif
 *
 * \if CHINESE
 * @brief 处理搜索编辑器编辑完成
 * @details 当编辑完成时处理搜索文本。
 * \endif
 */
void MainWindow::onSearchEditorEditingFinished()
{
    ui->textBrowser->append(mSearchEditor->text());
}

/**
 * \if ENGLISH
 * @brief Handle login action trigger
 * @details Login action demonstration.
 * \endif
 *
 * \if CHINESE
 * @brief 处理登录动作触发
 * @details 登录操作演示。
 * \endif
 */
void MainWindow::onLoginActionTriggered()
{
    ui->textBrowser->append("Login triggered");
}

/**
 * \if ENGLISH
 * @brief Handle add gallery item action trigger
 * @param galleryGroup The gallery group to add the item to
 * @details Add a new item to the specified gallery group.
 * \endif
 *
 * \if CHINESE
 * @brief 处理添加Gallery项动作触发
 * @param galleryGroup 要添加项的Gallery组
 * @details 向指定的Gallery组添加新项。
 * \endif
 */
void MainWindow::onAddGalleryItemActionTriggered(SARibbonGalleryGroup* galleryGroup)
{
    if (galleryGroup) {
        QAction* newAction = new QAction(QIcon(":/gallery-icon/icon/gallery/Document-File.svg"), QString("New Action"));
        galleryGroup->addActionItem(newAction);
    }
}

/**
 * \if ENGLISH
 * @brief Handle change language action trigger
 * @param toEnglish Whether to change to English
 * @details Toggle between Chinese and English panel names.
 * \endif
 *
 * \if CHINESE
 * @brief 处理切换语言动作触发
 * @param toEnglish 是否切换到英文
 * @details 在中文和英文面板名称之间切换。
 * \endif
 */
void MainWindow::onChangeLanguageActionTriggered(bool toEnglish)
{
    SARibbonPanel* panelUtf8 = qobject_cast< SARibbonPanel* >(sender()->parent());
    if (toEnglish) {
        if (panelUtf8) {
            panelUtf8->setPanelName(QStringLiteral(u"show chinese char"));
        }
    } else {
        if (panelUtf8) {
            panelUtf8->setPanelName(QStringLiteral(u"中文显示测试"));
        }
    }
}

/**
 * \if ENGLISH
 * @brief Handle context category 1 action trigger
 * @param visible Whether to make the context category visible
 * @details Set the visibility of context category 1.
 * \endif
 *
 * \if CHINESE
 * @brief 处理上下文分类1动作触发
 * @param visible 是否使上下文分类可见
 * @details 设置上下文分类1的可见性。
 * \endif
 */
void MainWindow::onContextCategory1ActionTriggered(bool visible)
{
    ribbonBar()->setContextCategoryVisible(mContextCategory, visible);
}

/**
 * \if ENGLISH
 * @brief Handle context category 2 action trigger
 * @param visible Whether to make the context category visible
 * @details Set the visibility of context category 2.
 * \endif
 *
 * \if CHINESE
 * @brief 处理上下文分类2动作触发
 * @param visible 是否使上下文分类可见
 * @details 设置上下文分类2的可见性。
 * \endif
 */
void MainWindow::onContextCategory2ActionTriggered(bool visible)
{
    ribbonBar()->setContextCategoryVisible(mContextCategory2, visible);
}

/**
 * \if ENGLISH
 * @brief Create main category page content
 * @param categoryPage Main category page pointer
 * @details Build panels and controls for the main category page, demonstrating basic usage of Ribbon.
 *          This function shows how to:
 *          - Create panels with different button styles
 *          - Set up keyboard shortcuts
 *          - Configure ribbon styles and themes
 *          - Add various widgets to panels
 * \endif
 *
 * \if CHINESE
 * @brief 创建主分类页面内容
 * @param categoryPage 主分类页面指针
 * @details 构建主分类页面的各个面板和控件，展示Ribbon的基本用法。
 *          此函数演示如何：
 *          - 创建具有不同按钮样式的面板
 *          - 设置键盘快捷键
 *          - 配置Ribbon样式和主题
 *          - 向面板添加各种控件
 * \endif
 */
void MainWindow::createCategoryMain(SARibbonCategory* categoryPage)
{
    SARibbonPanel* panelStyle = categoryPage->addPanel(tr("ribbon style"));

    QAction* actionSave = createAction(tr("Save"), ":/icon/icon/save.svg");
    connect(actionSave, &QAction::triggered, this, &MainWindow::onSaveActionTriggered);
    actionSave->setShortcut(QKeySequence(QLatin1String("Ctrl+S")));
    addAction(actionSave);
    panelStyle->addLargeAction(actionSave);

    QAction* actionHideRibbon = createAction(tr("hide ribbon"), ":/icon/icon/hideRibbon.svg", "actHideRibbon");
    actionHideRibbon->setCheckable(true);
    panelStyle->addSmallAction(actionHideRibbon);
    connect(actionHideRibbon, &QAction::triggered, this, &MainWindow::onHideRibbonActionTriggered);
    connect(ribbonBar(), &SARibbonBar::ribbonModeChanged, this, [ actionHideRibbon ](SARibbonBar::RibbonMode currentMode) {
        actionHideRibbon->setChecked(currentMode == SARibbonBar::MinimumRibbonMode);
    });

    QAction* actionShowHideButton =
        createAction(tr("show \nhide button"), ":/icon/icon/showHideButton.svg", "show hide button");
    actionShowHideButton->setCheckable(true);
    actionShowHideButton->setChecked(ribbonBar()->haveShowMinimumModeButton());
    panelStyle->addSmallAction(actionShowHideButton);
    connect(actionShowHideButton, &QAction::triggered, this, &MainWindow::onShowHideButtonActionTriggered);

    mActionWordWrap = createAction(tr("word wrap"), ":/icon/icon/wordwrap.svg");
    mActionWordWrap->setCheckable(true);
    mActionWordWrap->setChecked(ribbonBar()->isEnableWordWrap());
    panelStyle->addSmallAction(mActionWordWrap);
    connect(mActionWordWrap, &QAction::triggered, this, &MainWindow::onActionWordWrapTriggered);

    QAction* actionShowTitleIcon =
        createAction(tr("show\n title icon"), ":/icon/icon/showHideButton.svg", "show hide title icon");
    actionShowTitleIcon->setCheckable(true);
    actionShowTitleIcon->setChecked(ribbonBar()->isTitleIconVisible());
    connect(actionShowTitleIcon, &QAction::triggered, this, &MainWindow::onShowTitleIconActionTriggered);
    panelStyle->addSmallAction(actionShowTitleIcon);

    QAction* actionAppBtnExpand = createAction(tr("App Btn\n Expand"), ":/icon/icon/layout.svg");
    actionAppBtnExpand->setCheckable(true);
    actionAppBtnExpand->setChecked(ribbonBar()->isApplicationButtonVerticalExpansion());
    panelStyle->addSmallAction(actionAppBtnExpand);
    connect(actionAppBtnExpand, &QAction::triggered, this, [ this, actionShowTitleIcon ](bool expand) {
        onAppButtonExpandActionTriggered(expand);
        if (expand) {
            actionShowTitleIcon->setChecked(false);
        }
    });

    panelStyle->addSeparator();

    QButtonGroup* styleButtonGroup = new QButtonGroup(categoryPage);

    QRadioButton* radioOfficeStyle = new QRadioButton();
    radioOfficeStyle->setText(tr("use office style"));
    radioOfficeStyle->setObjectName(("use office style"));
    radioOfficeStyle->setWindowTitle(radioOfficeStyle->text());
    radioOfficeStyle->setChecked(true);
    panelStyle->addSmallWidget(radioOfficeStyle);
    styleButtonGroup->addButton(radioOfficeStyle, SARibbonBar::RibbonStyleLooseThreeRow);

    QRadioButton* radioWpsStyle = new QRadioButton();
    radioWpsStyle->setObjectName(("use wps style"));
    radioWpsStyle->setText(tr("use wps style"));
    radioWpsStyle->setWindowTitle(radioWpsStyle->text());
    radioWpsStyle->setChecked(false);
    panelStyle->addSmallWidget(radioWpsStyle);
    styleButtonGroup->addButton(radioWpsStyle, SARibbonBar::RibbonStyleCompactThreeRow);

    QRadioButton* radioOffice2RowStyle = new QRadioButton();
    radioOffice2RowStyle->setObjectName(("use office 2row style"));
    radioOffice2RowStyle->setText(tr("use office 2 row style"));
    radioOffice2RowStyle->setWindowTitle(radioOffice2RowStyle->text());
    radioOffice2RowStyle->setChecked(false);
    panelStyle->addSmallWidget(radioOffice2RowStyle);
    styleButtonGroup->addButton(radioOffice2RowStyle, SARibbonBar::RibbonStyleLooseTwoRow);

    QRadioButton* radioWps2RowStyle = new QRadioButton();
    radioWps2RowStyle->setObjectName(("use wps 2row style"));
    radioWps2RowStyle->setText(tr("use wps 2row style"));
    radioWps2RowStyle->setWindowTitle(radioWps2RowStyle->text());
    radioWps2RowStyle->setChecked(false);
    panelStyle->addSmallWidget(radioWps2RowStyle);
    styleButtonGroup->addButton(radioWps2RowStyle, SARibbonBar::RibbonStyleCompactTwoRow);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    connect(styleButtonGroup,
            static_cast< void (QButtonGroup::*)(int) >(&QButtonGroup::idClicked),
            this,
            &MainWindow::onStyleClicked);
#else
    connect(styleButtonGroup,
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

    QCheckBox* checkBoxAlignment = new QCheckBox(this);
    checkBoxAlignment->setText(tr("Alignment Center"));
    checkBoxAlignment->setObjectName("checkBoxAlignmentCenter");
    checkBoxAlignment->setWindowTitle(checkBoxAlignment->text());
    connect(checkBoxAlignment, &QCheckBox::clicked, this, &MainWindow::onCheckBoxAlignmentCenterClicked);
    panelStyle->addSmallWidget(checkBoxAlignment);

    SARibbonPanel* panelToolButtonStyle = categoryPage->addPanel(("sa ribbon toolbutton style"));

    SARibbonMenu* demoMenu  = new SARibbonMenu(this);
    QAction* menuItemAction = nullptr;
    {
        QIcon itemIcon = QIcon(":/icon/icon/item.svg");
        for (int itemIndex = 0; itemIndex < 5; ++itemIndex) {
            menuItemAction = demoMenu->addAction(itemIcon, tr("item %1").arg(itemIndex + 1));
            menuItemAction->setObjectName(QStringLiteral("item %1").arg(itemIndex + 1));
        }
    }

    QAction* actionTest1 = createAction(tr("test 1"), ":/icon/icon/test1.svg");
    QVariant testData("Test");
    actionTest1->setData(testData);
    actionTest1->setMenu(demoMenu);
    actionTest1->setToolTip(tr("use QToolButton::MenuButtonPopup mode"));
    panelToolButtonStyle->addSmallAction(actionTest1, QToolButton::MenuButtonPopup);

    QAction* actionTest2 = createAction(tr("test 2"), ":/icon/icon/test2.svg");
    actionTest2->setMenu(demoMenu);
    actionTest2->setToolTip(tr("use QToolButton::InstantPopup mode"));
    panelToolButtonStyle->addSmallAction(actionTest2, QToolButton::InstantPopup);

    panelToolButtonStyle->addSeparator();

    QAction* actionDelayedPopup = createAction(tr("Delayed\nPopup"), ":/icon/icon/folder-cog.svg");
    actionDelayedPopup->setMenu(demoMenu);
    panelToolButtonStyle->addLargeAction(actionDelayedPopup, QToolButton::DelayedPopup);
    connect(actionDelayedPopup, &QAction::triggered, this, &MainWindow::onDelayedPopupCheckabletriggered);

    QAction* actionMenuButtonPopup = createAction(tr("Menu Button Popup"), ":/icon/icon/folder-star.svg");
    actionMenuButtonPopup->setMenu(demoMenu);
    panelToolButtonStyle->addLargeAction(actionMenuButtonPopup, QToolButton::MenuButtonPopup);
    connect(actionMenuButtonPopup, &QAction::triggered, this, &MainWindow::onMenuButtonPopupCheckabletriggered);

    QAction* actionInstantPopup = createAction(tr("Instant Popup"), ":/icon/icon/folder-stats.svg");
    actionInstantPopup->setMenu(demoMenu);
    panelToolButtonStyle->addLargeAction(actionInstantPopup, QToolButton::InstantPopup);
    connect(actionInstantPopup, &QAction::triggered, this, &MainWindow::onInstantPopupCheckabletriggered);

    QAction* actionDelayedPopupCheckable = createAction(tr("Delayed Popup checkable"), ":/icon/icon/folder-table.svg");
    actionDelayedPopupCheckable->setCheckable(true);
    actionDelayedPopupCheckable->setMenu(demoMenu);
    panelToolButtonStyle->addLargeAction(actionDelayedPopupCheckable, QToolButton::DelayedPopup);
    connect(actionDelayedPopupCheckable, &QAction::triggered, this, &MainWindow::onDelayedPopupCheckableTest);

    QAction* actionMenuButtonPopupCheckable =
        createAction(tr("Menu Button Popup checkable"), ":/icon/icon/folder-checkmark.svg");
    actionMenuButtonPopupCheckable->setCheckable(true);
    actionMenuButtonPopupCheckable->setMenu(demoMenu);
    panelToolButtonStyle->addLargeAction(actionMenuButtonPopupCheckable, QToolButton::MenuButtonPopup);
    connect(actionMenuButtonPopupCheckable, &QAction::triggered, this, &MainWindow::onMenuButtonPopupCheckableTest);

    QAction* actionDisabled = createAction(tr("disable action"), ":/icon/icon/disable.svg");
    actionDisabled->setCheckable(true);
    actionDisabled->setMenu(demoMenu);
    actionDisabled->setEnabled(false);
    panelToolButtonStyle->addLargeAction(actionDisabled);

    QAction* optionAction = new QAction(this);
    connect(optionAction, &QAction::triggered, this, [ this ](bool triggered) {
        Q_UNUSED(triggered);
        QMessageBox::information(this, tr("Option Action Triggered"), tr("Option Action Triggered"));
    });
    panelToolButtonStyle->setOptionAction(optionAction);

    SARibbonPanel* panelContextTest = categoryPage->addPanel(("panel 2"));

    QAction* actionShowContext = createAction(tr("show Context"), ":/icon/icon/showContext.svg");
    actionShowContext->setCheckable(true);
    panelContextTest->addLargeAction(actionShowContext);
    connect(actionShowContext, &QAction::triggered, this, &MainWindow::onShowContextCategory);

    mActionDeleteContext = createAction(tr("delete Context"), ":/icon/icon/deleteContext.svg");
    panelContextTest->addLargeAction(mActionDeleteContext);
    connect(mActionDeleteContext, &QAction::triggered, this, &MainWindow::onDeleteContextActionTriggered);

    QAction* actionWordWrapDemo = createAction(tr("Word\nWrap"), ":/icon/icon/setText.svg");
    panelContextTest->addLargeAction(actionWordWrapDemo);
    connect(actionWordWrapDemo, &QAction::triggered, this, &MainWindow::onWordWrapDemoActionTriggered);

    QAction* actionWordWrapWithMenu = createAction(tr("Word \nWrap"), ":/icon/icon/setText.svg");
    actionWordWrapWithMenu->setMenu(demoMenu);
    panelContextTest->addLargeAction(actionWordWrapWithMenu);
    connect(actionWordWrapWithMenu, &QAction::triggered, this, &MainWindow::onWordWrapWithMenuActionTriggered);

    QAction* actionIconOnly = createAction(QString(), ":/icon/icon/setText.svg", "ToolButtonIconOnly");
    panelContextTest->addLargeAction(actionIconOnly);
    if (SARibbonToolButton* toolButton = panelContextTest->lastAddActionButton()) {
        toolButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    }

    QAction* actionTextOnly = createAction(tr("Text Only"), ":/icon/icon/setText.svg", "ToolButtonTextOnly");
    panelContextTest->addLargeAction(actionTextOnly);
    if (SARibbonToolButton* toolButton = panelContextTest->lastAddActionButton()) {
        toolButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    }

    SARibbonPanel* panelWidgetTest = categoryPage->addPanel(tr("widget test"));
    panelWidgetTest->setObjectName(QStringLiteral(u"panelWidgetTest"));

    QComboBox* comboBoxTest = new QComboBox(this);
    comboBoxTest->setObjectName("QComboBox test");
    for (int itemIndex = 0; itemIndex < 40; ++itemIndex) {
        comboBoxTest->addItem(QString("QComboBox test%1").arg(itemIndex + 1));
    }
    comboBoxTest->setEditable(true);
    panelWidgetTest->addSmallWidget(comboBoxTest);

    QComboBox* comboBoxEditable = new QComboBox(this);
    comboBoxEditable->setObjectName("ComboBox Editable");
    for (int itemIndex = 0; itemIndex < 40; ++itemIndex) {
        comboBoxEditable->addItem(QString("item %1").arg(itemIndex + 1));
    }
    panelWidgetTest->addSmallWidget(comboBoxEditable);

    QLineEdit* lineEditWidget = new QLineEdit(this);
    lineEditWidget->setObjectName("Line Edit");
    lineEditWidget->setText("LineEdit");
    panelWidgetTest->addSmallWidget(lineEditWidget);

    panelWidgetTest->addSeparator();

    QCalendarWidget* calendarWidget = new QCalendarWidget(this);
    calendarWidget->setObjectName(("calendarWidget"));
    panelWidgetTest->addLargeWidget(calendarWidget);
    QAction* calendarOptionAction = new QAction(this);
    connect(calendarOptionAction, &QAction::triggered, this, [ this ](bool triggered) {
        Q_UNUSED(triggered);
        QMessageBox::information(this, tr("Option Action Triggered"), tr("Option Action Triggered"));
    });
    panelWidgetTest->setOptionAction(calendarOptionAction);

    panelWidgetTest->setVisible(true);
}

/**
 * \if ENGLISH
 * @brief Create other category page content
 * @param categoryPage Other category page pointer
 * @details Build content for other category pages, demonstrating more Ribbon control usages.
 *          This function shows how to:
 *          - Create button groups
 *          - Use Gallery controls
 *          - Handle context categories
 * \endif
 *
 * \if CHINESE
 * @brief 创建其他分类页面内容
 * @param categoryPage 其他分类页面指针
 * @details 构建其他分类页面的内容，展示更多Ribbon控件的用法。
 *          此函数演示如何：
 *          - 创建按钮组
 *          - 使用Gallery控件
 *          - 处理上下文分类
 * \endif
 */
void MainWindow::createCategoryOther(SARibbonCategory* categoryPage)
{
    SARibbonPanel* panelButtonGroups = new SARibbonPanel(tr("panel one"));
    panelButtonGroups->setObjectName("CategoryOther-panel1");
    categoryPage->addPanel(panelButtonGroups);

    SARibbonButtonGroupWidget* buttonGroup1 = new SARibbonButtonGroupWidget(panelButtonGroups);
    buttonGroup1->setObjectName("SARibbonButtonGroupWidget1");

    buttonGroup1->addAction(createAction(tr("Decrease Margin"), ":/icon/icon/Decrease-Margin.svg"));
    buttonGroup1->addAction(createAction(tr("Decrease Indent"), ":/icon/icon/Decrease-Indent.svg"));
    buttonGroup1->addAction(createAction(tr("Wrap Image Left"), ":/icon/icon/Wrap-Image Left.svg"));
    buttonGroup1->addAction(createAction(tr("Wrap Image Right"), ":/icon/icon/Wrap-Image Right.svg"));
    panelButtonGroups->addWidget(buttonGroup1, SARibbonPanelItem::Medium);

    SARibbonButtonGroupWidget* buttonGroup2 = new SARibbonButtonGroupWidget(panelButtonGroups);
    buttonGroup2->setObjectName("SARibbonButtonGroupWidget2");

    QAction* actionAlignRight = createAction(tr("Align Right"), ":/icon/icon/Align-Right.svg");
    actionAlignRight->setProperty("align", (int)Qt::AlignRight | Qt::AlignVCenter);
    buttonGroup2->addAction(actionAlignRight);
    QAction* actionAlignLeft = createAction(tr("Align Left"), ":/icon/icon/Align-Left.svg");
    actionAlignLeft->setProperty("align", (int)Qt::AlignLeft | Qt::AlignVCenter);
    buttonGroup2->addAction(actionAlignLeft);
    QAction* actionAlignCenter = createAction(tr("Align Center"), ":/icon/icon/Align-Center.svg");
    actionAlignCenter->setProperty("align", (int)Qt::AlignCenter);
    buttonGroup2->addAction(actionAlignCenter);
    panelButtonGroups->addWidget(buttonGroup2, SARibbonPanelItem::Medium);
    connect(buttonGroup2, &SARibbonButtonGroupWidget::actionTriggered, this, &MainWindow::onButtonGroupActionTriggered);

    SARibbonGallery* gallery = panelButtonGroups->addGallery();
    QList< QAction* > galleryActions;
    auto createGalleryAction = [ this ](const QString& text, const QString& iconPath) -> QAction* {
        QAction* galleryAction = this->createAction(text, iconPath);
        this->connect(galleryAction, &QAction::triggered, this, [ this, text ]() {
            if (this->ui->textBrowser) {
                this->ui->textBrowser->append(QString("%1 triggered").arg(text));
            }
        });
        return galleryAction;
    };
    galleryActions.append(createGalleryAction("Document File", ":/gallery-icon/icon/gallery/Document-File.svg"));
    galleryActions.append(createGalleryAction("Download File", ":/gallery-icon/icon/gallery/Download-File.svg"));
    galleryActions.append(createGalleryAction("Drive File Four Word", ":/gallery-icon/icon/gallery/Drive-File.svg"));
    galleryActions.append(createGalleryAction("Dropbox File", ":/gallery-icon/icon/gallery/Dropbox-File.svg"));
    galleryActions.append(createGalleryAction("Email File", ":/gallery-icon/icon/gallery/Email-File.svg"));
    galleryActions.append(createGalleryAction("Encode File", ":/gallery-icon/icon/gallery/Encode-File.svg"));
    galleryActions.append(createGalleryAction("Favorit File", ":/gallery-icon/icon/gallery/Favorit-File.svg"));
    galleryActions.append(createGalleryAction("File Error", ":/gallery-icon/icon/gallery/File-Error.svg"));
    galleryActions.append(createGalleryAction("File Read Only", ":/gallery-icon/icon/gallery/File-Readonly.svg"));
    galleryActions.append(createGalleryAction("File Settings", ":/gallery-icon/icon/gallery/File-Settings.svg"));
    galleryActions.append(createGalleryAction("Presentation File", ":/gallery-icon/icon/gallery/Presentation-File.svg"));
    SARibbonGalleryGroup* galleryGroup1 = gallery->addCategoryActions(tr("Files"), galleryActions);
    galleryGroup1->setGalleryGroupStyle(SARibbonGalleryGroup::IconWithWordWrapText);
    galleryGroup1->setGridMinimumWidth(80);
    galleryActions.clear();
    galleryActions.append(createGalleryAction("Photoshop", ":/gallery-icon/icon/gallery/Photoshop.svg"));
    galleryActions.append(createGalleryAction("Internet-Explorer", ":/gallery-icon/icon/gallery/Internet-Explorer.svg"));
    galleryActions.append(createGalleryAction("Illustrator", ":/gallery-icon/icon/gallery/Illustrator.svg"));
    galleryActions.append(createGalleryAction("Google-Maps", ":/gallery-icon/icon/gallery/Google-Maps.svg"));
    galleryActions.append(createGalleryAction("Adobe", ":/gallery-icon/icon/gallery/Adobe.svg"));
    galleryActions.append(createGalleryAction("Word", ":/gallery-icon/icon/gallery/Word.svg"));
    SARibbonGalleryGroup* galleryGroup2 = gallery->addCategoryActions(tr("Apps"), galleryActions);
    galleryGroup2->setGridMinimumWidth(80);
    gallery->setCurrentViewGroup(galleryGroup1);

    QAction* galleryOptionAction = new QAction(this);
    galleryOptionAction->setObjectName(("add gallery item"));
    connect(galleryOptionAction, &QAction::triggered, this, [ galleryGroup2 ]() {
        QAction* newGalleryAction =
            new QAction(QIcon(":/gallery-icon/icon/gallery/Document-File.svg"), QString("New Action"));
        galleryGroup2->addActionItem(newGalleryAction);
    });
    panelButtonGroups->setOptionAction(galleryOptionAction);

    SARibbonPanel* panelAppButton = new SARibbonPanel(tr("panel two"));
    panelAppButton->setObjectName("CategoryOther-panel2");
    categoryPage->addPanel(panelAppButton);
    QAction* actionRemoveAppBtn = createAction(tr("remove application button"), ":/icon/icon/remove-app-btn.svg");
    actionRemoveAppBtn->setCheckable(true);
    connect(actionRemoveAppBtn, &QAction::toggled, this, &MainWindow::onActionRemoveAppBtnTriggered);
    panelAppButton->addLargeAction(actionRemoveAppBtn);

    QAction* actionLongText =
        createAction(tr("show very long text in a button,balabalabala etc"), ":/icon/icon/long-text.svg", "long-text");
    panelAppButton->addLargeAction(actionLongText);

    SARibbonPanel* panelStyle = new SARibbonPanel(tr("style"));
    panelStyle->setObjectName("CategoryOther-panelStyle");
    categoryPage->addPanel(panelStyle);

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
    categoryPage->addPanel(panelUtf8);

    QAction* actionChangeText =
        createAction(QStringLiteral(u"改变显示为英文"), ":/icon/icon/chinese-char.svg", "actionChangeText");
    actionChangeText->setCheckable(true);
    actionChangeText->setChecked(false);
    panelUtf8->addLargeAction(actionChangeText);
    connect(actionChangeText, &QAction::triggered, this, [ panelUtf8, actionChangeText ](bool toEnglish) {
        if (toEnglish) {
            panelUtf8->setPanelName(QStringLiteral(u"show chinese char"));
            actionChangeText->setText(QStringLiteral(u"show in chinese"));
        } else {
            panelUtf8->setPanelName(QStringLiteral(u"中文显示测试"));
            actionChangeText->setText(QStringLiteral(u"改变显示为英文"));
        }
    });

    SARibbonPanel* panelContextCategory = new SARibbonPanel(("Context Category"));
    categoryPage->addPanel(panelContextCategory);
    QAction* actionContextCategory1 = createAction("Context Category 1", ":/icon/icon/ContextCategory.svg");
    actionContextCategory1->setCheckable(true);
    connect(actionContextCategory1, &QAction::triggered, this, &MainWindow::onContextCategory1ActionTriggered);

    QAction* actionContextCategory2 = createAction("Context Category 2", ":/icon/icon/ContextCategory.svg");
    actionContextCategory2->setCheckable(true);
    connect(actionContextCategory2, &QAction::triggered, this, &MainWindow::onContextCategory2ActionTriggered);
    panelContextCategory->addLargeAction(actionContextCategory1);
    panelContextCategory->addLargeAction(actionContextCategory2);
}

/**
 * \if ENGLISH
 * @brief Create delete category page content
 * @param categoryPage Delete category page pointer
 * @details Demonstrate how to dynamically create and delete Ribbon category pages.
 *          This function shows how to:
 *          - Remove panels from categories
 *          - Remove categories from the ribbon bar
 *          - Insert panels at specific positions
 * \endif
 *
 * \if CHINESE
 * @brief 创建删除分类页面内容
 * @param categoryPage 删除分类页面指针
 * @details 演示如何动态创建和删除Ribbon分类页面。
 *          此函数演示如何：
 *          - 从分类中移除面板
 *          - 从Ribbon栏中移除分类
 *          - 在特定位置插入面板
 * \endif
 */
void MainWindow::createCategoryDelete(SARibbonCategory* categoryPage)
{
    SARibbonPanel* panelRemoveTest   = new SARibbonPanel(("panel 1"));
    SARibbonPanel* panelCategoryTest = new SARibbonPanel(("panel 2"));

    QAction* actionRemovePanel = createAction(tr("remove panel"), ":/icon/icon/remove.svg");
    connect(actionRemovePanel, &QAction::triggered, this, [ this, categoryPage, panelCategoryTest ]() {
        onRemovePanelActionTriggered(categoryPage, panelCategoryTest);
    });
    panelRemoveTest->addLargeAction(actionRemovePanel);

    QAction* actionRemoveCategory = createAction(tr("remove this category"), ":/icon/icon/removePage.svg");
    connect(actionRemoveCategory, &QAction::triggered, this, [ this, categoryPage, actionRemoveCategory ]() {
        onRemoveCategoryActionTriggered(categoryPage);
        actionRemoveCategory->setDisabled(true);
    });
    panelCategoryTest->addLargeAction(actionRemoveCategory);

    categoryPage->addPanel(panelRemoveTest);
    categoryPage->addPanel(panelCategoryTest);

    SARibbonPanel* panelInsertTest  = categoryPage->addPanel(("insert panel test"));
    QAction* actionInsertPanelAt0   = createAction(tr("insert at 0"), ":/icon/icon/test1.svg");
    QAction* actionInsertPanelAtEnd = createAction(tr("insert at end"), ":/icon/icon/test2.svg");
    QAction* actionInsertPanelAtNeg = createAction(tr("insert at -1"), ":/icon/icon/item.svg");

    connect(actionInsertPanelAt0, &QAction::triggered, this, [ this, categoryPage ]() {
        onInsertPanelAtStartActionTriggered(categoryPage);
    });
    connect(actionInsertPanelAtEnd, &QAction::triggered, this, [ this, categoryPage ]() {
        onInsertPanelAtEndActionTriggered(categoryPage);
    });
    connect(actionInsertPanelAtNeg, &QAction::triggered, this, [ this, categoryPage ]() {
        onInsertPanelAtNegativeOneActionTriggered(categoryPage);
    });

    panelInsertTest->addLargeAction(actionInsertPanelAt0);
    panelInsertTest->addLargeAction(actionInsertPanelAtEnd);
    panelInsertTest->addLargeAction(actionInsertPanelAtNeg);
}

/**
 * \if ENGLISH
 * @brief Create size category page content
 * @param categoryPage Size category page pointer
 * @details Build size adjustment page, demonstrating the use of font and size related controls.
 *          This function shows how to:
 *          - Use font comboboxes in ribbon panels
 *          - Create button groups for font tools
 *          - Adjust font sizes dynamically
 * \endif
 *
 * \if CHINESE
 * @brief 创建尺寸分类页面内容
 * @param categoryPage 尺寸分类页面指针
 * @details 构建尺寸调整页面，展示字体和尺寸相关控件的使用。
 *          此函数演示如何：
 *          - 在Ribbon面板中使用字体组合框
 *          - 创建字体工具的按钮组
 *          - 动态调整字体大小
 * \endif
 */
void MainWindow::createCategorySize(SARibbonCategory* categoryPage)
{
    SARibbonPanel* panelFont                   = categoryPage->addPanel(tr("Font"));
    SARibbonButtonGroupWidget* groupFontFamily = new SARibbonButtonGroupWidget(panelFont);
    groupFontFamily->setObjectName(QStringLiteral(u"group1"));
    QLabel* labelFontSize = new QLabel(groupFontFamily);
    labelFontSize->setText(tr("select font"));
    labelFontSize->setObjectName(QStringLiteral(u"labelFontSize"));
    groupFontFamily->addWidget(labelFontSize);
    QFontComboBox* fontComboBox = new QFontComboBox(groupFontFamily);
    fontComboBox->setObjectName(QStringLiteral(u"fontComboBox"));
    connect(fontComboBox, &QFontComboBox::currentFontChanged, this, &MainWindow::onFontComWidgetCurrentFontChanged);
    groupFontFamily->setWindowTitle(tr("Font ComboBox"));
    groupFontFamily->addWidget(fontComboBox);
    QAction* actionWidgetFontFamily = panelFont->addWidget(groupFontFamily, SARibbonPanelItem::Medium);
    actionWidgetFontFamily->setObjectName(QStringLiteral("Size.Font.FontFamile"));

    SARibbonButtonGroupWidget* groupFontTools = new SARibbonButtonGroupWidget(panelFont);
    groupFontTools->setObjectName(QStringLiteral(u"group2"));
    groupFontTools->addAction(createAction("Bold", ":/icon/icon/bold.svg"));
    groupFontTools->addAction(createAction("Italic", ":/icon/icon/Italic.svg"));
    groupFontTools->addSeparator();
    groupFontTools->addAction(createAction("left alignment", ":/icon/icon/al-left.svg"));
    groupFontTools->addAction(createAction("center alignment", ":/icon/icon/al-center.svg"));
    groupFontTools->addAction(createAction("right alignment", ":/icon/icon/al-right.svg"));
    groupFontTools->addAction(createAction("line up on both sides", ":/icon/icon/al-bothside.svg"));
    groupFontTools->setWindowTitle(tr("Font Tools"));
    QAction* actionWidgetFontTools = panelFont->addWidget(groupFontTools, SARibbonPanelItem::Medium);
    actionWidgetFontTools->setObjectName(QStringLiteral("Size.Font.FontTools"));

    panelFont->addSeparator();

    QAction* actionLargerFontSize = createAction(tr("Larger"), ":/icon/icon/largerFont.svg", "actLargerFontSize");
    panelFont->addLargeAction(actionLargerFontSize);

    QAction* actionSmallFontSize = createAction(tr("Smaller"), ":/icon/icon/smallFont.svg", "actSmallFontSize");
    panelFont->addLargeAction(actionSmallFontSize);

    connect(actionLargerFontSize, &QAction::triggered, this, &MainWindow::onActionFontLargerTriggered);
    connect(actionSmallFontSize, &QAction::triggered, this, &MainWindow::onActionFontSmallerTriggered);

    SARibbonPanel* panelFontSize = new SARibbonPanel(tr("Font Size"));
    QFont panelFontSetting       = font();
    panelFontSetting.setPointSize(14);
    panelFontSize->setFont(panelFontSetting);
    QAction* actionLargeFontText = createAction(tr("Larger Font Text"), ":/icon/icon/file.svg", "actLargeFontText");
    panelFontSize->addLargeAction(actionLargeFontText);
    categoryPage->addPanel(panelFontSize);
}

/**
 * \if ENGLISH
 * @brief Create color category page content
 * @param categoryPage Color category page pointer
 * @details Build color-related page, demonstrating the use of color selection controls.
 *          This function shows how to:
 *          - Use SARibbonColorToolButton for color selection
 *          - Configure different color button styles
 *          - Use SAColorPaletteGridWidget for color palettes
 * \endif
 *
 * \if CHINESE
 * @brief 创建颜色分类页面内容
 * @param categoryPage 颜色分类页面指针
 * @details 构建颜色相关页面，展示颜色选择控件的使用。
 *          此函数演示如何：
 *          - 使用SARibbonColorToolButton进行颜色选择
 *          - 配置不同的颜色按钮样式
 *          - 使用SAColorPaletteGridWidget显示颜色面板
 * \endif
 */
void MainWindow::createCategoryColor(SARibbonCategory* categoryPage)
{
    SARibbonPanel* panelColor = categoryPage->addPanel(tr("color"));

    auto createColorButton = [ this, panelColor ](const QColor& defaultColor = Qt::red) -> SARibbonColorToolButton* {
        SARibbonColorToolButton* colorButton = new SARibbonColorToolButton(panelColor);
        colorButton->setColor(defaultColor);
        colorButton->setupStandardColorMenu();
        this->connect(colorButton, &SARibbonColorToolButton::colorClicked, this, &MainWindow::onColorButtonColorClicked);
        return colorButton;
    };
    SARibbonColorToolButton* colorButton = createColorButton();
    colorButton->setObjectName("ColorFillToIcon-NoIconNoText");
    colorButton->setColorStyle(SARibbonColorToolButton::ColorFillToIcon);
    panelColor->addSmallWidget(colorButton);

    colorButton = createColorButton(Qt::blue);
    colorButton->setColorStyle(SARibbonColorToolButton::ColorFillToIcon);
    colorButton->setObjectName("ColorFillToIcon-NoIconHaveText");
    colorButton->setText("No Icon have text");
    panelColor->addSmallWidget(colorButton);

    colorButton = createColorButton(QColor());
    colorButton->setIcon(QIcon(":/icon/icon/long-text.svg"));
    panelColor->addSmallWidget(colorButton);

    colorButton = createColorButton(Qt::red);
    colorButton->setIcon(QIcon(":/icon/icon/long-text.svg"));
    colorButton->setText("have Icon have text");
    panelColor->addSmallWidget(colorButton);

    colorButton = createColorButton(QColor());
    colorButton->setButtonType(SARibbonToolButton::LargeButton);
    colorButton->setObjectName("ColorFillToIcon-LargeColorButton");
    colorButton->setColorStyle(SARibbonColorToolButton::ColorFillToIcon);
    colorButton->setText("Large Color Button");
    panelColor->addLargeWidget(colorButton);

    colorButton = createColorButton(QColor());
    colorButton->setButtonType(SARibbonToolButton::LargeButton);
    colorButton->setIcon(QIcon(":/icon/icon/long-text.svg"));
    colorButton->setObjectName("ColorUnderIcon-LargeColorButton");
    colorButton->setText("Large Color Button");
    panelColor->addLargeWidget(colorButton);

    panelColor->addSeparator();
    SAColorPaletteGridWidget* colorPaletteWidget = new SAColorPaletteGridWidget(SA::getStandardColorList(), panelColor);
    colorPaletteWidget->setFactor({ 75, 120 });
    panelColor->addLargeWidget(colorPaletteWidget);
}

/**
 * \if ENGLISH
 * @brief Create context category 1
 * @details Create context-related category pages, which are usually displayed under specific conditions.
 *          Context categories are special tabs that appear when certain objects or modes are active.
 * \endif
 *
 * \if CHINESE
 * @brief 创建上下文分类1
 * @details 创建上下文相关的分类页面，这类页面通常在特定条件下才显示。
 *          上下文分类是当某些对象或模式处于活动状态时出现的特殊标签页。
 * \endif
 */
void MainWindow::createContextCategory1()
{
    SARibbonBar* currentRibbonBar = ribbonBar();
    mContextCategory              = currentRibbonBar->addContextCategory(tr("context"), QColor(), 1);

    SARibbonCategory* contextCategoryPage1 = mContextCategory->addCategoryPage(tr("Page1"));
    createContextCategoryPage1(contextCategoryPage1);

    SARibbonCategory* contextCategoryPage2 = mContextCategory->addCategoryPage(tr("Page2"));
    createContextCategoryPage2(contextCategoryPage2);
}

/**
 * \if ENGLISH
 * @brief Create context category 2
 * @details Create another context tag, demonstrating the management of multiple context tags.
 * \endif
 *
 * \if CHINESE
 * @brief 创建上下文分类2
 * @details 创建另一个上下文标签，演示多个上下文标签的管理。
 * \endif
 */
void MainWindow::createContextCategory2()
{
    SARibbonBar* currentRibbonBar = ribbonBar();
    mContextCategory2             = currentRibbonBar->addContextCategory(("context2"), QColor(), 2);
    mContextCategory2->addCategoryPage(("context2 Page1"));
    mContextCategory2->addCategoryPage(("context2 Page2"));
}

/**
 * \if ENGLISH
 * @brief Create first context category page content
 * @param categoryPage First context category page pointer
 * @details Build content for the first context category page, demonstrating the unique features of context pages.
 *          This function shows how to:
 *          - Control action visibility
 *          - Enable/disable actions dynamically
 *          - Use keyboard shortcuts in context pages
 * \endif
 *
 * \if CHINESE
 * @brief 创建第一个上下文分类页面内容
 * @param categoryPage 第一个上下文分类页面指针
 * @details 构建第一个上下文分类页面的内容，演示上下文页面的特有功能。
 *          此函数演示如何：
 *          - 控制动作可见性
 *          - 动态启用/禁用动作
 *          - 在上下文页面中使用键盘快捷键
 * \endif
 */
void MainWindow::createContextCategoryPage1(SARibbonCategory* categoryPage)
{
    SARibbonPanel* panelShowHide = categoryPage->addPanel(tr("show and hide test"));

    QAction* actionHideActions = createAction("hide action", ":/icon/icon/hidePanel.svg");
    actionHideActions->setCheckable(true);
    actionHideActions->setChecked(true);
    panelShowHide->addLargeAction(actionHideActions);
    connect(actionHideActions, &QAction::triggered, this, &MainWindow::onActionHideActionTriggered);

    mActionDisable = createAction(tr("Disable"), ":/icon/icon/enableTest.svg");
    mActionDisable->setDisabled(true);
    panelShowHide->addLargeAction(mActionDisable);
    connect(mActionDisable, &QAction::triggered, this, [ this ](bool triggered) {
        Q_UNUSED(triggered);
        mActionDisable->setDisabled(true);
    });

    mActionUnlock = createAction(tr("unlock"), ":/icon/icon/unlock.svg");
    mActionUnlock->setShortcut(QKeySequence(QLatin1String("Ctrl+E")));
    mActionUnlock->setShortcutContext(Qt::ApplicationShortcut);
    panelShowHide->addLargeAction(mActionUnlock);
    connect(mActionUnlock, &QAction::triggered, this, &MainWindow::onUnlockActionTriggered);

    QAction* actionShortText = createAction(tr("1"), ":/icon/icon/unlock.svg");
    actionShortText->setToolTip(tr("very short string"));
    panelShowHide->addLargeAction(actionShortText);

    mActionSetTextTest = createAction("set text", ":/icon/icon/setText.svg");
    mActionSetTextTest->setCheckable(true);
    mActionSetTextTest->setShortcut(QKeySequence(QLatin1String("Ctrl+D")));
    mActionSetTextTest->setShortcutContext(Qt::ApplicationShortcut);
    panelShowHide->addLargeAction(mActionSetTextTest);
    connect(mActionSetTextTest, &QAction::toggled, this, &MainWindow::onSetTextActionToggled);

    mActionShowTest = createAction("show beside panel", ":/icon/icon/show.svg");

    mActionShowTest->setCheckable(true);
    panelShowHide->addLargeAction(mActionShowTest);

    mPanelVisbileExample = categoryPage->addPanel(tr("show/hide"));
    mPanelVisbileExample->addLargeAction(mActionSetTextTest);

    connect(mActionShowTest, &QAction::toggled, this, [ this ](bool visible) {
        mPanelVisbileExample->setVisible(!visible);
        if (visible) {
            mActionShowTest->setText(tr("hide beside panel"));
        } else {
            mActionShowTest->setText(tr("show beside panel"));
        }
        ribbonBar()->repaint();
    });

    SARibbonPanel* panelShowHideAction = categoryPage->addPanel(("show/hide action test"));

    mActionHideAction2 = createAction("hide action 2", ":/icon/icon/action.svg");
    mActionHideAction4 = createAction("hide action 4", ":/icon/icon/action.svg");
    mActionHideAction2->setCheckable(true);
    mActionHideAction2->setChecked(true);
    mActionHideAction4->setCheckable(true);
    mActionHideAction4->setChecked(true);
    mActionHideShowTextAct2 = createAction("action2", ":/icon/icon/action2.svg");
    mActionHideShowTextAct3 = createAction("action3", ":/icon/icon/action3.svg");
    mActionHideShowTextAct4 = createAction("action4", ":/icon/icon/action4.svg");
    panelShowHideAction->addLargeAction(mActionHideAction2);
    panelShowHideAction->addLargeAction(mActionHideAction4);
    panelShowHideAction->addSmallAction(mActionHideShowTextAct2);
    panelShowHideAction->addSmallAction(mActionHideShowTextAct3);
    panelShowHideAction->addSmallAction(mActionHideShowTextAct4);
    connect(mActionHideAction2, &QAction::triggered, this, &MainWindow::onHideAction2Triggered);
    connect(mActionHideAction4, &QAction::triggered, this, &MainWindow::onHideAction4Triggered);

    SARibbonPanel* panelWidget            = categoryPage->addPanel(("widget"));
    QSpinBox* spinBoxWidget               = new QSpinBox(this);
    SARibbonCtrlContainer* ctrlContainer1 = new SARibbonCtrlContainer(panelWidget);
    ctrlContainer1->setContainerWidget(spinBoxWidget);
    ctrlContainer1->setText(tr("spinbox:"));
    ctrlContainer1->setEnableShowIcon(false);
    ctrlContainer1->setMaximumHeight(fontMetrics().lineSpacing() * 1.5);
    panelWidget->addMediumWidget(ctrlContainer1);

    QLineEdit* lineEditWidget             = new QLineEdit(this);
    SARibbonCtrlContainer* ctrlContainer2 = new SARibbonCtrlContainer(panelWidget);
    ctrlContainer2->setContainerWidget(lineEditWidget);
    ctrlContainer2->setText(tr("linedit:"));
    ctrlContainer2->setEnableShowIcon(false);
    ctrlContainer2->setMaximumHeight(fontMetrics().lineSpacing() * 1.5);
    panelWidget->addMediumWidget(ctrlContainer2);
}

/**
 * \if ENGLISH
 * @brief Create second context category page content
 * @param categoryPage Second context category page pointer
 * @details Build content for the second context category page, demonstrating special layouts and control combinations.
 *          This function shows how to:
 *          - Delete categories dynamically
 *          - Create custom panel layouts with different item sizes
 *          - Use separators in panels
 * \endif
 *
 * \if CHINESE
 * @brief 创建第二个上下文分类页面内容
 * @param categoryPage 第二个上下文分类页面指针
 * @details 构建第二个上下文分类页面的内容，展示特殊布局和控件组合。
 *          此函数演示如何：
 *          - 动态删除分类
 *          - 创建具有不同项目大小的自定义面板布局
 *          - 在面板中使用分隔符
 * \endif
 */
void MainWindow::createContextCategoryPage2(SARibbonCategory* categoryPage)
{
    SARibbonPanel* panelDeleteTest = categoryPage->addPanel(("删除CategoryPage测试"));

    QAction* actionDeleteThisCategory = createAction("delete this category", ":/icon/icon/delete.svg");
    panelDeleteTest->addLargeAction(actionDeleteThisCategory);
    connect(actionDeleteThisCategory, &QAction::triggered, this, [ this, categoryPage ]() {
        onDeleteThisCategoryActionTriggered(categoryPage);
    });

    SARibbonPanel* panelLayout = categoryPage->addPanel(("特殊布局"));

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
 * \if ENGLISH
 * @brief Create quick access toolbar
 * @details Build quick access toolbar, placing commonly used operation buttons.
 *          The quick access toolbar is located at the top of the ribbon bar and provides
 *          quick access to frequently used commands.
 * \endif
 *
 * \if CHINESE
 * @brief 创建快速访问工具栏
 * @details 构建快速访问工具栏，放置常用的操作按钮。
 *          快速访问工具栏位于Ribbon栏的顶部，提供对常用命令的快速访问。
 * \endif
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

    connect(actionUndo, &QAction::triggered, this, &MainWindow::onUndoActionTriggered);
    connect(actionRedo, &QAction::triggered, this, &MainWindow::onRedoActionTriggered);

    quickAccessBar->addSeparator();

    QMenu* menuPresentationFile1 = new QMenu("Presentation File 1", this);
    menuPresentationFile1->setIcon(QIcon(":/icon/icon/presentationFile.svg"));
    for (int fileIndex = 0; fileIndex < 10; ++fileIndex) {
        menuPresentationFile1->addAction(createAction(QString("file%1 - 1").arg(fileIndex + 1), ":/icon/icon/file.svg"));
    }
    quickAccessBar->addMenuAction(menuPresentationFile1->menuAction());

    QMenu* menuPresentationFile2 = new QMenu("Presentation File 2", this);
    menuPresentationFile2->setIcon(QIcon(":/icon/icon/presentationFile.svg"));
    for (int fileIndex = 0; fileIndex < 10; ++fileIndex) {
        menuPresentationFile2->addAction(createAction(QString("file%1 - 2").arg(fileIndex + 1), ":/icon/icon/file.svg"));
    }
    quickAccessBar->addMenuAction(menuPresentationFile2, QToolButton::MenuButtonPopup);

    QAction* actionCustomize = createAction("customize", ":/icon/icon/customize0.svg", "customize2");
    quickAccessBar->addAction(actionCustomize);
    connect(actionCustomize, &QAction::triggered, this, &MainWindow::onActionCustomizeTriggered);

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
    connect(mSearchEditor, &QLineEdit::editingFinished, this, &MainWindow::onSearchEditorEditingFinished);
}

/**
 * \if ENGLISH
 * @brief Create right button group
 * @details RightButtonGroup is a toolbar on the right side of the ribbonbar where you can place some shortcut icons.
 *          This is useful for placing help buttons, settings, and other frequently accessed commands.
 * \endif
 *
 * \if CHINESE
 * @brief 创建右侧按钮组
 * @details RightButtonGroup是在Ribbon栏右边的工具栏，可以放置一些快捷图标。
 *          这对于放置帮助按钮、设置和其他经常访问的命令非常有用。
 * \endif
 */
void MainWindow::createRightButtonGroup()
{
    SARibbonBar* currentRibbonBar = ribbonBar();
    if (!currentRibbonBar) {
        return;
    }
    SARibbonButtonGroupWidget* rightBar = currentRibbonBar->rightButtonGroup();
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
 * \if ENGLISH
 * @brief Create window button group bar
 * @details Add custom tool buttons next to system window buttons (minimize, maximize, close).
 *          This allows you to add custom buttons to the title bar area.
 * \endif
 *
 * \if CHINESE
 * @brief 创建窗口按钮组栏
 * @details 在系统窗口按钮（最小化、最大化、关闭）旁边添加自定义工具按钮。
 *          这允许您在标题栏区域添加自定义按钮。
 * \endif
 */
void MainWindow::createWindowButtonGroupBar()
{
    SARibbonSystemButtonBar* windowButtonBar = this->windowButtonBar();
    if (!windowButtonBar) {
        return;
    }
    QAction* actionLogin = new QAction(QIcon(), tr("Login"), this);
    QAction* actionHelp  = createAction(tr("help"), ":/icon/icon/help.svg");
    connect(actionLogin, &QAction::triggered, this, &MainWindow::onLoginActionTriggered);
    connect(actionHelp, &QAction::triggered, this, &MainWindow::onActionHelpTriggered);
    windowButtonBar->addAction(actionLogin);
    windowButtonBar->addAction(actionHelp);
}

/**
 * \if ENGLISH
 * @brief Create ActionsManager
 * @details Create ActionsManager to manage actions and customize SARibbonBar.
 *          The ActionsManager is essential for:
 *          - Managing all actions in the application
 *          - Enabling ribbon customization features
 *          - Categorizing actions for the customize dialog
 * \endif
 *
 * \if CHINESE
 * @brief 创建动作管理器
 * @details 创建ActionsManager来管理动作并自定义SARibbonBar。
 *          ActionsManager对于以下功能至关重要：
 *          - 管理应用程序中的所有动作
 *          - 启用Ribbon自定义功能
 *          - 为自定义对话框分类动作
 * \endif
 */
void MainWindow::createActionsManager()
{
    createOtherActions();
    mTagForActionText = SARibbonActionsManager::UserDefineActionTag + 1;
    mTagForActionIcon = SARibbonActionsManager::UserDefineActionTag + 2;

    mActionsManager = new SARibbonActionsManager(ribbonBar());

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

    mActionsManager->setTagName(SARibbonActionsManager::CommonlyUsedActionTag, tr("in common use"));
    mActionsManager->setTagName(mTagForActionText, tr("no icon action"));
    mActionsManager->setTagName(mTagForActionIcon, tr("have icon action"));
}

/**
 * \if ENGLISH
 * @brief Create an action with text, icon, and object name
 * @param text Action text
 * @param iconPath Icon path
 * @param objName Object name
 * @return Created action pointer
 * @details This is a helper function to create QAction objects with consistent settings.
 *          The object name is important for action management and customization.
 * \endif
 *
 * \if CHINESE
 * @brief 创建带有文本、图标和对象名称的动作
 * @param text 动作文本
 * @param iconPath 图标路径
 * @param objName 对象名称
 * @return 创建的动作指针
 * @details 这是一个辅助函数，用于创建具有一致设置的QAction对象。
 *          对象名称对于动作管理和自定义非常重要。
 * \endif
 */
QAction* MainWindow::createAction(const QString& text, const QString& iconPath, const QString& objName)
{
    QAction* newAction = new QAction(this);
    newAction->setText(text);
    newAction->setIcon(QIcon(iconPath));
    newAction->setObjectName(objName);
    return newAction;
}

/**
 * \if ENGLISH
 * @brief Create an action with text and icon (overloaded version)
 * @param text Action text
 * @param iconPath Icon path
 * @return Created action pointer
 * @details This overloaded version auto-generates an object name based on the text.
 * \endif
 *
 * \if CHINESE
 * @brief 创建带有文本和图标的动作（重载版本）
 * @param text 动作文本
 * @param iconPath 图标路径
 * @return 创建的动作指针
 * @details 此重载版本根据文本自动生成对象名称。
 * \endif
 */
QAction* MainWindow::createAction(const QString& text, const QString& iconPath)
{
    QAction* newAction = new QAction(this);
    newAction->setText(text);
    newAction->setIcon(QIcon(iconPath));
    newAction->setObjectName(text);
    return newAction;
}

/**
 * \if ENGLISH
 * @brief Test the checkable function in menu button popup mode
 * @param checked Whether the button is checked
 * \endif
 *
 * \if CHINESE
 * @brief 测试菜单按钮弹出模式下的可检查功能
 * @param checked 是否选中
 * \endif
 */
void MainWindow::onMenuButtonPopupCheckableTest(bool checked)
{
    ui->textBrowser->append(QString("MenuButtonPopupCheckableTest : %1").arg(checked));
}

/**
 * \if ENGLISH
 * @brief Test the checkable function in instant popup mode
 * @param checked Whether the button is checked
 * \endif
 *
 * \if CHINESE
 * @brief 测试即时弹出模式下的可检查功能
 * @param checked 是否选中
 * \endif
 */
void MainWindow::onInstantPopupCheckableTest(bool checked)
{
    ui->textBrowser->append(QString("InstantPopupCheckableTest : %1").arg(checked));
}

/**
 * \if ENGLISH
 * @brief Test the checkable function in delayed popup mode
 * @param checked Whether the button is checked
 * \endif
 *
 * \if CHINESE
 * @brief 测试延迟弹出模式下的可检查功能
 * @param checked 是否选中
 * \endif
 */
void MainWindow::onDelayedPopupCheckableTest(bool checked)
{
    ui->textBrowser->append(QString("DelayedPopupCheckableTest : %1").arg(checked));
}

/**
 * \if ENGLISH
 * @brief Handle trigger events for checkable actions in menu button popup mode
 * @param checked Whether the button is checked
 * \endif
 *
 * \if CHINESE
 * @brief 处理菜单按钮弹出模式下可检查动作的触发事件
 * @param checked 是否选中
 * \endif
 */
void MainWindow::onMenuButtonPopupCheckabletriggered(bool checked)
{
    ui->textBrowser->append(QString("MenuButtonPopupCheckabletriggered : %1").arg(checked));
}

/**
 * \if ENGLISH
 * @brief Handle trigger events for checkable actions in instant popup mode
 * @param checked Whether the button is checked
 * \endif
 *
 * \if CHINESE
 * @brief 处理即时弹出模式下可检查动作的触发事件
 * @param checked 是否选中
 * \endif
 */
void MainWindow::onInstantPopupCheckabletriggered(bool checked)
{
    ui->textBrowser->append(QString("InstantPopupCheckabletriggered : %1").arg(checked));
}

/**
 * \if ENGLISH
 * @brief Handle trigger events for checkable actions in delayed popup mode
 * @param checked Whether the button is checked
 * @details This also demonstrates how to use SARibbonToolButton::setPopupMode(QToolButton::DelayedPopup)
 *          to set the menu pop-up method to delayed pop-up.
 * \endif
 *
 * \if CHINESE
 * @brief 处理延迟弹出模式下可检查动作的触发事件
 * @param checked 是否选中
 * @details 这也演示了如何使用SARibbonToolButton::setPopupMode(QToolButton::DelayedPopup)
 *          方法设置菜单弹出方式为延迟弹出。
 * \endif
 */
void MainWindow::onDelayedPopupCheckabletriggered(bool checked)
{
    Q_UNUSED(checked);
    ui->textBrowser->append(tr("The SARibbonToolButton::setPopupMode(QToolButton::DelayedPopup) method "
                               "can be used to set the menu pop-up method to delayed pop-up. "
                               "This also demonstrates manually setting text wrapping"));
}
