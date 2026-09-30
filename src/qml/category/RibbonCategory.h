#ifndef RIBBONCATEGORY_H
#define RIBBONCATEGORY_H
#include "SARibbonQmlGlobal.h"
#include <SARibbonCore/SARibbonCategoryLayoutEngine.h>
#include <QQuickItem>
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
class RibbonCategory : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QQuickItem* categoryQmlItem READ categoryQmlItem WRITE setCategoryQmlItem NOTIFY categoryQmlItemChanged)
    Q_PROPERTY(int scrollPosition READ scrollPosition WRITE setScrollPosition NOTIFY scrollPositionChanged)
public:
    explicit RibbonCategory(QQuickItem* parent = nullptr);

    QString title() const;
    void setTitle(const QString& t);

    QQuickItem* categoryQmlItem() const;
    void setCategoryQmlItem(QQuickItem* item);

    int scrollPosition() const;
    void setScrollPosition(int pos);

    void registerPanel(RibbonPanel* panel);
    void unregisterPanel(RibbonPanel* panel);

    Q_INVOKABLE int contentWidth() const;

Q_SIGNALS:
    void titleChanged();
    void categoryQmlItemChanged();
    void scrollPositionChanged();

protected:
    void updatePolish() override;
    void itemChange(ItemChange change, const ItemChangeData& data) override;

private:
    void relayout();

    QString mTitle;
    QQuickItem* mCategoryQmlItem = nullptr;
    int mScrollXBase = 0;
    int mTotalWidth = 0;
    QVector< RibbonPanel* > mPanels;
    SARibbon::Core::SARibbonCategoryLayoutEngine mEngine;
};

}
#endif  // RIBBONCATEGORY_H
