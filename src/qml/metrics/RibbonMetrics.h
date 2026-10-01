#ifndef RIBBONMETRICS_H
#define RIBBONMETRICS_H
#include "SARibbonQmlGlobal.h"
#include <SARibbonCore/SARibbonMetrics.h>
#include <QObject>
#include <QFont>

namespace SARibbonQml {

/**
 * \if ENGLISH
 * @brief QML singleton bridge over SARibbonMetrics (plan-04 S2)
 * @details All metric construction happens in C++ (QML has no QFontMetrics —
 * TextMetrics lacks lineSpacing/height). Default font metrics come from
 * QGuiApplication::font(); the font property allows an override. Listens to
 * QEvent::ApplicationFontChange on QGuiApplication::instance() (the widgets side listens to per-widget
 * FontChange — conformance tests must set the APPLICATION font to hit both,
 * plan-04 S2 note).
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonMetrics 的 QML 单例桥（计划 04 S2）
 * @details 度量全部在 C++ 内构造（QML 未暴露 QFontMetrics——TextMetrics 无
 *          lineSpacing/height）。默认字体度量取 QGuiApplication::font()，font
 *          属性允许覆盖。在 QGuiApplication::instance() 上监听 ApplicationFontChange（widgets 侧监听
 *          各控件 FontChange——一致性测试须设置应用级字体才能同时触发两端）。
 * \endif
 */
class RibbonMetrics : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)
    Q_PROPERTY(int fontPointSize READ fontPointSize WRITE setFontPointSize NOTIFY fontChanged)
    Q_PROPERTY(QString fontFamily READ fontFamily WRITE setFontFamily NOTIFY fontChanged)
    Q_PROPERTY(int tabBarHeight READ tabBarHeight NOTIFY metricsChanged)
    Q_PROPERTY(int titleBarHeight READ titleBarHeight NOTIFY metricsChanged)
    Q_PROPERTY(int categoryHeight READ categoryHeight NOTIFY metricsChanged)
    Q_PROPERTY(int panelTitleHeight READ panelTitleHeight WRITE setPanelTitleHeight NOTIFY metricsChanged)
    Q_PROPERTY(int normalModeMainBarHeight READ normalModeMainBarHeight NOTIFY metricsChanged)
    Q_PROPERTY(int minimumModeMainBarHeight READ minimumModeMainBarHeight NOTIFY metricsChanged)
public:
    explicit RibbonMetrics(QObject* parent = nullptr);
    ~RibbonMetrics() override;

    static RibbonMetrics* instance();

    QFont font() const;
    void setFont(const QFont& f);

    // QML-friendly pointSize accessor (QFont itself is awkward to build in QML)
    int fontPointSize() const;
    void setFontPointSize(int ps);

    // QML-friendly family accessor (same rebuild chain as pointSize)
    QString fontFamily() const;
    void setFontFamily(const QString& family);

    // Common font families for the example's combo (QFontComboBox parity;
    // QFontDatabase is not exposed to QML)
    Q_INVOKABLE QStringList commonFontFamilies() const;

    // metric reads (formulas live in core SARibbonMetrics; style pixel metrics
    // are style-dependent only through the default style — QML has no QStyle)
    int tabBarHeight() const;
    int titleBarHeight() const;
    // row-count aware category height (core calcCategoryHeight parity; QML
    // cannot build the QFont needed for the panel-level font override, so the
    // bridge carries the app-level one)
    Q_INVOKABLE int categoryHeightForRows(int rowCount) const;
    int categoryHeight(bool threeRow = true, bool singleRow = false) const;
    int panelTitleHeight() const;
    void setPanelTitleHeight(int h);
    Q_INVOKABLE int normalModeMainBarHeightFor(bool tabOnTitle, int rowCount) const;
    int normalModeMainBarHeight(bool tabOnTitle = true) const;
    int minimumModeMainBarHeight(bool tabOnTitle = true) const;

    // expose the underlying core metrics (structural hosts feed their engines)
    const SARibbon::Core::SARibbonMetrics& coreMetrics() const;

Q_SIGNALS:
    void fontChanged();
    void metricsChanged();

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    void rebuild();

    SARibbon::Core::SARibbonMetrics mMetrics;
    QFont mFont;
    bool mUsingOverrideFont = false;
};

}
#endif  // RIBBONMETRICS_H
