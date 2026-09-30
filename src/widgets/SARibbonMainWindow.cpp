#include "SARibbonMainWindow.h"
#include "SARibbonUtil.h"
#include "SARibbonBar.h"
#include "SARibbonElementManager.h"
#include "SARibbonTabBar.h"
#include "SARibbonThemeManager.h"
#include "SARibbonThemePalette.h"
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QHash>
#include <QPainter>
#include <QPen>
#include <QWindowStateChangeEvent>
#include <QScreen>
#include <QTimer>

#include "SARibbonSystemButtonBar.h"
#include "SARibbonWidget.h"
#include "SARibbonTitleIconWidget.h"
#if SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
#include <QWKWidgets/widgetwindowagent.h>
#include "SARibbonButtonGroupWidget.h"
#include "SARibbonQuickAccessBar.h"
#include "SARibbonStackedWidget.h"
#else
#include "SAFramelessHelper.h"
#include "SARibbonButtonGroupWidget.h"
#include "SARibbonQuickAccessBar.h"
#include "SARibbonTabBar.h"
#endif
#if defined(Q_OS_WIN) && !SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#endif

/**
 * @brief The SARibbonMainWindowPrivate class
 */
namespace {
// 主题对应的内置调色板路径（与 SARibbonThemeManager.cpp / SARibbonUtil.cpp 的同名映射一致），
// 用于边框色跟随主题时解析 border-color token
QString mainWindowThemePalettePath(SARibbonTheme theme)
{
    switch (theme) {
    case SARibbonTheme::RibbonThemeOffice2016Blue:
        return ":/SARibbonTheme/resource/palettes/office2016-blue.json";
    case SARibbonTheme::RibbonThemeOffice2016Green:
        return ":/SARibbonTheme/resource/palettes/office2016-green.json";
    case SARibbonTheme::RibbonThemeOffice2016Dark:
        return ":/SARibbonTheme/resource/palettes/office2016-dark.json";
    case SARibbonTheme::RibbonThemeOffice2021Blue:
        return ":/SARibbonTheme/resource/palettes/office2021-blue.json";
    case SARibbonTheme::RibbonThemeOffice2021Green:
        return ":/SARibbonTheme/resource/palettes/office2021-green.json";
    case SARibbonTheme::RibbonThemeOffice2021Dark:
        return ":/SARibbonTheme/resource/palettes/office2021-dark.json";
    case SARibbonTheme::RibbonThemeDark:
        return ":/SARibbonTheme/resource/palettes/dark-default.json";
    case SARibbonTheme::RibbonThemeDark2:
        return ":/SARibbonTheme/resource/palettes/dark2-default.json";
    case SARibbonTheme::RibbonThemeWindows7:
        return ":/SARibbonTheme/resource/palettes/win7-default.json";
    case SARibbonTheme::RibbonThemeOffice2013:
        return ":/SARibbonTheme/resource/palettes/office2013-default.json";
    default:
        return QString();
    }
}
}  // namespace

namespace SA {
/**
 * \if ENGLISH
 * @brief Title-bar draggable area hit test for the Windows non-QWK frameless path
 * \endif
 *
 * \if CHINESE
 * @brief Windows 非 QWK 无边框路径的标题栏可拖拽区命中测试
 * \endif
 */
bool isTitleBarDragArea(const QPoint& localPos,
                        const QRect& windowRect,
                        int titleHeight,
                        const QList< QRect >& excludedRects,
                        bool maximizedOrFullscreen)
{
    if (maximizedOrFullscreen) {
        // 最大化/全屏时不返回 HTCAPTION，避免"最大化状态下拖动窗口"的怪异行为
        return false;
    }
    if (titleHeight <= 0 || !windowRect.contains(localPos)) {
        return false;
    }
    const QRect titleBarRect(windowRect.left(), windowRect.top(), windowRect.width(), titleHeight);
    if (!titleBarRect.contains(localPos)) {
        return false;
    }
    // 排除可点击控件区域（系统按钮、快速访问栏、tab 栏、应用按钮等）
    for (const QRect& r : excludedRects) {
        if (r.isValid() && r.contains(localPos)) {
            return false;
        }
    }
    return true;
}
}  // namespace SA

class SARibbonMainWindow::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonMainWindow)
public:
    PrivateData(SARibbonMainWindow* p);
    void installFrameless(SARibbonMainWindow* p);
    bool isUseRibbonBar() const;
    bool isUseRibbonFrame() const;
    bool isUseNativeFrame() const;
    void checkMainWindowFlag();

public:
    SARibbonMainWindowStyles mRibbonMainWindowStyle;
    SARibbonTheme mCurrentRibbonTheme { SARibbonTheme::RibbonThemeOffice2021Blue };
    SARibbonSystemButtonBar* mWindowButtonGroup { nullptr };
    bool mFrameBorderEnabled { false };  ///< 是否绘制 1px 窗口边框（默认关闭保持现行为）
    QColor mFrameBorderColor;            ///< 自定义边框颜色，无效色表示跟随主题
    bool mFrameShadowEnabled { false };  ///< 是否启用 DWM 系统阴影（仅 Windows 非 QWK 路径）
#if SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
    QWK::WidgetWindowAgent* mFramelessHelper { nullptr };
#else
    SAFramelessHelper* mFramelessHelper { nullptr };
#endif
    SARibbonMainWindowEventFilter* mEventFilter { nullptr };
};

SARibbonMainWindow::PrivateData::PrivateData(SARibbonMainWindow* p) : q_ptr(p)
{
}

void SARibbonMainWindow::PrivateData::installFrameless(SARibbonMainWindow* p)
{
#if SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
    mFramelessHelper = new QWK::WidgetWindowAgent(p);
    mFramelessHelper->setup(p);
#else
    mFramelessHelper = new SAFramelessHelper(p);
#endif
}

