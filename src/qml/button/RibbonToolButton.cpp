#include "RibbonToolButton.h"
#include "../panel/RibbonPanel.h"
#include "../metrics/RibbonMetrics.h"
#include "../SARibbonQmlTypes.h"

namespace SARibbonQml {

RibbonToolButton::RibbonToolButton(QQuickItem* parent)
    : QQuickItem(parent), SARibbon::Core::SARibbonAbstractLayoutItem()
{
    // contract field default follows the 2.x createItem behavior (Large)
    rowProportion = SARibbon::Core::SARibbonRowProportion::Large;
}

RibbonToolButton::~RibbonToolButton()
{
    // leaf destruction: unparent + deleteLater, NEVER direct delete (KDDW Group.cpp rule)
    if (mButtonQmlItem) {
        mButtonQmlItem->setParentItem(nullptr);
        mButtonQmlItem->setParent(nullptr);
        mButtonQmlItem->deleteLater();
        mButtonQmlItem = nullptr;
    }
}

QString RibbonToolButton::text() const
{
    return mText;
}

void RibbonToolButton::setText(const QString& t)
{
    if (mText == t) {
        return;
    }
    mText = t;
    Q_EMIT textChanged();
    updateSizeHint();
}

QString RibbonToolButton::iconSource() const
{
    return mIconSource;
}

void RibbonToolButton::setIconSource(const QString& s)
{
    if (mIconSource == s) {
        return;
    }
    mIconSource = s;
    Q_EMIT iconSourceChanged();
    updateSizeHint();
}

RibbonEnums::RowProportion RibbonToolButton::proportion() const
{
    switch (rowProportion) {  // the contract base field
    case SARibbon::Core::SARibbonRowProportion::None:
        return RibbonEnums::None;
    case SARibbon::Core::SARibbonRowProportion::Large:
        return RibbonEnums::Large;
    case SARibbon::Core::SARibbonRowProportion::Medium:
        return RibbonEnums::Medium;
    case SARibbon::Core::SARibbonRowProportion::Small:
        return RibbonEnums::Small;
    }
    return RibbonEnums::Large;
}

void RibbonToolButton::setProportion(RibbonEnums::RowProportion rp)
{
    SARibbon::Core::SARibbonRowProportion coreRp;
    switch (rp) {
    case RibbonEnums::None:
        coreRp = SARibbon::Core::SARibbonRowProportion::None;
        break;
    case RibbonEnums::Large:
        coreRp = SARibbon::Core::SARibbonRowProportion::Large;
        break;
    case RibbonEnums::Medium:
        coreRp = SARibbon::Core::SARibbonRowProportion::Medium;
        break;
    case RibbonEnums::Small:
        coreRp = SARibbon::Core::SARibbonRowProportion::Small;
        break;
    default:
        return;
    }
    if (rowProportion == coreRp) {
        return;
    }
    rowProportion = coreRp;
    Q_EMIT proportionChanged();
    updateSizeHint();
}

QQuickItem* RibbonToolButton::buttonQmlItem() const
{
    return mButtonQmlItem;
}

void RibbonToolButton::setButtonQmlItem(QQuickItem* item)
{
    if (mButtonQmlItem == item) {
        return;
    }
    mButtonQmlItem = item;
    Q_EMIT buttonQmlItemChanged();
}

QSize RibbonToolButton::sizeHint() const
{
    return mCachedSizeHint;
}

bool RibbonToolButton::isHidden() const
{
    return !isVisible();
}

Qt::Orientations RibbonToolButton::expandingDirections() const
{
    return Qt::Orientations();  // buttons never expand; only Gallery does (widgets)
}

void RibbonToolButton::applyGeometry(const QRect& rect)
{
    setPosition(QPointF(rect.topLeft()));
    setSize(QSizeF(rect.size()));
}

QString RibbonToolButton::debugName() const
{
    return objectName();
}

void RibbonToolButton::componentComplete()
{
    QQuickItem::componentComplete();
    ensureQmlItem();
}

void RibbonToolButton::ensureQmlItem()
{
    if (mButtonQmlItem) {
        return;
    }
    QQuickItem* leaf = createVisualLeaf(this, SARibbonQmlLeafUrls::toolButtonLeaf(), "buttonCpp");
    if (leaf && !mButtonQmlItem) {
        setButtonQmlItem(leaf);  // handshake assigns it; fallback keeps the pair intact
    }
}

void RibbonToolButton::updateSizeHint()
{
    // iron rule (plan-04 S5-1): sizeHint derives in C++ from core metrics,
    // NEVER from the QML leaf's implicit sizes
    mCachedSizeHint = computeSizeHintFromMetrics();
    setImplicitWidth(mCachedSizeHint.width());
    setImplicitHeight(mCachedSizeHint.height());
    // hint changed: drop the stale engine cache entry and re-run the panel layout
    if (RibbonPanel* panel = qobject_cast< RibbonPanel* >(parentItem())) {
        panel->invalidateChildCache(this);
    }
}

QSize RibbonToolButton::computeSizeHintFromMetrics()
{
    const SARibbon::Core::SARibbonMetrics& m = RibbonMetrics::instance()->coreMetrics();
    const QFontMetrics fm = m.fontMetrics();
    const int textW = fm.horizontalAdvance(mText);
    const bool isLarge = (rowProportion == SARibbon::Core::SARibbonRowProportion::Large);
    const int iconSide = isLarge ? 32 : 16;
    if (isLarge) {
        const int largeH = m.calcCategoryHeight(true, false) - m.panelTitleHeight - 2;
        return QSize(qMax(textW + 8, iconSide + 8), qMax(largeH, 22));
    }
    const int rowH = qMax(fm.lineSpacing(), 16);
    return QSize(textW + iconSide + 12, rowH);
}

}
