#ifndef RIBBONWINDOWAGENT_H
#define RIBBONWINDOWAGENT_H
#include "SARibbonQmlGlobal.h"
#include <QQuickItem>
#include <QQuickWindow>
#include <QObject>

namespace QWK {
class QuickWindowAgent;
}

namespace SARibbonQml {

/**
 * \if ENGLISH
 * @brief Frameless window agent for the QML front end (QWindowKit Quick route)
 * @details The QML counterpart of the widgets SARibbonMainWindow frameless
 *          plumbing: this host owns a QWK::QuickWindowAgent, exposes setup()
 *          for a QQuickWindow, and publishes the system-button strip
 *          geometry the RibbonBar reserves on its right edge. Declare it as
 *          a child of RibbonBar and the bar wires everything automatically
 *          (title bar delegation, hit-test registration, strip reservation);
 *          the system buttons themselves render through the leaf as ordinary
 *          QML items and register here as QWK system buttons so the native
 *          window manager recognizes them (Snap Layout, hover animations).
 * \endif
 *
 * \if CHINESE
 * @brief QML 前端的无边框窗口代理（QWindowKit Quick 路线）
 * @details 对应 widgets 侧 SARibbonMainWindow 的无边框装配：本宿主持有一个
 *          QWK::QuickWindowAgent，暴露 setup() 供 QQuickWindow 使用，并发布
 *          RibbonBar 右缘需要预留的系统按钮条几何。声明为 RibbonBar 的子项后
 *          bar 会自动完成全部接线（标题栏代理、命中测试登记、右侧预留）；
 *          系统按钮本身由叶子渲染为普通 QML item，并在此登记为 QWK 系统
 *          按钮，原生窗口管理器因此能识别它们（Snap Layout、悬停动画）。
 * \endif
 */
class SA_RIBBON_QML_EXPORT RibbonWindowAgent : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQuickWindow* window READ window NOTIFY windowChanged)
    Q_PROPERTY(int buttonWidth READ buttonWidth WRITE setButtonWidth NOTIFY buttonWidthChanged)
    Q_PROPERTY(bool framelessEnabled READ isFramelessEnabled WRITE setFramelessEnabled NOTIFY framelessEnabledChanged)
    Q_PROPERTY(QQuickItem* titleBarItem READ titleBarItem WRITE setTitleBarItem NOTIFY titleBarItemChanged)
public:
    explicit RibbonWindowAgent(QObject* parent = nullptr);
    ~RibbonWindowAgent() override;

    // Attach the agent to a QQuickWindow; idempotent (re-setup returns false)
    Q_INVOKABLE bool setup(QQuickWindow* window);

    // Detach from the current window (destructor does this automatically)
    Q_INVOKABLE void release();

    // The attached window (null before setup)
    QQuickWindow* window() const;

    // Standard system button glyph width; the button row height follows the
    // bar's title row. Widgets SARibbonSystemButtonBar stretch parity: close
    // is wider than max/min (4:3:3 over 3x width)
    int buttonWidth() const;
    void setButtonWidth(int w);

    // Whether the frameless decoration is active; off keeps the native frame
    // and the bar reserves nothing (tests and offscreen platforms)
    bool isFramelessEnabled() const;
    void setFramelessEnabled(bool on);

    // The item acting as the draggable title bar (usually the RibbonBar
    // itself); QWK excludes every registered child button from the drag area
    QQuickItem* titleBarItem() const;
    void setTitleBarItem(QQuickItem* item);

    // Register a QML-rendered system button with QWK; kind is one of
    // "minimize", "maximize", "close" (window-icon buttons are not used by
    // the ribbon bar). Returns false when QWK rejected the item
    Q_INVOKABLE bool setSystemButton(const QString& kind, QQuickItem* item);

    // Mark an interactive item as click-through for the frameless hit test
    // (widgets setFramelessHitTestVisible parity)
    Q_INVOKABLE void setHitTestVisible(QQuickItem* item, bool visible = true);

    // Native dark-mode attribute for the window (QWK "dark-mode" attribute);
    // keeps the DWM frame and caption consistent with RibbonTheme
    Q_INVOKABLE void setDarkMode(bool on);

    // Computed strip geometry the RibbonBar reserves (0 when disabled)
    int stripWidth() const;

Q_SIGNALS:
    void windowChanged();
    void buttonWidthChanged();
    void framelessEnabledChanged();
    void titleBarItemChanged();

private:
    QWK::QuickWindowAgent* mAgent = nullptr;
    QQuickWindow* mWindow = nullptr;
    QQuickItem* mTitleBarItem = nullptr;
    int mButtonWidth = 35;
    bool mFramelessEnabled = true;
};

}

#endif  // RIBBONWINDOWAGENT_H
