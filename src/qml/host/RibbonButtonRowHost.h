#ifndef RIBBONBUTTONROWHOST_H
#define RIBBONBUTTONROWHOST_H
#include "SARibbonQmlGlobal.h"
#include <QQuickItem>
#include <QVector>

namespace SARibbonQml {

class RibbonToolButton;

/**
 * \if ENGLISH
 * @brief Shared base for title-row button rows (quick access / right group)
 * @details Both bar-level button containers share the whole mechanics: tool
 *          buttons declared as children register through itemChange, sizes
 *          come from the metrics-derived sizeHints (no panel engine — the
 *          row host IS the layout authority for its children), and the row
 *          width is published for the bar to reserve space. Subclasses only
 *          pick the row height and the bar-side placement.
 * \endif
 *
 * \if CHINESE
 * @brief 标题行按钮排的共享基类（快速访问栏 / 右侧按钮组）
 * @details 两个 bar 级按钮容器的机制完全一致：声明为子项的工具按钮经
 *          itemChange 登记，尺寸取度量推导的 sizeHint（无面板引擎——行
 *          宿主即子项的布局权威），行宽发布给 bar 预留空间。子类只需
 *          决定行高与 bar 侧的摆放位置。
 * \endif
 */
class SA_RIBBON_QML_EXPORT RibbonButtonRowHost : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(int rowWidth READ rowWidth NOTIFY rowWidthChanged)
public:
    explicit RibbonButtonRowHost(QQuickItem* parent = nullptr);
    ~RibbonButtonRowHost() override;

    // Sum of the registered buttons' widths + spacing (the bar reserves it)
    int rowWidth() const;

    void registerButton(RibbonToolButton* btn);
    void unregisterButton(RibbonToolButton* btn);

Q_SIGNALS:
    void rowWidthChanged();

protected:
    // vertical extent of the row (subclasses pick; default: title bar height)
    virtual int rowHeight() const;

    void itemChange(ItemChange change, const ItemChangeData& data) override;
    void updatePolish() override;

private:
    void layoutButtons();

    QVector< RibbonToolButton* > mButtons;
    int mRowWidth = 0;
};

}

#endif  // RIBBONBUTTONROWHOST_H