bool SARibbonMainWindow::PrivateData::isUseRibbonBar() const
{
    return mRibbonMainWindowStyle.testFlag(SARibbonMainWindowStyleFlag::UseRibbonMenuBar);
}

bool SARibbonMainWindow::PrivateData::isUseRibbonFrame() const
{
    return mRibbonMainWindowStyle.testFlag(SARibbonMainWindowStyleFlag::UseRibbonFrame);
}

bool SARibbonMainWindow::PrivateData::isUseNativeFrame() const
{
    return mRibbonMainWindowStyle.testFlag(SARibbonMainWindowStyleFlag::UseNativeFrame);
}

/**
 * @brief 检查flag的设置合理性
 */
void SARibbonMainWindow::PrivateData::checkMainWindowFlag()
{
    // 如果都没有设置边框样式，默认设置为ribbon边框
    if (!mRibbonMainWindowStyle.testFlag(SARibbonMainWindowStyleFlag::UseRibbonFrame)
        && !mRibbonMainWindowStyle.testFlag(SARibbonMainWindowStyleFlag::UseNativeFrame)) {
        mRibbonMainWindowStyle.setFlag(SARibbonMainWindowStyleFlag::UseRibbonFrame, true);
    }

    // 如果都没有设置MenuBar，默认设置为ribbonbar
    if (!mRibbonMainWindowStyle.testFlag(SARibbonMainWindowStyleFlag::UseRibbonMenuBar)
        && !mRibbonMainWindowStyle.testFlag(SARibbonMainWindowStyleFlag::UseNativeMenuBar)) {
        mRibbonMainWindowStyle.setFlag(SARibbonMainWindowStyleFlag::UseRibbonMenuBar, true);
    }
}

//===================================================
// SARibbonMainWindow
//===================================================

/**
 * \if ENGLISH
 * @brief Constructs a SARibbonMainWindow instance.
 *
 * This constructor initializes a main window with Ribbon interface style, supporting custom window styles (such as whether to use Ribbon border, menu bar, etc.),
 * and automatically configures window behavior (such as borderless, RibbonBar installation, etc.) according to the style.
 *
 * @param parent Parent widget
 * @param style Window style flags, controlling window appearance and behavior. The following flag combinations are supported:
 *        - @c SARibbonMainWindowStyleFlag::UseRibbonFrame: Use Ribbon custom border (enables borderless window) (enabled by default).
 *        - @c SARibbonMainWindowStyleFlag::UseNativeFrame: Use system native window border.
 *        - @c SARibbonMainWindowStyleFlag::UseRibbonMenuBar: Use Ribbon style menu bar (enabled by default).
 *        - @c SARibbonMainWindowStyleFlag::UseNativeMenuBar: Use system native menu bar (non-ribbon).
 *        Flags can be combined using bitwise OR (|), for example: @c SARibbonMainWindowStyleFlag::UseRibbonFrame | SARibbonMainWindowStyleFlag::UseRibbonMenuBar
 *
 * @param flags Standard Qt window flags
 *
 * @note If @c UseRibbonFrame is enabled, the window will automatically install borderless support.
 *
 * @sa SARibbonMainWindowStyleFlag, setRibbonBar(), ribbonTheme()
 * \endif
 *
 * \if CHINESE
 * @brief 构造一个 SARibbonMainWindow 实例。
 *
 * 此构造函数初始化一个带有 Ribbon 界面风格的主窗口，支持自定义窗口样式（如是否使用 Ribbon 边框、菜单栏等），
 * 并根据样式自动配置窗口行为（如无边框、RibbonBar 安装等）。
 *
 * @param parent 父窗口部件
 * @param style 窗口样式标志，控制窗口外观和行为。支持以下标志组合：
 *        - @c SARibbonMainWindowStyleFlag::UseRibbonFrame：使用Ribbon自定义边框（会启用无边框窗口）（默认启用）。
 *        - @c SARibbonMainWindowStyleFlag::UseNativeFrame：使用系统原生窗口边框。
 *        - @c SARibbonMainWindowStyleFlag::UseRibbonMenuBar：使用 Ribbon 风格菜单栏（默认启用）。
 *        - @c SARibbonMainWindowStyleFlag::UseNativeMenuBar：使用系统原生菜单栏（非ribbon）。
 *        标志可通过位或（|）组合使用，例如：@c SARibbonMainWindowStyleFlag::UseRibbonFrame | SARibbonMainWindowStyleFlag::UseRibbonMenuBar
 *
 * @param flags 标准 Qt 窗口标志
 *
 * @note 如果启用了 @c UseRibbonFrame，则窗口将自动安装无边框支持。
 *
 * @sa SARibbonMainWindowStyleFlag, setRibbonBar(), ribbonTheme()
 * \endif
 */
