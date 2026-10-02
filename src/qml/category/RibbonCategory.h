#ifndef RIBBONCATEGORY_H
#define RIBBONCATEGORY_H
#include "SARibbonQmlGlobal.h"
#include "../host/RibbonQuickHost.h"
#include <SARibbonCore/SARibbonCategoryLayoutEngine.h>
#include <SARibbonCore/SARibbonToolButtonLayout.h>
#include <QVector>

namespace SARibbonQml {

class RibbonPanel;

/**
 * \if ENGLISH
 * @brief Category structural host: drives the core CategoryLayoutEngine (plan-04 S4)
 * @details Panel arrangement via SARibbonCategoryLayoutEngine; scrolling uses
 * QML `Behavior on x` (frontend animation, v2 section 3.4.3) with engine-clamped
 * targets.
 * \endif
 *
 * \if CHINESE
 * @brief Category 结构宿主：驱动 core 的 CategoryLayoutEngine（计划 04 S4）
 * @details panel 排布经 SARibbonCategoryLayoutEngine；滚动用 QML Behavior 动画
 *          （前端动画，v2 §3.4.3），目标值经引擎钳制。
 * \endif
 */
class RibbonCategory : public RibbonQuickHost
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(int scrollPosition READ scrollPosition WRITE setScrollPosition NOTIFY scrollPositionChanged)
    Q_PROPERTY(QVariantList separatorXs READ separatorXs NOTIFY separatorXsChanged)
public:
    explicit RibbonCategory(QQuickItem* parent = nullptr);
    ~RibbonCategory() override;

    QString title() const;
    void setTitle(const QString& t);

    int scrollPosition() const;
    void setScrollPosition(int pos);

    // Panel separator x positions (engine-written resultSeparatorGeometry),
    // re-published per relayout for the visual leaf to render
    QVariantList separatorXs() const;

    void registerPanel(RibbonPanel* panel);
    void unregisterPanel(RibbonPanel* panel);

    // Style push from the bar (ribbonStyle propagation chain); panels
    // registered later inherit through the stored fields
    void applyRibbonStyle(int rowCount, bool showPanelTitle, bool wordWrap, bool iconRightText);

    // Layout factor push from the bar (widgets SARibbonCategory::
    // setButtonMaximumAspectRatio parity); panels registered later inherit
    // through the stored fields
    void applyLayoutFactors(qreal buttonMaximumAspectRatio, qreal largeButtonMinimumWidthRatio);

    Q_INVOKABLE int contentWidth() const;

Q_SIGNALS:
    void titleChanged();
    void scrollPositionChanged();
    void separatorXsChanged();

protected:
    QUrl leafUrl() const override;
    void componentComplete() override;
    void updatePolish() override;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry) override;
#else
    void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) override;
#endif
    void itemChange(ItemChange change, const ItemChangeData& data) override;

private:
    void relayout();

    QString mTitle;
    int mScrollXBase = 0;
    int mTotalWidth = 0;
    int mStyleRowCount = 3;       ///< last pushed bar style rows (default LooseThreeRow)
    bool mStyleShowPanelTitle = true;
    bool mStyleWordWrap = true;
    bool mStyleIconRightText = false;
    qreal mButtonMaximumAspectRatio     = SARibbon::Core::ToolButtonLayoutConstants::BUTTON_MAX_ASPECT_RATIO_DEFAULT;
    qreal mLargeButtonMinimumWidthRatio = SARibbon::Core::ToolButtonLayoutConstants::LARGE_BUTTON_MIN_WIDTH_RATIO;
    QVector< RibbonPanel* > mPanels;
    QVariantList mSeparatorXs;
    SARibbon::Core::SARibbonCategoryLayoutEngine mEngine;
};

}
#endif  // RIBBONCATEGORY_H
