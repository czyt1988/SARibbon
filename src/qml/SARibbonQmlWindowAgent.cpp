#include "SARibbonQmlWindowAgent.h"
#include <QWKCore/windowagentbase.h>
#include <QWKQuick/quickwindowagent.h>
#include <QQuickItem>
#include <QQuickWindow>

namespace SARibbonQml {

RibbonWindowAgent::RibbonWindowAgent(QObject* parent) : QObject(parent)
{
    mAgent = new QWK::QuickWindowAgent(this);
}

RibbonWindowAgent::~RibbonWindowAgent()
{
    // nothing extra: the agent is a QObject child, teardown follows the
    // parent automatically; QWK uninstalls its filters in its own destructor
}

bool RibbonWindowAgent::setup(QQuickWindow* window)
{
    if (!window || mWindow == window) {
        return false;
    }
    if (mWindow) {
        release();
    }
    if (!mFramelessEnabled) {
        mWindow = window;
        Q_EMIT windowChanged();
        return true;
    }
    if (!mAgent->setup(window)) {
        return false;
    }
    mWindow = window;
    // re-apply the title bar item on the new window
    if (mTitleBarItem) {
        mAgent->setTitleBar(mTitleBarItem);
    }
    Q_EMIT windowChanged();
    return true;
}

void RibbonWindowAgent::release()
{
    if (!mWindow) {
        return;
    }
    // QWK has no public detach besides destroying the agent; recreate it to
    // drop every native hook from the old window before forgetting it
    mAgent->deleteLater();
    mAgent = new QWK::QuickWindowAgent(this);
    mWindow = nullptr;
    Q_EMIT windowChanged();
}

QQuickWindow* RibbonWindowAgent::window() const
{
    return mWindow;
}

int RibbonWindowAgent::buttonWidth() const
{
    return mButtonWidth;
}

void RibbonWindowAgent::setButtonWidth(int w)
{
    w = qMax(w, 16);
    if (mButtonWidth == w) {
        return;
    }
    mButtonWidth = w;
    Q_EMIT buttonWidthChanged();
}

bool RibbonWindowAgent::isFramelessEnabled() const
{
    return mFramelessEnabled;
}

void RibbonWindowAgent::setFramelessEnabled(bool on)
{
    if (mFramelessEnabled == on) {
        return;
    }
    QQuickWindow* win = mWindow;
    if (win) {
        // (re-)route the native decoration through the new flag: release()
        // drops every QWK hook, setup() re-attaches when enabled again.
        // The disabled attach keeps the window remembered (no native hooks,
        // window()/title contracts stay alive for the bar).
        release();
    }
    mFramelessEnabled = on;
    if (win) {
        setup(win);
    }
    Q_EMIT framelessEnabledChanged();
}

QQuickItem* RibbonWindowAgent::titleBarItem() const
{
    return mTitleBarItem;
}

void RibbonWindowAgent::setTitleBarItem(QQuickItem* item)
{
    if (mTitleBarItem == item) {
        return;
    }
    mTitleBarItem = item;
    if (mWindow && mFramelessEnabled) {
        mAgent->setTitleBar(item);
    }
    Q_EMIT titleBarItemChanged();
}

bool RibbonWindowAgent::setSystemButton(const QString& kind, QQuickItem* item)
{
    if (!mWindow || !mFramelessEnabled || !item) {
        return false;
    }
    QWK::WindowAgentBase::SystemButton button;
    if (kind == QLatin1String("minimize")) {
        button = QWK::WindowAgentBase::Minimize;
    } else if (kind == QLatin1String("maximize")) {
        button = QWK::WindowAgentBase::Maximize;
    } else if (kind == QLatin1String("close")) {
        button = QWK::WindowAgentBase::Close;
    } else {
        return false;
    }
    mAgent->setSystemButton(button, item);
    return true;
}

void RibbonWindowAgent::setHitTestVisible(QQuickItem* item, bool visible)
{
    if (!item || !mWindow || !mFramelessEnabled) {
        return;
    }
    mAgent->setHitTestVisible(item, visible);
}

void RibbonWindowAgent::setDarkMode(bool on)
{
    if (!mWindow || !mFramelessEnabled) {
        return;
    }
    mAgent->setWindowAttribute(QStringLiteral("dark-mode"), on);
}

int RibbonWindowAgent::stripWidth() const
{
    // widgets SARibbonSystemButtonBar stretch 4:3:3 over 3*buttonWidth:
    // close 40%, max 30%, min 30% — 105px at the default 35px width.
    // The reservation follows the frameless flag: a native-frame window
    // (disabled agent) reserves nothing, exactly like the widgets layouts
    // only subtract the strip under isUseRibbonFrame()
    if (!mFramelessEnabled) {
        return 0;
    }
    return 3 * mButtonWidth;
}

}