SARibbonMainWindow::SARibbonMainWindow(QWidget* parent, SARibbonMainWindowStyles style, const Qt::WindowFlags flags)
    : QMainWindow(parent, flags), d_ptr(new SARibbonMainWindow::PrivateData(this))
{
    SA_D(d);
    d->mRibbonMainWindowStyle = style;
    d->checkMainWindowFlag();
    if (d->isUseRibbonBar()) {
        if (d->isUseRibbonFrame()) {
            d->installFrameless(this);
        }
        setRibbonBar(createRibbonBar());
        // 系统暗色模式自动切换，可通过 SA::setEnableSystemDarkModeAutoSwitch(false) 关闭
        if (SA::isEnableSystemDarkModeAutoSwitch() && SA::isOperatingSystemInDarkMode()
            && d->mCurrentRibbonTheme == SARibbonTheme::RibbonThemeOffice2021Blue) {
            d->mCurrentRibbonTheme = SARibbonTheme::RibbonThemeDark;
        }
        SARibbonTheme themeAtConstruction = d->mCurrentRibbonTheme;
        // 同步应用主题：确保 QSS 在窗口 show() 之前就挂载，避免首帧绘制默认灰底后重绘造成的闪现
        SA::applyRibbonTheme(this, ribbonBar(), themeAtConstruction);
        // 兜底：部分 Qt 版本在构造函数中应用样式可能未完全生效，事件循环启动后重应用一次
        QTimer::singleShot(0, this, [this, themeAtConstruction]() {
            SA_D(d);
            SARibbonTheme t = ribbonTheme();
            if (d->mCurrentRibbonTheme == themeAtConstruction) {
                SA::applyRibbonTheme(this, ribbonBar(), t);
            } else {
                setRibbonTheme(t);
            }
        });
        setContentsMargins(2, 0, 2, 0);
        if (d->isUseNativeFrame()) {
            // 在ribbon模式下使用本地边框，将隐藏icon，同时默认设置为紧凑模式
            if (SARibbonBar* bar = ribbonBar()) {
                // 隐藏icon
                bar->setTitleIconVisible(false);
                // 设置为紧凑模式
                bar->setRibbonStyle(SARibbonBar::RibbonStyleCompactThreeRow);
            }
        }
    }
    connect(qApp, &QApplication::primaryScreenChanged, this, &SARibbonMainWindow::onPrimaryScreenChanged);
}

/**
 * \if ENGLISH
 * @brief Destructor
 * \endif
 *
 * \if CHINESE
 * @brief 析构函数
 * \endif
 */
SARibbonMainWindow::~SARibbonMainWindow()
{
}

/**
 * \if ENGLISH
 * @brief Returns the ribbonbar, returns nullptr if not using ribbon mode
 * @return Pointer to the SARibbonBar, or nullptr if not using ribbon mode
 * \endif
 *
 * \if CHINESE
 * @brief 返回ribbonbar，如果不是使用ribbon模式，返回nullptr
 * @return 指向SARibbonBar的指针，如果不是使用ribbon模式，返回nullptr
 * \endif
 */
SARibbonBar* SARibbonMainWindow::ribbonBar() const
{
    return qobject_cast< SARibbonBar* >(menuWidget());
}

/**
 * \if ENGLISH
 * @brief Sets the ribbonbar
 * @param ribbon Pointer to the SARibbonBar to set
 * \endif
 *
 * \if CHINESE
 * @brief 设置ribbonbar
 * @param ribbon 要设置的SARibbonBar指针
 * \endif
 */
void SARibbonMainWindow::setRibbonBar(SARibbonBar* ribbon)
{
    Q_ASSERT(ribbon != nullptr);
    QWidget* old = QMainWindow::menuWidget();
    if (old) {
        // 如果之前已经设置了menubar，要把之前的删除
        old->deleteLater();
    }
    QMainWindow::setMenuWidget(ribbon);
    ribbon->setMainWindowStyles(d_ptr->mRibbonMainWindowStyle);
    const int th = ribbon->titleBarHeight();
    // 先提升ribbon，避免被别的窗口覆盖
    ribbon->raise();
    if (d_ptr->isUseRibbonFrame()) {
        // 设置window按钮
        if (nullptr == d_ptr->mWindowButtonGroup) {
            d_ptr->mWindowButtonGroup = RibbonSubElementFactory->createWindowButtonGroup(this);
            d_ptr->mWindowButtonGroup->setObjectName(QStringLiteral("objSARibbonSystemButtonBar"));
            d_ptr->mWindowButtonGroup->setIconSize(QSize(18, 18));
            // SARibbonSystemButtonBar的eventfilter捕获mainwindow的事件
            // 通过eventerfilter来处理mainwindow的事件，避免用户错误的继承resizeEvent导致systembar的位置异常
            installEventFilter(d_ptr->mWindowButtonGroup);
        } else {
            // 避免重复 installEventFilter（Qt 文档：同一 filter 多次 install 会被调用多次）
            removeEventFilter(d_ptr->mWindowButtonGroup);
            installEventFilter(d_ptr->mWindowButtonGroup);
        }

        SARibbonSystemButtonBar* sysBar = d_ptr->mWindowButtonGroup;
        sysBar->setWindowStates(windowState());
        sysBar->setWindowTitleHeight(th);
        // 确保sysbar在最顶层，避免第二次设置ribbonbar的时候，被ribbonbar覆盖了sysbar
        sysBar->raise();
        sysBar->show();

        // 图标
        ribbon->titleIconWidget()->setIcon(windowIcon());
#if SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
        auto helper = d_ptr->mFramelessHelper;
        helper->setTitleBar(ribbon);
        // 以下这些窗口，需要允许点击
        helper->setHitTestVisible(sysBar);                         // IMPORTANT!
        helper->setHitTestVisible(ribbon->ribbonTabBar());         // IMPORTANT!
        helper->setHitTestVisible(ribbon->rightButtonGroup());     // IMPORTANT!
        helper->setHitTestVisible(ribbon->applicationButton());    // IMPORTANT!
        helper->setHitTestVisible(ribbon->quickAccessBar());       // IMPORTANT!
        helper->setHitTestVisible(ribbon->ribbonStackedWidget());  // IMPORTANT!
        helper->setHitTestVisible(ribbon->titleIconWidget());      // IMPORTANT!
#if SARIBBON_ENABLE_SNAP_LAYOUT
        if (sysBar->closeButton()) {
            helper->setSystemButton(QWK::WindowAgentBase::Close, sysBar->closeButton());
        }
        if (sysBar->minimizeButton()) {
            helper->setSystemButton(QWK::WindowAgentBase::Minimize, sysBar->minimizeButton());
        }
        if (sysBar->maximizeButton()) {
            helper->setSystemButton(QWK::WindowAgentBase::Maximize, sysBar->maximizeButton());
        }
#endif
#else
        // 捕获ribbonbar的事件
        ribbon->installEventFilter(this);
        // 设置窗体的标题栏高度
        d_ptr->mFramelessHelper->setTitleHeight(th);
        d_ptr->mFramelessHelper->setRubberBandOnResize(false);
#endif
        // 最后要提升，否则新加入的会被覆盖
        if (d_ptr->mWindowButtonGroup) {
            d_ptr->mWindowButtonGroup->raise();
        }
    }
    if (!d_ptr->mEventFilter) {
        d_ptr->mEventFilter = new SARibbonMainWindowEventFilter(this);
        installEventFilter(d_ptr->mEventFilter);
    }
}

