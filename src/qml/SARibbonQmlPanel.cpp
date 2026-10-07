#include "SARibbonQmlPanel.h"
#include "SARibbonQmlLayoutItemHost.h"
#include "SARibbonQmlToolButton.h"
#include "SARibbonQmlCategory.h"
#include "SARibbonQmlMetrics.h"
#include "SARibbonQmlTheme.h"
#include "SARibbonQmlTypes.h"
#include <SARibbonCore/SARibbonCoreUtil.h>
#include <QQuickItem>

namespace SARibbonQml {

RibbonPanel::RibbonPanel(QQuickItem* parent) : RibbonQuickHost(parent)
{
    // RTL flip re-runs the engine pass (SA::saIsRTL() re-read on polish);
    // the theme singleton owns the rtl property and broadcasts the change
    connect(RibbonTheme::instance(), &RibbonTheme::rtlChanged, this, [this]() { polish(); });
}

RibbonPanel::~RibbonPanel()
{
}

QUrl RibbonPanel::leafUrl() const
{
    return SARibbonQmlLeafUrls::panelLeaf();
}

QString RibbonPanel::panelTitle() const
{
    return mPanelTitle;
}

void RibbonPanel::setPanelTitle(const QString& t)
{
    if (mPanelTitle == t) {
        return;
    }
    mPanelTitle = t;
    Q_EMIT panelTitleChanged();
    polish();
}

RibbonEnums::LayoutMode RibbonPanel::layoutMode() const
{
    return mLayoutMode;
}

void RibbonPanel::setLayoutMode(RibbonEnums::LayoutMode mode)
{
    if (mLayoutMode == mode) {
        return;
    }
    mLayoutMode = mode;
    Q_EMIT layoutModeChanged();
    polish();
}

bool RibbonPanel::enableShowPanelTitle() const
{
    return mEnableShowPanelTitle;
}

void RibbonPanel::setEnableShowPanelTitle(bool on)
{
    if (mEnableShowPanelTitle == on) {
        return;
    }
    mEnableShowPanelTitle = on;
    Q_EMIT enableShowPanelTitleChanged();
    polish();
}

void RibbonPanel::applyRibbonStyle(RibbonEnums::LayoutMode mode, bool showPanelTitle, bool wordWrap, bool iconRightText)
{
    // remember the flags so panels/buttons registered later inherit them
    mWordWrap      = wordWrap;
    mIconRightText = iconRightText;
    setEnableShowPanelTitle(showPanelTitle);
    setLayoutMode(mode);
    // forward the item-level flags to the registered tool buttons
    for (RibbonLayoutItemHost* item : mChildItems) {
        if (auto* btn = qobject_cast< RibbonToolButton* >(item)) {
            btn->setWordWrap(wordWrap);
            btn->setIconRightText(iconRightText);
        }
    }
}

void RibbonPanel::registerChildItem(RibbonLayoutItemHost* item)
{
    if (!mChildItems.contains(item)) {
        // The arrival order is NOT the declaration order, and it cannot be
        // repaired here: Repeater delegates arrive at the Repeater's
        // componentComplete (which runs in REVERSE sibling order), and
        // ItemChildAddedChange fires while the delegate still sits at the
        // END of childItems — QQuickRepeater restacks it to its final
        // position only AFTER the notification. Anchor the newcomer at the
        // next layout pass instead, when the visual order has settled
        // (settlePendingOrder); a panel declaring [Repeater A, separator,
        // Repeater B] packed [separator, B..., A...] exactly through the
        // arrival-order scramble.
        mChildItems.append(item);
        mPendingOrderItems.append(item);
        // buttons registered later inherit the current style flags and the
        // layout knobs already pushed down from the category / bar; a button
        // arriving from a title-row host loses its flat flag and its toolbar
        // rendering context (panel buttons paint the content background and
        // follow their proportion again)
        if (auto* btn = qobject_cast< RibbonToolButton* >(item)) {
            btn->setFlat(false);
            btn->setTitleRow(false);
            btn->setWordWrap(mWordWrap);
            btn->setIconRightText(mIconRightText);
            btn->setSmallIconSize(mSmallIconSize);
            btn->setLargeIconSize(mLargeIconSize);
            btn->setButtonMaximumAspectRatio(mButtonMaximumAspectRatio);
            btn->setLargeButtonMinimumWidthRatio(mLargeButtonMinimumWidthRatio);
        }
        polish();
    }
}

int RibbonPanel::settleInsertIndex(RibbonLayoutItemHost* item, const QVector< RibbonLayoutItemHost* >& stillPending) const
{
    // anchor between the nearest settled registered siblings in the VISUAL
    // order: after the nearest preceding one, else before the nearest
    // following one
    const auto kids = childItems();
    const int visualIndex = kids.indexOf(item);
    for (int i = visualIndex - 1; i >= 0; --i) {
        auto* prev = qobject_cast< RibbonLayoutItemHost* >(kids[ i ]);
        if (prev && !stillPending.contains(prev)) {
            const int at = mChildItems.indexOf(prev);
            if (at >= 0) {
                return at + 1;
            }
        }
    }
    for (int i = visualIndex + 1; i < kids.size(); ++i) {
        auto* next = qobject_cast< RibbonLayoutItemHost* >(kids[ i ]);
        if (next && !stillPending.contains(next)) {
            const int at = mChildItems.indexOf(next);
            if (at >= 0) {
                return at;
            }
        }
    }
    // no settled sibling on either side: the item visually precedes every
    // settled registration — it starts the list
    return 0;
}

void RibbonPanel::settlePendingOrder()
{
    if (mPendingOrderItems.isEmpty()) {
        return;
    }
    QVector< RibbonLayoutItemHost* > pending = mPendingOrderItems;
    mPendingOrderItems.clear();
    // settle in VISUAL order so every processed item becomes a valid anchor
    // for the next one (the walk skips the not-yet-processed tail)
    const auto kids = childItems();
    std::sort(pending.begin(), pending.end(),
              [ &kids ](RibbonLayoutItemHost* a, RibbonLayoutItemHost* b) {
                  return kids.indexOf(a) < kids.indexOf(b);
              });
    while (!pending.isEmpty()) {
        RibbonLayoutItemHost* item = pending.takeFirst();
        if (!mChildItems.contains(item)) {
            continue;  // unregistered in between
        }
        mChildItems.removeOne(item);
        mChildItems.insert(settleInsertIndex(item, pending), item);
    }
}

/**
 * \if ENGLISH
 * @brief Push the bar-level layout factors down to every registered button
 * @details Counterpart of SARibbonPanel::setButtonMaximumAspectRatio /
 *          setLargeButtonMinimumWidthRatio: the widgets panel forwards the pair
 *          to its layout and to each SARibbonToolButton child, and the QML host
 *          does the same through the button setters. The values are stored so a
 *          button declared after the push inherits them (registerChildItem).
 *          Each button setter drops the engine sizeHint cache entry and
 *          re-polishes the panel, so no extra invalidate call is needed here.
 * \endif
 *
 * \if CHINESE
 * @brief 把 bar 级布局系数下发到每个已注册按钮
 * @details 对应 SARibbonPanel::setButtonMaximumAspectRatio /
 *          setLargeButtonMinimumWidthRatio：widgets 面板把这对系数转给自身布局
 *          与每个 SARibbonToolButton 子项，QML 宿主经按钮设置函数做同样的事。
 *          数值会被记住，以便下发之后才声明的按钮继承（registerChildItem）。
 *          按钮的设置函数会丢弃引擎 sizeHint 缓存并重新 polish 面板，故此处
 *          无需再调一次失效接口。
 * \endif
 */
void RibbonPanel::applyLayoutFactors(qreal buttonMaximumAspectRatio, qreal largeButtonMinimumWidthRatio)
{
    mButtonMaximumAspectRatio     = buttonMaximumAspectRatio;
    mLargeButtonMinimumWidthRatio = largeButtonMinimumWidthRatio;
    for (RibbonLayoutItemHost* item : mChildItems) {
        if (auto* btn = qobject_cast< RibbonToolButton* >(item)) {
            btn->setButtonMaximumAspectRatio(buttonMaximumAspectRatio);
            btn->setLargeButtonMinimumWidthRatio(largeButtonMinimumWidthRatio);
        }
    }
}

QSize RibbonPanel::smallIconSize() const
{
    return mSmallIconSize;
}

void RibbonPanel::setSmallIconSize(const QSize& size)
{
    if (mSmallIconSize == size) {
        return;
    }
    mSmallIconSize = size;
    Q_EMIT smallIconSizeChanged();
    for (RibbonLayoutItemHost* item : mChildItems) {
        if (auto* btn = qobject_cast< RibbonToolButton* >(item)) {
            btn->setSmallIconSize(size);
        }
    }
}

QSize RibbonPanel::largeIconSize() const
{
    return mLargeIconSize;
}

void RibbonPanel::setLargeIconSize(const QSize& size)
{
    if (mLargeIconSize == size) {
        return;
    }
    mLargeIconSize = size;
    Q_EMIT largeIconSizeChanged();
    for (RibbonLayoutItemHost* item : mChildItems) {
        if (auto* btn = qobject_cast< RibbonToolButton* >(item)) {
            btn->setLargeIconSize(size);
        }
    }
}

void RibbonPanel::unregisterChildItem(RibbonLayoutItemHost* item)
{
    mPendingOrderItems.removeOne(item);
    if (mChildItems.removeOne(item)) {
        mEngine.removeFromCache(item);
        polish();
    }
}

int RibbonPanel::childItemCount() const
{
    return mChildItems.size();
}

RibbonLayoutItemHost* RibbonPanel::childItemAt(int index) const
{
    return (index >= 0 && index < mChildItems.size()) ? mChildItems[ index ] : nullptr;
}

int RibbonPanel::childItemIndex(RibbonLayoutItemHost* item) const
{
    return mChildItems.indexOf(item);
}

void RibbonPanel::reorderChildItem(RibbonLayoutItemHost* item, int index)
{
    const int from = mChildItems.indexOf(item);
    if (from < 0) {
        return;
    }
    const int to = (index < 0 || index >= mChildItems.size()) ? (mChildItems.size() - 1) : index;
    if (from != to) {
        // an explicit order overrides the deferred visual anchoring
        mPendingOrderItems.removeOne(item);
        mChildItems.move(from, to);
        polish();
    }
}

/**
 * \if ENGLISH
 * @brief Put an existing host item into this panel at a given slot
 * @details The registration itself is left to itemChange: setting the visual
 *          parent makes the panel see an ItemChildAddedChange and append the
 *          item, so this function only has to repair the resulting order. Doing
 *          it this way keeps a single registration path — the declarative one —
 *          and therefore a single place that pushes the inherited style flags
 *          and layout knobs down to the newcomer. An item already living in
 *          another panel is moved, which is what the customize dialog means by
 *          "add this command here".
 * \endif
 *
 * \if CHINESE
 * @brief 把一个既有宿主项放进本面板的指定位置
 * @details 登记动作交给 itemChange：设置视觉父项会让面板收到
 *          ItemChildAddedChange 并把该项追加进来，因此本函数只需修正由此得到的
 *          顺序。这样做保证登记路径唯一——就是声明式那一条——因而也只有一处
 *          负责把继承的样式开关与布局旋钮下发给新来的项。已经住在别的面板里的
 *          项会被搬移过来，这正是定制对话框"把该命令加到这里"的含义。
 * \endif
 */
bool RibbonPanel::attachChildItem(RibbonLayoutItemHost* item, int index)
{
    if (!item) {
        return false;
    }
    if (item->parentItem() != this) {
        item->setParentItem(this);
    }
    item->setVisible(true);
    reorderChildItem(item, index);
    return mChildItems.contains(item);
}

/**
 * \if ENGLISH
 * @brief Place a command into the panel from C++ or QML
 * @details Plan-05 S6 backend-driven route (widgets addAction parity): a fresh
 *          action-bound button is created, the placement proportion lands on
 *          the button instance (contract D3) and the attach goes through the
 *          same single registration path as a declared child (itemChange), so
 *          the style/factor pushes apply to it too.
 * \endif
 *
 * \if CHINESE
 * @brief 从 C++ 或 QML 把一条命令放进面板
 * @details 计划 05 S6 的后端驱动路径（对应 widgets 的 addAction）：新建一个
 *          action 绑定按钮，放置比例落在按钮实例上（契约 D3），并经与声明式
 *          子项相同的唯一登记路径（itemChange）挂入面板，样式/系数下发同样
 *          覆盖它。
 * \endif
 */
SARibbonQml::RibbonToolButton* RibbonPanel::addAction(QAction* action, int proportion, int index)
{
    if (nullptr == action) {
        return nullptr;
    }
    // construct WITHOUT a visual parent: a parented-in-ctor QQuickItem fires
    // ItemChildAddedChange while still inside the base-class constructor,
    // where the derived vtable is not installed yet and the panel's
    // qobject_cast<RibbonLayoutItemHost*> in itemChange would fail silently.
    // The QObject parent is set explicitly, the visual parenting goes through
    // attachChildItem so the single registration path runs at a safe moment
    auto* btn = new RibbonToolButton();
    btn->setParent(this);
    btn->setAction(action);
    RibbonEnums::RowProportion rp = RibbonEnums::Large;
    switch (proportion) {
    case int(SARibbon::Core::SARibbonRowProportion::None):
        rp = RibbonEnums::None;
        break;
    case int(SARibbon::Core::SARibbonRowProportion::Medium):
        rp = RibbonEnums::Medium;
        break;
    case int(SARibbon::Core::SARibbonRowProportion::Small):
        rp = RibbonEnums::Small;
        break;
    default:
        break;
    }
    btn->setProportion(rp);
    if (!attachChildItem(btn, index)) {
        delete btn;
        return nullptr;
    }
    return btn;
}

/**
 * \if ENGLISH
 * @brief Take an item out of the panel without destroying it
 * @details Un-parenting fires ItemChildRemovedChange, which routes through
 *          unregisterChildItem: the item leaves the list, drops its engine
 *          sizeHint cache entry and the panel repacks. Visibility is cleared
 *          first so an item that ends up re-parented elsewhere by the caller
 *          never flashes in its old geometry. The QObject parent is untouched —
 *          lifetime stays with whoever declared or created the item.
 * \endif
 *
 * \if CHINESE
 * @brief 把一项从面板取出但不销毁
 * @details 解除视觉父子关系会触发 ItemChildRemovedChange，经
 *          unregisterChildItem 处理：该项离开列表、丢弃引擎 sizeHint 缓存条目，
 *          面板重新装箱。先清掉可见性，这样调用方稍后把它重新挂到别处时不会
 *          以旧几何闪现一次。QObject 父子关系不动——生命周期仍归声明者或
 *          创建者。
 * \endif
 */
bool RibbonPanel::detachChildItem(RibbonLayoutItemHost* item)
{
    if (!item || !mChildItems.contains(item)) {
        return false;
    }
    item->setVisible(false);
    item->setParentItem(nullptr);
    return !mChildItems.contains(item);
}

bool RibbonPanel::moveChildItem(int from, int to)
{
    if (from < 0 || from >= mChildItems.size() || to < 0 || to >= mChildItems.size() || from == to) {
        return false;
    }
    // an explicit order overrides the deferred visual anchoring
    mPendingOrderItems.removeOne(mChildItems[ from ]);
    mChildItems.move(from, to);
    polish();
    return true;
}

void RibbonPanel::invalidateChildCache(SARibbon::Core::SARibbonAbstractLayoutItem* item)
{
    // engine caches button sizeHints keyed by largeHeight only: a text/proportion
    // change must drop the entry explicitly, then a fresh pass repacks the columns
    mEngine.removeFromCache(item);
    polish();
}

bool RibbonPanel::hasOptionAction() const
{
    return mHasOptionAction;
}

void RibbonPanel::setHasOptionAction(bool on)
{
    if (mHasOptionAction == on) {
        return;
    }
    mHasOptionAction = on;
    Q_EMIT hasOptionActionChanged();
    polish();
}

void RibbonPanel::triggerOptionAction()
{
    Q_EMIT optionActionTriggered();
}

QRectF RibbonPanel::optionButtonRect() const
{
    return QRectF(mLastOptionButtonGeometry);
}

int RibbonPanel::rowCountForMode() const
{
    switch (mLayoutMode) {
    case RibbonEnums::ThreeRowMode:
        return 3;
    case RibbonEnums::TwoRowMode:
        return 2;
    case RibbonEnums::SingleRowMode:
        return 1;
    }
    return 3;
}

void RibbonPanel::componentComplete()
{
    RibbonQuickHost::componentComplete();
    ensureQmlLeaf();
    polish();
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void RibbonPanel::geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    RibbonQuickHost::geometryChange(newGeometry, oldGeometry);
#else
void RibbonPanel::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    RibbonQuickHost::geometryChanged(newGeometry, oldGeometry);
#endif
    if (newGeometry.size() != oldGeometry.size()) {
        polish();  // the category host sizes this item: re-run the engine on resize
    }
}

QRectF RibbonPanel::titleGeometry() const
{
    return QRectF(mLastTitleGeometry);
}

void RibbonPanel::itemChange(ItemChange change, const ItemChangeData& data)
{
    if (change == QQuickItem::ItemChildAddedChange) {
        // declaration order (sibling componentComplete runs reversed)
        if (RibbonLayoutItemHost* item = qobject_cast< RibbonLayoutItemHost* >(data.item)) {
            registerChildItem(item);
        }
    } else if (change == QQuickItem::ItemChildRemovedChange) {
        if (RibbonLayoutItemHost* item = qobject_cast< RibbonLayoutItemHost* >(data.item)) {
            unregisterChildItem(item);
        }
    } else if (change == QQuickItem::ItemVisibleHasChanged) {
        polish();
    }
    RibbonQuickHost::itemChange(change, data);
}

void RibbonPanel::updatePolish()
{
    runLayout();
}

void RibbonPanel::runLayout()
{
    // the arrival order settles to the visual one before every engine pass
    // (Repeater delegates restack after their ItemChildAddedChange, so their
    // final position is only reliable here)
    settlePendingOrder();
    if (mChildItems.isEmpty()) {
        return;
    }
    SARibbon::Core::SARibbonPanelLayoutEngine::Input input;
    input.rowCount        = rowCountForMode();
    input.showPanelTitle  = mEnableShowPanelTitle && !mPanelTitle.isEmpty();
    input.hasTitleLabel   = true;
    input.hasOptionAction = mHasOptionAction;
    input.isRTL           = SA::saIsRTL();
    input.contentsMargins = QMargins(2, 2, 2, 2);  // widgets parity (SARibbonPanelLayout)
    input.spacing         = 2;
    // widgets adapter contract (engine comment: titleTextWidth = fm advance + 4;
    // -1 means "no title"); optionBtnSize mirrors the widgets option button
    const QFontMetrics fm = RibbonMetrics::instance()->coreMetrics().fontMetrics();
    input.titleTextWidth  = input.showPanelTitle ? fm.horizontalAdvance(mPanelTitle) + 4 : -1;
    input.optionBtnSize   = mHasOptionAction ? QSize(16, 16) : QSize();
    input.titleHeight     = RibbonMetrics::instance()->panelTitleHeight();
    input.titleSpace      = 2;
    input.fontMetrics     = fm;
    input.previousSizeHintWidth = mLastSizeHint.width();

    QVector< SARibbon::Core::SARibbonAbstractLayoutItem* > items;
    items.reserve(mChildItems.size());
    for (RibbonLayoutItemHost* item : mChildItems) {
        items.append(item);
    }

    SARibbon::Core::SARibbonPanelLayoutEngine::Result r = mEngine.layout(items, QRect(0, 0, int(width()), int(height())), input);

    mLastSizeHint = r.sizeHint;
    mLastColumnCount = r.columnCount;
    mLastLargeHeight = r.largeHeight;
    if (r.titleGeometry != mLastTitleGeometry) {
        mLastTitleGeometry = r.titleGeometry;
        Q_EMIT titleGeometryChanged();
    }
    if (r.optionBtnGeometry != mLastOptionButtonGeometry) {
        mLastOptionButtonGeometry = r.optionBtnGeometry;
        Q_EMIT optionButtonRectChanged();
    }
    // publish implicit sizes even at zero geometry: the sizeHint derives from the
    // C++ item hints (metrics-driven, rect-independent), and the category reads
    // implicitWidth as its layout hint — gating this on our own size would
    // deadlock the hint chain (category waits for the panel, panel for category).
    // The hint follows the engine output in BOTH directions: keeping the wider
    // of sizeHint and width() would freeze the panel at its widest state, so a
    // style switch that repacks buttons narrower (word wrap off, titles hidden)
    // would leave the panel at its old width — a growing strip of dead space
    setImplicitWidth(qreal(r.sizeHint.width()));
    setImplicitHeight(qreal(r.sizeHint.height()));

    if (width() <= 0 || height() <= 0) {
        return;  // degenerate rect: hints published, item placement skipped
    }
    // apply engine outputs to the REAL quick items
    for (RibbonLayoutItemHost* item : mChildItems) {
        if (!item->isHidden()) {
            item->applyGeometry(item->resultGeometry);
        }
    }
    // publish the fresh large row height to every item (widgets parity: the
    // large-proportion sizeHint width depends on the panel's largeButtonHeight,
    // so the width hint must follow the height whenever it changes). A change
    // invalidates the item hints and re-requests polish; the second pass
    // repacks with the final widths (the engine cache is keyed on largeHeight
    // and drops together with this notification)
    for (RibbonLayoutItemHost* item : mChildItems) {
        item->setLargeButtonHeightContext(r.largeHeight);
    }
}

}
