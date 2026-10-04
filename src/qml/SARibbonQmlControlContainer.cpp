#include "SARibbonQmlControlContainer.h"
#include "SARibbonQmlMetrics.h"
#include "SARibbonQmlTypes.h"
#include <SARibbonCore/SARibbonEnums.h>
#include <QQuickItem>

namespace SARibbonQml {

namespace {
constexpr int kSmallIconSide = 20;  // widgets parity (SARibbonBar small icon)
constexpr int kLargeIconSide = 32;
constexpr int kLabelSpacing  = 3;
constexpr int kContentMargin = 2;
// Ribbon rows are compact (the panel engine derives them from the category
// height), while QtQuick Controls 2 styles pad their controls for standalone
// forms (Basic: 6..12px). An embedded control therefore gets its padding
// squeezed to this value so its implicit height lands inside one row.
constexpr int kControlPadding = 1;
// Text elides as soon as a column is a fraction of a pixel narrower than the
// control's implicit advance; the hint keeps a small slack against that.
constexpr int kWidthSlack = 2;
}  // namespace

RibbonControlContainer::RibbonControlContainer(QQuickItem* parent) : RibbonLayoutItemHost(parent)
{
    // contract default: widgets addSmallWidget wraps with Small proportion
    rowProportion = SARibbon::Core::SARibbonRowProportion::Small;
    updateLabelWidth();
    updateSuffixWidth();
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

QString RibbonControlContainer::suffixText() const
{
    return mSuffixText;
}

/**
 * \if ENGLISH
 * @brief Set the trailing label text drawn after the embedded control
 * @details Parity with SARibbonLineWidgetContainer::setSuffix. An empty suffix
 *          reserves zero width, so the container geometry is byte-identical to
 *          the pre-suffix behaviour.
 * \endif
 *
 * \if CHINESE
 * @brief 设置画在内嵌控件之后的尾随标签文本
 * @details 对应 SARibbonLineWidgetContainer::setSuffix。空后缀占用零宽度，
 *          因此不设后缀时容器几何与引入该能力之前逐像素相同。
 * \endif
 */
void RibbonControlContainer::setSuffixText(const QString& t)
{
    if (mSuffixText == t) {
        return;
    }
    mSuffixText = t;
    Q_EMIT suffixTextChanged();
    updateSuffixWidth();
    updateSizeHint();
    positionControl();
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
        // adapt the control to the ribbon row it is embedded into: squeeze the
        // style padding (Controls 2 defaults are form-sized, ribbon rows are
        // not) and clip, so nothing the control draws can escape its row
        if (mControl->property("padding").isValid()) {
            mControl->setProperty("padding", kControlPadding);
        }
        mControl->setClip(true);
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

bool RibbonControlContainer::isEnableShowIcon() const
{
    return mEnableShowIcon;
}

/**
 * \if ENGLISH
 * @brief Collapse the icon slot of the leading label strip
 * @details Parity with SARibbonCtrlContainer::setEnableShowIcon, which toggles
 *          the visibility of the icon QLabel so the box layout hands the freed
 *          width to the embedded widget. Here the strip width is recomputed and
 *          the control is repositioned, which is the same net effect.
 * \endif
 *
 * \if CHINESE
 * @brief 收起前端标签条里的图标槽位
 * @details 对应 SARibbonCtrlContainer::setEnableShowIcon——widgets 侧切换图标
 *          QLabel 的可见性，盒布局把腾出的宽度交给内嵌控件。这里改为重算标签
 *          条宽度并重新摆放 control，净效果相同。
 * \endif
 */
void RibbonControlContainer::setEnableShowIcon(bool on)
{
    if (mEnableShowIcon == on) {
        return;
    }
    mEnableShowIcon = on;
    Q_EMIT enableShowIconChanged();
    updateLabelWidth();
    updateSizeHint();
    positionControl();
}

bool RibbonControlContainer::isEnableShowTitle() const
{
    return mEnableShowTitle;
}

/**
 * \if ENGLISH
 * @brief Collapse the title slot of the leading label strip
 * @note Parity with SARibbonCtrlContainer::setEnableShowTitle; the text stays
 *       readable through text() and is only kept out of the metrics.
 * \endif
 *
 * \if CHINESE
 * @brief 收起前端标签条里的标题槽位
 * @note 对应 SARibbonCtrlContainer::setEnableShowTitle；文本本身仍可由 text()
 *       读到，只是不再计入度量。
 * \endif
 */
void RibbonControlContainer::setEnableShowTitle(bool on)
{
    if (mEnableShowTitle == on) {
        return;
    }
    mEnableShowTitle = on;
    Q_EMIT enableShowTitleChanged();
    updateLabelWidth();
    updateSizeHint();
    positionControl();
}

qreal RibbonControlContainer::labelWidth() const
{
    return mLabelWidth;
}

qreal RibbonControlContainer::suffixWidth() const
{
    return mSuffixWidth;
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

void RibbonControlContainer::updateSuffixWidth()
{
    const int w = computeSuffixWidthFromMetrics();
    if (mSuffixWidth != w) {
        mSuffixWidth = w;
        Q_EMIT suffixWidthChanged();
    }
}

int RibbonControlContainer::computeLabelWidthFromMetrics() const
{
    // label strip: [icon 20] spacing [text advance] trailing spacing; an
    // empty label keeps the icon slot only when an icon exists. The two
    // enableShow* switches drop their slot from the metrics entirely, which is
    // what the widgets box layout does when the QLabel is hidden
    const QFontMetrics fm = RibbonMetrics::instance()->coreMetrics().fontMetrics();
    int w = 0;
    if (mEnableShowIcon && !mIconSource.isEmpty()) {
        w += kSmallIconSide + kLabelSpacing;
    }
    if (mEnableShowTitle && !mText.isEmpty()) {
        w += fm.horizontalAdvance(mText) + kLabelSpacing;
    }
    return w;
}

int RibbonControlContainer::computeSuffixWidthFromMetrics() const
{
    // trailing strip: leading spacing + suffix advance; zero when unset so the
    // no-suffix geometry stays untouched
    if (mSuffixText.isEmpty()) {
        return 0;
    }
    const QFontMetrics fm = RibbonMetrics::instance()->coreMetrics().fontMetrics();
    return kLabelSpacing + fm.horizontalAdvance(mSuffixText);
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
    return QSize(mLabelWidth + int(ctrlW) + mSuffixWidth + kContentMargin + kWidthSlack, h);
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
    // control sits after the label strip and fills the row height the panel
    // engine assigned (the label zone is reserved by the leaf rendering, the
    // trailing suffix strip on the other side): every embedded control of a
    // row shares exactly the same height, which is what keeps a radio, a
    // combo and a line edit visually aligned inside one ribbon row
    const bool isLarge = (rowProportion == SARibbon::Core::SARibbonRowProportion::Large);
    const qreal x  = isLarge ? kContentMargin : mLabelWidth;
    const qreal tail = isLarge ? kContentMargin : (mSuffixWidth + kContentMargin);
    const qreal availW = qMax(width() - x - tail, 0.0);
    const qreal availH = qMax(height() - 2 * kContentMargin, 0.0);
    mControl->setPosition(QPointF(x, kContentMargin));
    mControl->setSize(QSizeF(availW, availH));
}

}