#if SARIBBON_USE_3RDPARTY_FRAMELESSHELPER

/**
 * \if ENGLISH
 * @brief If there are custom windows in the ribbon added to non-clickable areas such as the title bar, and you want them to be clickable, you need to call this interface to inform that they are clickable
 * @param w The widget to set as clickable
 * @param visible Whether the widget should be clickable
 * \endif
 *
 * \if CHINESE
 * @brief 如果ribbon中有自定义的窗口在标题栏等非点击区域加入后，想能点击，需要调用此接口告知可点击
 * @param w 要设置为可点击的窗口部件
 * @param visible 窗口部件是否应该可点击
 * \endif
 */
void SARibbonMainWindow::setFramelessHitTestVisible(QWidget* w, bool visible)
{
    auto helper = d_ptr->mFramelessHelper;
    helper->setHitTestVisible(const_cast< QWidget* >(w), visible);
}
#else

/**
 * \if ENGLISH
 * @brief Borderless helper object
 * @return Pointer to the SAFramelessHelper
 * \endif
 *
 * \if CHINESE
 * @brief 无边框辅助对象
 * @return 指向SAFramelessHelper的指针
 * \endif
 */
SAFramelessHelper* SARibbonMainWindow::framelessHelper() const
{
    return (d_ptr->mFramelessHelper);
}

/**
 * \if ENGLISH
 * @brief Sets whether to enable "rubber band" indication mode during scaling.
 *
 * When enabled, the window does not immediately redraw its content during drag scaling, but first uses a semi-transparent rectangle (rubber band)
 * to display the target size, and only completes the resize once the user releases the mouse. This mode can significantly reduce
 * CPU/GPU consumption during high-frequency resize operations for heavy-load applications such as CAD, 3D rendering, and large charts.
 *
 * @param on  true  Enable rubber band scaling indication;
 *             false Disable rubber band, use real-time redrawing (default behavior).
 *
 * @see isRubberBandOnResize(), SAFramelessHelper::setRubberBandOnResize()
 * \endif
 *
 * \if CHINESE
 * @brief 设置在缩放时是否启用“橡皮筋”示意模式。
 *
 * 当启用时，窗口在拖拽缩放过程中不会立即重绘内容，而是先用一个半透明矩形框（橡皮筋）
 * 显示目标尺寸，待用户释放鼠标后才一次性完成resize。此模式可显著降低
 * CAD、三维渲染、大型图表等重负载应用在高频resize时的CPU/GPU消耗。
 *
 * @param on  true  启用橡皮筋缩放示意；
 *             false 禁用橡皮筋，采用实时重绘（默认行为）。
 *
 * @see isRubberBandOnResize(), SAFramelessHelper::setRubberBandOnResize()
 * \endif
 */
void SARibbonMainWindow::setRubberBandOnResize(bool on)
{
    if (SAFramelessHelper* fl = framelessHelper()) {
        fl->setRubberBandOnResize(on);
    }
}

/**
 * \if ENGLISH
 * @brief Returns whether the rubber band scaling indication mode is currently enabled.
 *
 * @return true  Rubber band mode is enabled;
 *         false Rubber band mode is not enabled
 *
 * @see setRubberBandOnResize(), SAFramelessHelper::rubberBandOnResize()
 * \endif
 *
 * \if CHINESE
 * @brief 返回当前是否启用了橡皮筋缩放示意模式。
 *
 * @return true  橡皮筋模式已启用；
 *         false 橡皮筋模式未启用
 *
 * @see setRubberBandOnResize(), SAFramelessHelper::rubberBandOnResize()
 * \endif
 */
bool SARibbonMainWindow::isRubberBandOnResize() const
{
    if (SAFramelessHelper* fl = framelessHelper()) {
        return fl->rubberBandOnResisze();
    }
    return false;
}
#endif

/**
 * \if ENGLISH
 * @brief Event filter for handling events from the ribbon bar
 *
 * This event filter is used to pass events from the ribbon bar to the main window and then to the frameless helper.
 * Since the ribbon bar may cover the frameless area, causing the frameless helper to fail to capture these messages,
 * it is necessary to call ribbonBar()->installEventFilter(this).
 *
 * @param obj The object that is sending the event
 * @param e The event being sent
 * @return true if the event was handled, false otherwise
 * \endif
 *
 * \if CHINESE
 * @brief 用于处理来自ribbon bar的事件的事件过滤器
 *
 * 此事件过滤器用于将ribbon bar上的事件传递到mainwindow，再传递到frameless helper。
 * 由于ribbonbar会遮挡掉frameless的区域，导致frameless无法捕获这些消息，
 * 因此必须调用ribbonBar()->installEventFilter(this)。
 *
 * @param obj 发送事件的对象
 * @param e 正在发送的事件
 * @return 如果事件已处理则返回true，否则返回false
 * \endif
 */
bool SARibbonMainWindow::eventFilter(QObject* obj, QEvent* e)
{
#if SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
#else
    // 这个过滤是为了把ribbonBar上的动作传递到mainwindow，再传递到frameless，
    // 由于ribbonbar会遮挡掉frameless的区域，导致frameless无法捕获这些消息
    // 因此必须ribbonBar()->installEventFilter(this);

    if (obj == ribbonBar()) {
        switch (e->type()) {
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
        case QEvent::Leave:
        case QEvent::HoverMove:
        case QEvent::MouseButtonDblClick: {
            QApplication::sendEvent(this, e);
        } break;
        default:
            break;
        }
    }
#endif
    return (QMainWindow::eventFilter(obj, e));
}

