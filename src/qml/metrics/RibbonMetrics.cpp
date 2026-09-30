#include "RibbonMetrics.h"
#include <QGuiApplication>
#include <QEvent>
#include <QFontMetrics>
#include <QFontDatabase>

namespace SARibbonQml {

RibbonMetrics::RibbonMetrics(QObject* parent) : QObject(parent), mMetrics(QFontMetrics(QFont()), 1.0)
{
    if (auto* app = QGuiApplication::instance()) {
        app->installEventFilter(this);
    }
    rebuild();
}

RibbonMetrics::~RibbonMetrics()
{
}

RibbonMetrics* RibbonMetrics::instance()
{
    static RibbonMetrics s_instance;
    return &s_instance;
}

void RibbonMetrics::rebuild()
{
    const QFont f = mUsingOverrideFont ? mFont : QGuiApplication::font();
    mMetrics.setFontMetrics(QFontMetrics(f));
    mMetrics.estimateSizeHint(true, false);  // default three-row derivation
    Q_EMIT metricsChanged();
}

QFont RibbonMetrics::font() const
{
    return mUsingOverrideFont ? mFont : QGuiApplication::font();
}

void RibbonMetrics::setFont(const QFont& f)
{
    if (mUsingOverrideFont && mFont == f) {
        return;
    }
    mUsingOverrideFont = true;
    mFont = f;
    Q_EMIT fontChanged();
    rebuild();
}

int RibbonMetrics::tabBarHeight() const
{
    return mMetrics.calcDefaultTabBarHeight();
}

int RibbonMetrics::titleBarHeight() const
{
    return mMetrics.calcDefaultTitleBarHeight();
}

int RibbonMetrics::categoryHeight(bool threeRow) const
{
    return mMetrics.calcCategoryHeight(threeRow, false);
}

int RibbonMetrics::panelTitleHeight() const
{
    return mMetrics.panelTitleHeight;
}

void RibbonMetrics::setPanelTitleHeight(int h)
{
    if (mMetrics.panelTitleHeight == h) {
        return;
    }
    mMetrics.panelTitleHeight = h;
    Q_EMIT metricsChanged();
}

int RibbonMetrics::normalModeMainBarHeight(bool tabOnTitle) const
{
    return SARibbon::Core::SARibbonMetrics::calcMainBarHeight(
        tabBarHeight(), titleBarHeight(), categoryHeight(), tabOnTitle, false);
}

int RibbonMetrics::minimumModeMainBarHeight(bool tabOnTitle) const
{
    return SARibbon::Core::SARibbonMetrics::calcMainBarHeight(
        tabBarHeight(), titleBarHeight(), categoryHeight(), tabOnTitle, true);
}

const SARibbon::Core::SARibbonMetrics& RibbonMetrics::coreMetrics() const
{
    return mMetrics;
}

bool RibbonMetrics::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == QGuiApplication::instance() && event->type() == QEvent::ApplicationFontChange) {
        if (!mUsingOverrideFont) {
            rebuild();  // application font drives the metrics (unless overridden)
        }
    }
    return QObject::eventFilter(obj, event);
}

}
