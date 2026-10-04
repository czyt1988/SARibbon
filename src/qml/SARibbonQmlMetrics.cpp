#include "SARibbonQmlMetrics.h"
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

int RibbonMetrics::fontPointSize() const
{
    return font().pointSize();
}

void RibbonMetrics::setFontPointSize(int ps)
{
    if (ps <= 0) {
        return;
    }
    QFont f = font();
    f.setPointSize(ps);
    setFont(f);
}

QString RibbonMetrics::fontFamily() const
{
    return font().family();
}

void RibbonMetrics::setFontFamily(const QString& family)
{
    if (family.isEmpty()) {
        return;
    }
    QFont f = font();
    f.setFamily(family);
    setFont(f);
}

QStringList RibbonMetrics::commonFontFamilies() const
{
    // a compact, always-available selection for example combos
    // (QFontComboBox parity without exposing QFontDatabase to QML)
    QStringList families;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    // Qt6 made every QFontDatabase accessor static
    const QStringList all = QFontDatabase::families(QFontDatabase::Latin);
#else
    // Qt5: non-static, query through an instance
    const QFontDatabase db;
    const QStringList all = db.families(QFontDatabase::Latin);
#endif
    const QStringList preferred = { QStringLiteral("Microsoft YaHei"), QStringLiteral("Segoe UI"),
                                    QStringLiteral("Arial"), QStringLiteral("Times New Roman"),
                                    QStringLiteral("Courier New"), QStringLiteral("Consolas") };
    for (const QString& p : preferred) {
        if (all.contains(p)) {
            families.append(p);
        }
    }
    if (families.isEmpty() && !all.isEmpty()) {
        families.append(all.first());
    }
    return families;
}

int RibbonMetrics::tabBarHeight() const
{
    return mMetrics.calcDefaultTabBarHeight();
}

int RibbonMetrics::titleBarHeight() const
{
    return mMetrics.calcDefaultTitleBarHeight();
}

int RibbonMetrics::categoryHeightForRows(int rowCount) const
{
    return categoryHeight(rowCount >= 3, rowCount <= 1);
}

int RibbonMetrics::categoryHeight(bool threeRow, bool singleRow) const
{
    return mMetrics.calcCategoryHeight(threeRow, singleRow);
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

int RibbonMetrics::normalModeMainBarHeightFor(bool tabOnTitle, int rowCount) const
{
    return SARibbon::Core::SARibbonMetrics::calcMainBarHeight(
        tabBarHeight(), titleBarHeight(), categoryHeightForRows(rowCount), tabOnTitle, false);
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