/**
 * \if ENGLISH
 * @brief Gets the system button group, you can add other buttons based on this
 * @return Pointer to the SARibbonSystemButtonBar
 * \endif
 *
 * \if CHINESE
 * @brief 获取系统按钮组，可以在此基础上添加其他按钮
 * @return 指向SARibbonSystemButtonBar的指针
 * \endif
 */
SARibbonSystemButtonBar* SARibbonMainWindow::windowButtonBar() const
{
    return d_ptr->mWindowButtonGroup;
}

/**
 * \if ENGLISH
 * @brief Gets the current style of the ribbon main window
 * @return The current SARibbonMainWindowStyles
 * \endif
 *
 * \if CHINESE
 * @brief 获取当前ribbonMainWidow的样式
 * @return 当前的SARibbonMainWindowStyles
 * \endif
 */
SARibbonMainWindowStyles SARibbonMainWindow::ribbonMainwindowStyle() const
{
    return d_ptr->mRibbonMainWindowStyle;
}

/**
 * \if ENGLISH
 * @brief This function is only used to control the display of minimize, maximize, and close buttons
 * @param flags The window flags to update
 * \endif
 *
 * \if CHINESE
 * @brief 此函数仅用于控制最小最大化和关闭按钮的显示
 * @param flags 要更新的窗口标志
 * \endif
 */
void SARibbonMainWindow::updateWindowFlag(Qt::WindowFlags flags)
{
    if (d_ptr->isUseRibbonFrame()) {
        if (SARibbonSystemButtonBar* g = d_ptr->mWindowButtonGroup) {
            g->updateWindowFlag(flags);
        }
    }
}

/**
 * \if ENGLISH
 * @brief Sets the ribbon theme
 *
 * Note that in some versions of Qt, setting the theme in the constructor may not take full effect. You can use QTimer to put it at the end of the queue, like:
 * @code
 * QTimer::singleShot(0, this, [ this ]() { this->setRibbonTheme(SARibbonMainWindow::RibbonThemeDark); });
 * @endcode
 *
 * @param theme The theme to set
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonMainWindow::setRibbonTheme
 *
 * 注意某些版本的qt，在构造函数设置主题会不完全生效，可以使用QTimer投放到队列最后执行，如：
 * @code
 * QTimer::singleShot(0, this, [ this ]() { this->setRibbonTheme(SARibbonMainWindow::RibbonThemeDark); });
 * @endcode
 *
 * @param theme 要设置的主题
 * \endif
 */
void SARibbonMainWindow::setRibbonTheme(SARibbonTheme theme)
{
    if (d_ptr->mCurrentRibbonTheme != theme) {
        d_ptr->mCurrentRibbonTheme = theme;
        SA::applyRibbonTheme(this, ribbonBar(), theme);
        Q_EMIT ribbonThemeChanged(theme);
        // 主题切换会重设样式表；边框色跟随主题时需重绘
        if (d_ptr->mFrameBorderEnabled && !d_ptr->mFrameBorderColor.isValid()) {
            update();
        }
    }
}

/**
 * \if ENGLISH
 * @brief Checks whether the 1px window frame border is drawn
 * @return true if the frame border is drawn
 * \endif
 *
 * \if CHINESE
 * @brief 查询是否绘制 1px 窗口边框
 * @return 绘制边框时返回 true
 * \endif
 */
bool SARibbonMainWindow::isFrameBorderEnabled() const
{
    return d_ptr->mFrameBorderEnabled;
}

/**
 * \if ENGLISH
 * @brief Enables/disables drawing of the 1px window frame border
 * @param on true to draw the border, false to keep the default appearance
 * @details Useful for frameless windows placed over same-colored backgrounds where the
 *          window boundary is otherwise invisible. Default is off.
 * \endif
 *
 * \if CHINESE
 * @brief 开启/关闭 1px 窗口边框的绘制
 * @param on true 绘制边框，false 保持默认外观
 * @details 适用于无边框窗口落在同色背景（如同为白色的文档区或桌面）上边界不可辨的场景。默认关闭
 * \endif
 */
void SARibbonMainWindow::setFrameBorderEnabled(bool on)
{
    if (d_ptr->mFrameBorderEnabled == on) {
        return;
    }
    d_ptr->mFrameBorderEnabled = on;
    Q_EMIT frameBorderEnabledChanged(on);
    update();
}

/**
 * \if ENGLISH
 * @brief Gets the custom frame border color
 * @return The custom color; an invalid QColor means "follow current theme"
 * \endif
 *
 * \if CHINESE
 * @brief 获取自定义边框颜色
 * @return 自定义颜色；无效的 QColor 表示"跟随当前主题"
 * \endif
 */
QColor SARibbonMainWindow::frameBorderColor() const
{
    return d_ptr->mFrameBorderColor;
}

/**
 * \if ENGLISH
 * @brief Sets a custom frame border color
 * @param color The color to use; pass an invalid QColor to follow the theme's border-color token
 * \endif
 *
 * \if CHINESE
 * @brief 设置自定义边框颜色
 * @param color 使用的颜色；传入无效 QColor 表示跟随主题的 border-color 色板
 * \endif
 */
void SARibbonMainWindow::setFrameBorderColor(const QColor& color)
{
    if (d_ptr->mFrameBorderColor == color) {
        return;
    }
    d_ptr->mFrameBorderColor = color;
    Q_EMIT frameBorderColorChanged(color);
    if (d_ptr->mFrameBorderEnabled) {
        update();
    }
}

