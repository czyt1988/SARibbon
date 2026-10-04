#include "SARibbonQmlButtonRowHost.h"
#include "SARibbonQmlToolButton.h"
#include "SARibbonQmlMetrics.h"

namespace SARibbonQml {

namespace {
constexpr int kRowSpacing = 2;
}  // namespace

RibbonButtonRowHost::RibbonButtonRowHost(QQuickItem* parent) : QQuickItem(parent)
{
    setAcceptedMouseButtons(Qt::NoButton);  // structural container
}

RibbonButtonRowHost::~RibbonButtonRowHost()
{
}

int RibbonButtonRowHost::rowWidth() const
{
    return mRowWidth;
}

bool RibbonButtonRowHost::isExclusive() const
{
    return mExclusive;
}

/**
 * \if ENGLISH
 * @brief Turn the row into a single-choice group
 * @details QActionGroup::setExclusive parity, including the part that is easy
 *          to get wrong: switching the flag on does NOT retroactively uncheck
 *          anything, it only constrains the next check. Buttons already
 *          checked stay checked until one of them is clicked, exactly as Qt
 *          behaves for an action group.
 * \endif
 *
 * \if CHINESE
 * @brief 把这一排变成单选组
 * @details 与 QActionGroup::setExclusive 对齐，包括最容易被做错的一点：打开
 *          开关**不会**追溯性地取消已有勾选，只约束下一次勾选。已经处于勾选
 *          态的按钮保持不变，直到其中之一被点击——与 Qt 对 action 组的处理
 *          一致。
 * \endif
 */
void RibbonButtonRowHost::setExclusive(bool on)
{
    if (mExclusive == on) {
        return;
    }
    mExclusive = on;
    Q_EMIT exclusiveChanged();
}

RibbonToolButton* RibbonButtonRowHost::checkedButton() const
{
    for (RibbonToolButton* btn : mButtons) {
        if (btn && btn->isChecked()) {
            return btn;
        }
    }
    return nullptr;
}

void RibbonButtonRowHost::registerButton(RibbonToolButton* btn)
{
    if (btn && !mButtons.contains(btn)) {
        mButtons.append(btn);
        // title-row rendering context: flat buttons paint no normal-state
        // background (widgets theme-base QSS `SARibbonButtonGroupWidget >
        // QToolButton` parity); the panel host clears the flag again when a
        // customize record moves the button back into a panel
        btn->setFlat(true);
        // hint changes re-flow the row (text/icon edits)
        connect(btn, &QQuickItem::implicitWidthChanged, this, [this]() { polish(); });
        connect(btn, &QQuickItem::implicitHeightChanged, this, [this]() { polish(); });
        connect(btn, &RibbonToolButton::checkedChanged, this, [this, btn]() { enforceExclusivity(btn); });
        polish();
    }
}

void RibbonButtonRowHost::unregisterButton(RibbonToolButton* btn)
{
    if (mButtons.removeOne(btn)) {
        disconnect(btn, nullptr, this, nullptr);
        btn->setFlat(false);
        polish();
    }
}

int RibbonButtonRowHost::buttonCount() const
{
    return mButtons.size();
}

RibbonToolButton* RibbonButtonRowHost::buttonAt(int index) const
{
    return (index >= 0 && index < mButtons.size()) ? mButtons[ index ] : nullptr;
}

int RibbonButtonRowHost::buttonIndex(RibbonToolButton* btn) const
{
    return mButtons.indexOf(btn);
}

void RibbonButtonRowHost::reorderButton(RibbonToolButton* btn, int index)
{
    const int from = mButtons.indexOf(btn);
    if (from < 0) {
        return;
    }
    const int to = (index < 0 || index >= mButtons.size()) ? (mButtons.size() - 1) : index;
    if (from != to) {
        mButtons.move(from, to);
        polish();
    }
}

bool RibbonButtonRowHost::attachButton(RibbonToolButton* btn, int index)
{
    if (!btn) {
        return false;
    }
    if (btn->parentItem() != this) {
        btn->setParentItem(this);  // itemChange -> registerButton (appends)
    }
    btn->setVisible(true);
    reorderButton(btn, index);
    return mButtons.contains(btn);
}

bool RibbonButtonRowHost::detachButton(RibbonToolButton* btn)
{
    if (!btn || !mButtons.contains(btn)) {
        return false;
    }
    btn->setVisible(false);
    btn->setParentItem(nullptr);  // itemChange -> unregisterButton
    return !mButtons.contains(btn);
}

bool RibbonButtonRowHost::moveButton(int from, int to)
{
    if (from < 0 || from >= mButtons.size() || to < 0 || to >= mButtons.size() || from == to) {
        return false;
    }
    mButtons.move(from, to);
    polish();
    return true;
}

int RibbonButtonRowHost::rowHeight() const
{
    return RibbonMetrics::instance()->titleBarHeight();
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void RibbonButtonRowHost::geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry)
#else
void RibbonButtonRowHost::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
#endif
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QQuickItem::geometryChange(newGeometry, oldGeometry);
#else
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
#endif
    if (newGeometry.height() != oldGeometry.height()) {
        // the bar host sizes this row (title row height in compact styles,
        // tab row height in loose ones): re-center the buttons vertically
        polish();
    }
}

