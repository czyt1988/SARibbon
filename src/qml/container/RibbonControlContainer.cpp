#include "RibbonControlContainer.h"
#include "../metrics/RibbonMetrics.h"
#include "../SARibbonQmlTypes.h"
#include <SARibbonCore/SARibbonEnums.h>
#include <QQuickItem>

namespace SARibbonQml {

namespace {
constexpr int kSmallIconSide = 20;  // widgets parity (SARibbonBar small icon)
constexpr int kLargeIconSide = 32;
constexpr int kLabelSpacing  = 3;
constexpr int kContentMargin = 2;
}  // namespace

RibbonControlContainer::RibbonControlContainer(QQuickItem* parent) : RibbonLayoutItemHost(parent)
{
    // contract default: widgets addSmallWidget wraps with Small proportion
    rowProportion = SARibbon::Core::SARibbonRowProportion::Small;
    updateLabelWidth();
    updateSizeHint();
}

RibbonControlContainer::~RibbonControlContainer()
{
}

QUrl RibbonControlContainer::leafUrl() const
{
    return SARibbonQmlLeafUrls::controlContainerLeaf();
}

QString RibbonControlContainer::text() const
{
    return mText;
}

void RibbonControlContainer::setText(const QString& t)
{
    if (mText == t) {
        return;
    }
    mText = t;
    Q_EMIT textChanged();
    updateLabelWidth();
    updateSizeHint();
}

QString RibbonControlContainer::iconSource() const
{
    return mIconSource;
}

void RibbonControlContainer::setIconSource(const QString& s)
{
    if (mIconSource == s) {
        return;
    }
    mIconSource = s;
    Q_EMIT iconSourceChanged();
    updateLabelWidth();
    updateSizeHint();
}

QQuickItem* RibbonControlContainer::control() const
{
    return mControl;
}

void RibbonControlContainer::setControl(QQuickItem* item)
{
    if (mControl == item) {
        return;
    }
    if (mControl) {
        disconnect(mControl, nullptr, this, nullptr);
    }
    mControl = item;
    if (mControl) {
        // the control belongs to this container from now on (both parents)
        mControl->setParentItem(this);
        mControl->setParent(this);
        // implicit size changes re-run the panel layout (QWidgetItem parity)
        connect(mControl, &QQuickItem::implicitWidthChanged, this, [this]() { updateSizeHint(); });
        connect(mControl, &QQuickItem::implicitHeightChanged, this, [this]() { updateSizeHint(); });
        positionControl();
    }
    Q_EMIT controlChanged();
    updateSizeHint();
}

RibbonEnums::RowProportion RibbonControlContainer::proportion() const
{
    switch (rowProportion) {
    case SARibbon::Core::SARibbonRowProportion::None:
        return RibbonEnums::None;
    case SARibbon::Core::SARibbonRowProportion::Large:
        return RibbonEnums::Large;
    case SARibbon::Core::SARibbonRowProportion::Medium:
        return RibbonEnums::Medium;
    case SARibbon::Core::SARibbonRowProportion::Small:
        return RibbonEnums::Small;
    }
    return RibbonEnums::Small;
}

void RibbonControlContainer::setProportion(RibbonEnums::RowProportion rp)
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
    positionControl();
}

qreal RibbonControlContainer::labelWidth() const
{
    return mLabelWidth;
}

void RibbonControlContainer::componentComplete()
{
    RibbonLayoutItemHost::componentComplete();
    ensureQmlLeaf();
    positionControl();
}

void RibbonControlContainer::largeHeightContextChanged()
{
    updateSizeHint();
}

void RibbonControlContainer::applyGeometry(const QRect& rect)
{
    RibbonLayoutItemHost::applyGeometry(rect);
    positionControl();
}

void RibbonControlContainer::updateLabelWidth()
{
    const int w = computeLabelWidthFromMetrics();
    if (mLabelWidth != w) {
        mLabelWidth = w;
        Q_EMIT labelWidthChanged();
    }
}

int RibbonControlContainer::computeLabelWidthFromMetrics() const
{
    // label strip: [icon 20] spacing [text advance] trailing spacing; an
    // empty label keeps the icon slot only when an icon exists
    const QFontMetrics fm = RibbonMetrics::instance()->coreMetrics().fontMetrics();
    int w = 0;
    if (!mIconSource.isEmpty()) {
        w += kSmallIconSide + kLabelSpacing;
    }
    if (!mText.isEmpty()) {
        w += fm.horizontalAdvance(mText) + kLabelSpacing;
    }
    return w;
}

QSize RibbonControlContainer::sizeHint() const
{
    // iron rule: derived in C++ from core metrics + the control's implicit
    // size (the control is a QQuickItem — its implicit size IS the front-end
    // equivalent of QWidget::sizeHint, not a QML leaf of this module)
    const SARibbon::Core::SARibbonMetrics& m = RibbonMetrics::instance()->coreMetrics();
    const QFontMetrics fm = m.fontMetrics();
    const qreal ctrlW = mControl ? mControl->implicitWidth() : 0;
    const qreal ctrlH = mControl ? mControl->implicitHeight() : 0;
    const bool isLarge = (rowProportion == SARibbon::Core::SARibbonRowProportion::Large);
    if (isLarge) {
        // large embedding (e.g. a calendar-like block): the control spans the
        // full large cell; its own implicit width drives the hint width
        const int largeH = largeButtonHeightContext() > 0
                               ? largeButtonHeightContext()
                               : m.calcCategoryHeight(true, false) - m.panelTitleHeight - 4 - 2;
        const int w = qMax(int(ctrlW) + 2 * kContentMargin, kLargeIconSide + 4);
        return QSize(w, qMax(largeH, 22));
    }
    const int h = qMax(qMax(int(ctrlH), fm.lineSpacing()), 16);
    return QSize(mLabelWidth + int(ctrlW) + kContentMargin, h);
}

void RibbonControlContainer::updateSizeHint()
{
    mCachedSizeHint = sizeHint();
    setImplicitWidth(mCachedSizeHint.width());
    setImplicitHeight(mCachedSizeHint.height());
    invalidatePanelLayout();
}

void RibbonControlContainer::positionControl()
{
    if (!mControl) {
        return;
    }
    // control sits after the label strip, vertically centered, clamped to the
    // container bounds (the label zone is reserved by the leaf rendering)
    const bool isLarge = (rowProportion == SARibbon::Core::SARibbonRowProportion::Large);
    const qreal x  = isLarge ? kContentMargin : mLabelWidth;
    const qreal availW = qMax(width() - x - kContentMargin, 0.0);
    const qreal availH = qMax(height() - 2 * kContentMargin, 0.0);
    const qreal h  = qMin(qMax(mControl->implicitHeight(), 0.0), availH);
    mControl->setPosition(QPointF(x, (height() - h) / 2.0));
    mControl->setSize(QSizeF(availW, h));
}

}
