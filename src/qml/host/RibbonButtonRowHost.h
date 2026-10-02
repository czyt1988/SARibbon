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
 *          width is published for the bar to reserve space. `exclusive`
 *          supplies the QActionGroup behaviour the widgets front end gets for
 *          free by assigning its actions to a QActionGroup; the QML front end
 *          has no action bridge, so the row host enforces it directly.
 *          Subclasses only pick the row height and the bar-side placement.
 * \endif
 *
 * \if CHINESE
 * @brief 标题行按钮排的共享基类（快速访问栏 / 右侧按钮组）
 * @details 两个 bar 级按钮容器的机制完全一致：声明为子项的工具按钮经
 *          itemChange 登记，尺寸取度量推导的 sizeHint（无面板引擎——行
 *          宿主即子项的布局权威），行宽发布给 bar 预留空间。`exclusive`
 *          补上 widgets 前端把 action 交给 QActionGroup 就自动获得、而 QML
 *          前端因为没有 action 桥而无法表达的那份互斥语义，由行宿主直接
 *          实施。子类只需决定行高与 bar 侧的摆放位置。
 * \endif
 */
class SA_RIBBON_QML_EXPORT RibbonButtonRowHost : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(int rowWidth READ rowWidth NOTIFY rowWidthChanged)
    Q_PROPERTY(bool exclusive READ isExclusive WRITE setExclusive NOTIFY exclusiveChanged)
public:
    explicit RibbonButtonRowHost(QQuickItem* parent = nullptr);
    ~RibbonButtonRowHost() override;

    // Sum of the registered buttons' widths + spacing (the bar reserves it)
    int rowWidth() const;

    bool isExclusive() const;
    void setExclusive(bool on);

    // The single checked button, or nullptr (QActionGroup::checkedAction parity)
    Q_INVOKABLE SARibbonQml::RibbonToolButton* checkedButton() const;

    void registerButton(RibbonToolButton* btn);
    void unregisterButton(RibbonToolButton* btn);

    // Registered button queries (WS-C2: the customizer addresses quick access /
    // right group entries through them)
    int buttonCount() const;
    SARibbonQml::RibbonToolButton* buttonAt(int index) const;
    int buttonIndex(RibbonToolButton* btn) const;

    // Ordered button mutation (WS-C2). Attach re-parents under this row and
    // puts the button at `index` (negative = append); detach un-parents and
    // hides it without destroying it, mirroring widgets
    // SARibbonQuickAccessBar::removeAction, which keeps the QAction alive in
    // the manager so the record list can add it back
    bool attachButton(RibbonToolButton* btn, int index = -1);
    bool detachButton(RibbonToolButton* btn);
    bool moveButton(int from, int to);

Q_SIGNALS:
    void rowWidthChanged();
    void exclusiveChanged();

protected:
    // vertical extent of the row (subclasses pick; default: title bar height)
    virtual int rowHeight() const;

    void itemChange(ItemChange change, const ItemChangeData& data) override;
    void updatePolish() override;

private:
    void layoutButtons();
    void enforceExclusivity(RibbonToolButton* btn);
    // Move an already-registered button to `index` (negative = tail)
    void reorderButton(RibbonToolButton* btn, int index);

    QVector< RibbonToolButton* > mButtons;
    int mRowWidth = 0;
    bool mExclusive = false;
};

}

#endif  // RIBBONBUTTONROWHOST_H