void RibbonButtonRowHost::itemChange(ItemChange change, const ItemChangeData& data)
{
    if (change == QQuickItem::ItemChildAddedChange) {
        if (RibbonToolButton* btn = qobject_cast< RibbonToolButton* >(data.item)) {
            registerButton(btn);
        }
    } else if (change == QQuickItem::ItemChildRemovedChange) {
        if (RibbonToolButton* btn = qobject_cast< RibbonToolButton* >(data.item)) {
            unregisterButton(btn);
        }
    }
    QQuickItem::itemChange(change, data);
}

void RibbonButtonRowHost::updatePolish()
{
    layoutButtons();
}

void RibbonButtonRowHost::layoutButtons()
{
    // buttons flow horizontally, vertically centered in the row; sizes come
    // from the metrics-derived sizeHints (widgets quick-access/button-group
    // internal row layout parity). The row height follows whatever the bar
    // host sized this item to (title row vs tab row placement), so buttons
    // stay centered instead of hugging the top when the row is taller
    const int h0 = (height() > 0) ? int(height()) : rowHeight();
    int x = 0;
    for (RibbonToolButton* btn : mButtons) {
        const QSize hint = btn->sizeHint();
        const int h      = qMin(hint.height(), h0);
        btn->setPosition(QPointF(x, (qreal(h0) - h) / 2.0));
        btn->setSize(QSizeF(hint.width(), h));
        x += hint.width() + kRowSpacing;
    }
    const int newWidth = qMax(x - kRowSpacing, 0);
    if (newWidth != mRowWidth) {
        mRowWidth = newWidth;
        setImplicitWidth(qreal(mRowWidth));
        Q_EMIT rowWidthChanged();
    }
}

/**
 * \if ENGLISH
 * @brief Keep at most one button checked while exclusive is on
 * @details Called on every checkedChanged of every registered button. The
 *          isChecked() gate is also the re-entrancy guard: unchecking a sibling
 *          emits its own checkedChanged, which re-enters here and returns
 *          immediately because that sibling is no longer checked. A separate
 *          "enforcing" flag would be redundant. The isChecked() filter is also
 *          what keeps untouched siblings out of the way: only buttons that
 *          actually report checked get written to, so no signal storm and no
 *          spurious toggled emissions on the rest of the row.
 * \endif
 *
 * \if CHINESE
 * @brief 互斥开启时保证至多一个按钮处于勾选态
 * @details 每个已登记按钮的 checkedChanged 都会进来一次。isChecked() 这个判据
 *          同时也是重入闸：取消兄弟按钮的勾选会再发一次它自己的
 *          checkedChanged，重入到这里时它已经不是勾选态，直接返回，因此不需要
 *          另设"正在实施互斥"的标志位。也正是这个判据把未受影响的兄弟挡在外
 *          面——只有真的报告勾选态的按钮才会被写，其余按钮既不产生信号风暴，
 *          也不会收到多余的 toggled。
 * \endif
 */
void RibbonButtonRowHost::enforceExclusivity(RibbonToolButton* btn)
{
    if (!mExclusive || !btn || !btn->isChecked()) {
        return;
    }
    for (RibbonToolButton* other : mButtons) {
        if (other && other != btn && other->isChecked()) {
            other->setChecked(false);
        }
    }
}

}