#if defined(Q_OS_WIN) && !SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
namespace {
// dwmapi 动态解析（照抄 QWK qwkwindowsextra_p.h 的做法，不引入链接期依赖）
typedef HRESULT(WINAPI* DwmExtendFrameIntoClientAreaPtr)(HWND, const MARGINS*);
typedef HRESULT(WINAPI* DwmIsCompositionEnabledPtr)(BOOL*);

DwmExtendFrameIntoClientAreaPtr dwmExtendFrameIntoClientArea()
{
    static DwmExtendFrameIntoClientAreaPtr fn = nullptr;
    static bool resolved = false;
    if (!resolved) {
        resolved = true;
        HMODULE dwm = ::LoadLibraryW(L"dwmapi.dll");
        if (dwm) {
            fn = reinterpret_cast< DwmExtendFrameIntoClientAreaPtr >(
                ::GetProcAddress(dwm, "DwmExtendFrameIntoClientArea"));
        }
    }
    return fn;
}

bool isDwmCompositionEnabled()
{
    DwmIsCompositionEnabledPtr fn = nullptr;
    HMODULE dwm                   = ::LoadLibraryW(L"dwmapi.dll");
    if (dwm) {
        fn = reinterpret_cast< DwmIsCompositionEnabledPtr >(::GetProcAddress(dwm, "DwmIsCompositionEnabled"));
    }
    if (!fn) {
        return false;
    }
    BOOL enabled = FALSE;
    return SUCCEEDED(fn(&enabled)) && enabled;
}

// 给无边框窗口补 WS_THICKFRAME（可调整尺寸边框），DWM 阴影与系统 resize 光标依赖它；
// 不加 WS_CAPTION，避免系统标题栏回来
void applyShadowStyle(HWND hwnd, bool on)
{
    const LONG_PTR style = ::GetWindowLongPtrW(hwnd, GWL_STYLE);
    const LONG_PTR wanted = on ? (style | WS_THICKFRAME) : (style & ~WS_THICKFRAME);
    if (wanted != style) {
        ::SetWindowLongPtrW(hwnd, GWL_STYLE, wanted);
        // 触发非客户区重算
        ::SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
                       SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

// 应用 DWM 阴影：DwmExtendFrameIntoClientArea 负责把 frame 扩入客户区（阴影随之出现）
void applyDwmShadow(HWND hwnd, bool on)
{
    auto fn = dwmExtendFrameIntoClientArea();
    if (!fn || !isDwmCompositionEnabled()) {
        return;
    }
    MARGINS margins = on ? MARGINS { 0, 0, 0, 1 }  // 底部 1px：保留阴影所需的 frame 痕迹
                         : MARGINS { 0, 0, 0, 0 };
    fn(hwnd, &margins);
}
}  // namespace
#endif

/**
 * \if ENGLISH
 * @brief Checks whether the DWM system shadow is enabled for the frameless window
 * @return true if enabled
 * @note Windows only (non-QWK path); always false on other platforms and on the QWK path
 * \endif
 *
 * \if CHINESE
 * @brief 查询无边框窗口是否启用了 DWM 系统阴影
 * @return 启用时返回 true
 * @note 仅 Windows（非 QWK 路径）有效；其他平台与 QWK 路径恒为 false
 * \endif
 */
bool SARibbonMainWindow::isFrameShadowEnabled() const
{
    return d_ptr->mFrameShadowEnabled;
}

/**
 * \if ENGLISH
 * @brief Enables the DWM system shadow for the frameless window
 * @param on true to enable the shadow
 * @details Windows non-QWK path: adds WS_THICKFRAME and extends the DWM frame into the client
 *          area, so the window gets the standard system shadow. WM_NCCALCSIZE/WM_NCACTIVATE are
 *          handled in nativeEvent() to keep the client area covering the whole window (no system
 *          title bar/border appears). No-op on non-Windows platforms and the QWK path (QWK has
 *          its own shadow handling). Note: the system shadow is not drawn when the window is
 *          maximized — that is a Windows behavior, not a bug.
 * \endif
 *
 * \if CHINESE
 * @brief 为无边框窗口启用 DWM 系统阴影
 * @param on true 启用阴影
 * @details Windows 非 QWK 路径：加 WS_THICKFRAME 并把 DWM frame 扩入客户区，窗口获得标准
 *          系统阴影。WM_NCCALCSIZE/WM_NCACTIVATE 在 nativeEvent() 中配套处理，客户区仍然
 *          铺满整个窗口（不会出现系统标题栏/边框）。非 Windows 平台与 QWK 路径为空操作
 *          （QWK 有自己的阴影处理）。注意：窗口最大化时系统不绘制阴影——这是 Windows 的
 *          行为，不是 bug。
 * \endif
 */
void SARibbonMainWindow::setFrameShadowEnabled(bool on)
{
    if (d_ptr->mFrameShadowEnabled == on) {
        return;
    }
    d_ptr->mFrameShadowEnabled = on;
    Q_EMIT frameShadowEnabledChanged(on);
#if defined(Q_OS_WIN) && !SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
    if (!testAttribute(Qt::WA_WState_Created) || !testAttribute(Qt::WA_WState_Visible)) {
        return;  // 窗口未创建/未显示，nativeEvent 会在显示后按状态应用
    }
    if (WId hwnd = winId()) {
        applyShadowStyle(reinterpret_cast< HWND >(hwnd), on);
        applyDwmShadow(reinterpret_cast< HWND >(hwnd), on);
    }
#endif
}

/**
 * \if ENGLISH
 * @brief Draws the optional 1px frame border and then the default window content
 * @param e Paint event
 * @details The border is only drawn when isFrameBorderEnabled() is true. Color resolution order:
 *          custom frameBorderColor() if valid, then the theme palette's "border-color" token,
 *          finally a fallback of palette window color darkened.
 * \endif
 *
 * \if CHINESE
 * @brief 绘制可选的 1px 边框，随后执行默认的窗口内容绘制
 * @param e 绘制事件
 * @details 仅当 isFrameBorderEnabled() 为真时绘制。取色顺序：自定义 frameBorderColor() 有效优先，
 *          其次主题调色板的 border-color 色板，最后退回 palette 窗口色加深
 * \endif
 */
void SARibbonMainWindow::paintEvent(QPaintEvent* e)
{
    if (d_ptr->mFrameBorderEnabled) {
        QPainter painter(this);
        QColor border = d_ptr->mFrameBorderColor;
        if (!border.isValid()) {
            // 跟随主题：从当前主题的调色板取 border-color token
            SA::SARibbonThemePalette themePalette;
            const QString palettePath = mainWindowThemePalettePath(d_ptr->mCurrentRibbonTheme);
            if (!palettePath.isEmpty() && themePalette.loadFromFile(palettePath)) {
                border = themePalette.color("border-color");
            }
        }
        if (!border.isValid()) {
            border = palette().color(QPalette::Window).darker(120);
        }
        QPen pen(border, 1);
        painter.setPen(pen);
        // rect().adjusted(0,0,-1,-1)：画在客户区内缘，画在 rect() 外侧会被裁掉
        painter.drawRect(rect().adjusted(0, 0, -1, -1));
    }
    QMainWindow::paintEvent(e);
}

#if defined(Q_OS_WIN) && !SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
/**
 * \if ENGLISH
 * @brief Windows non-QWK path: returns HTCAPTION for the title bar draggable area
 * @param eventType Native event type name
 * @param message Native message (MSG on Windows)
 * @param result Output: the native hit test result to return
 * @return true if the message is consumed
 * @details Returning HTCAPTION lets Windows take over the title bar drag loop, which provides
 *          the system Aero Snap behavior (drag to left/right screen edge shows the half-screen
 *          preview, drag to the top shows the maximize preview). Interactive child widgets
 *          (system buttons, quick access bar, right button group, application button, tab bar,
 *          title icon) are excluded so they keep receiving mouse events. Measured on Qt 5.15:
 *          QWidget::nativeEvent() receives WM_NCHITTEST (unlike the global native event filter,
 *          which only sees non-input messages). The result pointer type is long on Qt5 and
 *          qintptr on Qt6 (different types on MSVC x64), so the definition below switches
 *          its signature with QT_VERSION.
 * \endif
 *
 * \if CHINESE
 * @brief Windows 非 QWK 路径：标题栏可拖拽区返回 HTCAPTION
 * @param eventType 原生事件类型名
 * @param message 原生消息（Windows 上为 MSG）
 * @param result 输出：返回给系统的命中测试结果
 * @return true 表示消息已消费
 * @details 返回 HTCAPTION 后 Windows 接管标题栏拖拽循环，系统免费提供 Aero Snap
 *          （拖到屏幕左/右边缘出半屏预览、拖到顶部出最大化预览）。可交互子控件
 *          （系统按钮、快速访问栏、右侧按钮组、应用按钮、tab 栏、标题图标）被排除，
 *          保持正常接收鼠标事件。Qt 5.15 实测：QWidget::nativeEvent() 能收到
 *          WM_NCHITTEST（全局原生事件过滤器只看得到非输入消息，local filter 无此限制）。
 *          result 指针类型 Qt5 为 long、Qt6 为 qintptr（MSVC x64 上为不同类型），
 *          下方实现按 QT_VERSION 区分签名
 * \endif
 */
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
bool SARibbonMainWindow::nativeEvent(const QByteArray& eventType, void* message, qintptr* result)
#else
bool SARibbonMainWindow::nativeEvent(const QByteArray& eventType, void* message, long* result)
#endif
{
    if (eventType == "windows_generic_MSG" && message) {
        MSG* msg = static_cast< MSG* >(message);
        if (d_ptr->mFrameShadowEnabled) {
            // ---- DWM 阴影配套处理（issue #129）----
            if (msg->message == WM_NCCALCSIZE && msg->wParam) {
                // 加了 WS_THICKFRAME 后系统会预留边框区，这里把客户区还原为铺满整个窗口：
                // 先记下 top，跑 DefWindowProc（应用默认 frame，保住左/右/下边框与阴影），
                // 再恢复 top（去掉系统标题栏）——做法与 QWK nonClientCalcSizeHandler 一致
                auto* params = reinterpret_cast< LPNCCALCSIZE_PARAMS >(msg->lParam);
                const LONG originalTop = params->rgrc[ 0 ].top;
                const LRESULT defResult = ::DefWindowProcW(msg->hwnd, WM_NCCALCSIZE, msg->wParam, msg->lParam);
                params->rgrc[ 0 ].top  = originalTop;
                // 最大化时窗口实际尺寸比屏幕大一圈（resize 手柄在屏外），需裁剪，
                // 否则内容超出屏幕边界显示不全
                if (isMaximized() && !isFullScreen()) {
                    const UINT dpi = ::GetDpiForWindow(msg->hwnd);
                    const int frameSize = ::GetSystemMetricsForDpi(SM_CXSIZEFRAME, dpi)
                                        + ::GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);
                    params->rgrc[ 0 ].top += frameSize;
                }
                *result = 0;
                return true;
            } else if (msg->message == WM_NCACTIVATE) {
                // 防失活白边：lParam 设 -1 让系统不重绘 frame（经典 frameless 做法）
                *result = ::DefWindowProcW(msg->hwnd, WM_NCACTIVATE, msg->wParam, -1);
                return true;
            }
        }
        if (msg->message == WM_NCHITTEST) {
            // lParam 是屏幕物理坐标：ScreenToClient 转为窗口本地物理坐标，
            // 再除以 devicePixelRatioF 得到本地逻辑坐标（Qt 的逻辑↔物理映射关系）
            POINT pt = { GET_X_LPARAM(msg->lParam), GET_Y_LPARAM(msg->lParam) };
            ::ScreenToClient(msg->hwnd, &pt);
            const qreal dpr = devicePixelRatioF();
            const QPoint localLogical(qRound(pt.x / dpr), qRound(pt.y / dpr));
            // 收集需要保持可点击的控件区域（语义同 QWK 路径的 setHitTestVisible 清单）
            QList< QRect > excluded;
            SARibbonBar* rb = ribbonBar();
            const QWidget* candidates[] = {
                qobject_cast< QWidget* >(d_ptr->mWindowButtonGroup),
                rb ? qobject_cast< QWidget* >(rb->quickAccessBar()) : nullptr,
                rb ? qobject_cast< QWidget* >(rb->rightButtonGroup()) : nullptr,
                rb ? qobject_cast< QWidget* >(rb->applicationButton()) : nullptr,
                rb ? qobject_cast< QWidget* >(rb->titleIconWidget()) : nullptr,
                rb ? qobject_cast< QWidget* >(rb->ribbonTabBar()) : nullptr
            };
            for (const QWidget* w : candidates) {
                if (w && w->isVisible()) {
                    excluded.append(QRect(mapFromGlobal(w->mapToGlobal(QPoint(0, 0))),
                                          w->rect().size()));
                }
            }
            const int titleHeight = ribbonBar() ? ribbonBar()->titleBarHeight() : 0;
            if (SA::isTitleBarDragArea(localLogical, rect(), titleHeight, excluded,
                                       isMaximized() || isFullScreen())) {
                *result = HTCAPTION;
                return true;
            }
        }
    }
    return QMainWindow::nativeEvent(eventType, message, result);
}

/**
 * \if ENGLISH
 * @brief Applies the pending DWM shadow state once the native window exists
 * @param e Show event
 * \endif
 *
 * \if CHINESE
 * @brief 原生窗口创建后应用待生效的 DWM 阴影状态
 * @param e 显示事件
 * \endif
 */
void SARibbonMainWindow::showEvent(QShowEvent* e)
{
    QMainWindow::showEvent(e);
    if (d_ptr->mFrameShadowEnabled) {
        if (WId hwnd = winId()) {
            applyShadowStyle(reinterpret_cast< HWND >(hwnd), true);
            applyDwmShadow(reinterpret_cast< HWND >(hwnd), true);
        }
    }
}
#endif

/**
 * \if ENGLISH
 * @brief Gets the current ribbon theme
 * @return The current SARibbonTheme
 * \endif
 *
 * \if CHINESE
 * @brief 获取当前的ribbon主题
 * @return 当前的SARibbonTheme
 * \endif
 */
SARibbonTheme SARibbonMainWindow::ribbonTheme() const
{
    return (d_ptr->mCurrentRibbonTheme);
}

/**
 * \if ENGLISH
 * @brief Checks if ribbon is being used
 * @return true if ribbon is being used, false otherwise
 * \endif
 *
 * \if CHINESE
 * @brief 检查是否使用ribbon
 * @return 如果使用ribbon则返回true，否则返回false
 * \endif
 */
bool SARibbonMainWindow::isUseRibbon() const
{
    return (nullptr != ribbonBar());
}

/**
 * \if ENGLISH
 * @brief Factory function for creating ribbon bar
 *
 * If the user has overridden SARibbonBar, they can return their own Ribbon instance by overriding this virtual function
 * @return Pointer to the created SARibbonBar
 * \endif
 *
 * \if CHINESE
 * @brief 创建ribbonbar的工厂函数
 *
 * 用户如果重写了SARibbonBar，可以通过重新此虚函数返回自己的Ribbon实例
 * @return 指向创建的SARibbonBar的指针
 * \endif
 */
SARibbonBar* SARibbonMainWindow::createRibbonBar()
{
    SARibbonBar* bar = RibbonSubElementFactory->createRibbonBar(this);
    return bar;
}

/**
 * \if ENGLISH
 * @brief Signal triggered when the primary screen changes
 * @param screen The new primary screen
 * \endif
 *
 * \if CHINESE
 * @brief 主屏幕切换触发的信号
 * @param screen 新的主屏幕
 * \endif
 */
void SARibbonMainWindow::onPrimaryScreenChanged(QScreen* screen)
{
    Q_UNUSED(screen);
    // 主屏幕切换后，从新计算所有尺寸
    if (SARibbonBar* bar = ribbonBar()) {
        qDebug() << "Primary Screen Changed";
        bar->updateRibbonGeometry();
    }
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    // 主屏切换可能伴随 DPI 变化，窗口尺寸不变时系统按钮栏不会收到 resize 事件，
    // 主动发送屏幕变化事件触发其重算几何（issue #118）
    if (d_ptr->mWindowButtonGroup) {
        QEvent ev(QEvent::ScreenChangeInternal);
        QCoreApplication::sendEvent(this, &ev);
    }
#endif
}

//----------------------------------------------------
// SARibbonMainWindowEventFilter
//----------------------------------------------------
SARibbonMainWindowEventFilter::SARibbonMainWindowEventFilter(QObject* par) : QObject(par)
{
}

SARibbonMainWindowEventFilter::~SARibbonMainWindowEventFilter()
{
}

bool SARibbonMainWindowEventFilter::eventFilter(QObject* obj, QEvent* e)
{
    if (e && obj) {
        if (e->type() == QEvent::Resize) {
            if (SARibbonMainWindow* m = qobject_cast< SARibbonMainWindow* >(obj)) {
                if (SARibbonBar* ribbon = m->ribbonBar()) {
                    QMargins mg = m->contentsMargins();
                    ribbon->setFixedWidth(m->size().width() - mg.left() - mg.right());
                }
            }
        }
    }
    return QObject::eventFilter(obj, e);
}
