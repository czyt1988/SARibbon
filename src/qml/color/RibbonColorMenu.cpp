#include "RibbonColorMenu.h"
#include "../SARibbonQmlTypes.h"
#include <SARibbonCore/SARibbonCoreUtil.h>
#include <QMetaObject>
#include <QQuickItem>
#include <QRect>

namespace SARibbonQml {

/**
 * \if ENGLISH
 * @brief Constructor
 * @param parent Parent item
 * \endif
 *
 * \if CHINESE
 * @brief 构造函数
 * @param parent 父项
 * \endif
 */
RibbonColorMenu::RibbonColorMenu(QQuickItem* parent)
    : RibbonQuickHost(parent)
    , mStandardColors(SA::getStandardColorList())
    , mPaletteFactors(SA::defaultColorPaletteFactors())
    , mThemeColorsTitle(tr("Theme Colors"))
    , mCustomColorText(tr("Custom Color"))
    , mNoneColorText(tr("None"))
{
    updatePaletteColors();
    updateNoneMarkInset();
}

/**
 * \if ENGLISH
 * @brief Destructor
 * \endif
 *
 * \if CHINESE
 * @brief 析构函数
 * \endif
 */
RibbonColorMenu::~RibbonColorMenu() = default;

QList< QColor > RibbonColorMenu::standardColors() const
{
    return mStandardColors;
}

void RibbonColorMenu::setStandardColors(const QList< QColor >& colors)
{
    if (mStandardColors == colors) {
        return;
    }
    mStandardColors = colors;
    Q_EMIT standardColorsChanged();
    updatePaletteColors();
}

QList< int > RibbonColorMenu::paletteFactors() const
{
    return mPaletteFactors;
}

void RibbonColorMenu::setPaletteFactors(const QList< int >& factors)
{
    if (mPaletteFactors == factors) {
        return;
    }
    mPaletteFactors = factors;
    Q_EMIT paletteFactorsChanged();
    updatePaletteColors();
}

QList< QColor > RibbonColorMenu::paletteColors() const
{
    return mPaletteColors;
}

int RibbonColorMenu::paletteColumns() const
{
    return mStandardColors.size();
}

QList< QColor > RibbonColorMenu::customColors() const
{
    return mCustomColors;
}

void RibbonColorMenu::setCustomColors(const QList< QColor >& colors)
{
    if (mCustomColors == colors) {
        return;
    }
    mCustomColors = colors;
    Q_EMIT customColorsChanged();
}

int RibbonColorMenu::maxCustomColorCount() const
{
    return mMaxCustomColorCount;
}

void RibbonColorMenu::setMaxCustomColorCount(int n)
{
    const int count = qMax(0, n);
    if (mMaxCustomColorCount == count) {
        return;
    }
    mMaxCustomColorCount = count;
    Q_EMIT maxCustomColorCountChanged();
    // a smaller capacity trims the record from the front, keeping the newest
    if (mCustomColors.size() > mMaxCustomColorCount) {
        const int drop = mCustomColors.size() - mMaxCustomColorCount;
        for (int i = 0; i < drop; ++i) {
            mCustomColors.removeFirst();
        }
        Q_EMIT customColorsChanged();
    }
}

QSize RibbonColorMenu::colorIconSize() const
{
    return mColorIconSize;
}

void RibbonColorMenu::setColorIconSize(const QSize& s)
{
    if (mColorIconSize == s) {
        return;
    }
    mColorIconSize = s;
    Q_EMIT colorIconSizeChanged();
}

bool RibbonColorMenu::isNoneColorEnabled() const
{
    return mNoneColorEnabled;
}

void RibbonColorMenu::setNoneColorEnabled(bool on)
{
    if (mNoneColorEnabled == on) {
        return;
    }
    mNoneColorEnabled = on;
    Q_EMIT noneColorEnabledChanged();
}

int RibbonColorMenu::noneMarkSide() const
{
    return mNoneMarkSide;
}

void RibbonColorMenu::setNoneMarkSide(int side)
{
    const int s = qMax(0, side);
    if (mNoneMarkSide == s) {
        return;
    }
    mNoneMarkSide = s;
    updateNoneMarkInset();
    Q_EMIT noneMarkSideChanged();
}

int RibbonColorMenu::noneMarkSlashInset() const
{
    return mNoneMarkSlashInset;
}

QString RibbonColorMenu::themeColorsTitle() const
{
    return mThemeColorsTitle;
}

void RibbonColorMenu::setThemeColorsTitle(const QString& t)
{
    if (mThemeColorsTitle == t) {
        return;
    }
    mThemeColorsTitle = t;
    Q_EMIT themeColorsTitleChanged();
}

QString RibbonColorMenu::customColorText() const
{
    return mCustomColorText;
}

void RibbonColorMenu::setCustomColorText(const QString& t)
{
    if (mCustomColorText == t) {
        return;
    }
    mCustomColorText = t;
    Q_EMIT customColorTextChanged();
}

QString RibbonColorMenu::noneColorText() const
{
    return mNoneColorText;
}

void RibbonColorMenu::setNoneColorText(const QString& t)
{
    if (mNoneColorText == t) {
        return;
    }
    mNoneColorText = t;
    Q_EMIT noneColorTextChanged();
}

int RibbonColorMenu::actionRowHeight() const
{
    return mActionRowHeight;
}

void RibbonColorMenu::setActionRowHeight(int h)
{
    const int v = qMax(0, h);
    if (mActionRowHeight == v) {
        return;
    }
    mActionRowHeight = v;
    Q_EMIT actionRowHeightChanged();
}

int RibbonColorMenu::paletteSpacing() const
{
    return mPaletteSpacing;
}

void RibbonColorMenu::setPaletteSpacing(int v)
{
    const int s = qMax(0, v);
    if (mPaletteSpacing == s) {
        return;
    }
    mPaletteSpacing = s;
    Q_EMIT paletteSpacingChanged();
}

bool RibbonColorMenu::isMenuVisible() const
{
    return mMenuVisible;
}

void RibbonColorMenu::setMenuVisible(bool on)
{
    if (mMenuVisible == on) {
        return;
    }
    mMenuVisible = on;
    Q_EMIT menuVisibleChanged();
}

/**
 * \if ENGLISH
 * @brief Open the popup
 * @details Same shape as RibbonToolButton::openMenu: ask the leaf to open it
 *          and let the popup's onOpened callback publish the visibility, with a
 *          headless fallback for a host that has no rendered leaf yet.
 * \endif
 *
 * \if CHINESE
 * @brief 打开弹窗
 * @details 与 RibbonToolButton::openMenu 同一形状：请叶子打开，由弹窗的 onOpened
 *          回调发布可见性；宿主尚无渲染叶子时走 headless 兜底。
 * \endif
 */
void RibbonColorMenu::openMenu()
{
    if (mMenuVisible) {
        return;
    }
    ensureQmlLeaf();
    QQuickItem* leaf = qmlLeaf();
    if (leaf && QMetaObject::invokeMethod(leaf, "openMenu")) {
        return;
    }
    setMenuVisible(true);  // headless fallback (no rendered leaf, e.g. tests)
}

/**
 * \if ENGLISH
 * @brief Close the popup
 * \endif
 *
 * \if CHINESE
 * @brief 关闭弹窗
 * \endif
 */
void RibbonColorMenu::closeMenu()
{
    if (!mMenuVisible) {
        return;
    }
    QQuickItem* leaf = qmlLeaf();
    if (leaf && QMetaObject::invokeMethod(leaf, "closeMenu")) {
        return;
    }
    setMenuVisible(false);
}

/**
 * \if ENGLISH
 * @brief Close the menu and report a picked color
 * @param c The picked color; an invalid color means "no color"
 * \endif
 *
 * \if CHINESE
 * @brief 关闭菜单并报告选中的颜色
 * @param c 选中的颜色；无效色表示"无颜色"
 * \endif
 */
void RibbonColorMenu::emitSelectedColor(const QColor& c)
{
    closeMenu();
    Q_EMIT selectedColor(c);
}

/**
 * \if ENGLISH
 * @brief Report the "no color" choice
 * @details Widgets onNoneColorActionTriggered. An invalid QColor is the value
 *          the widgets menu reports here, so a receiver distinguishes "no
 *          color" from any real color through QColor::isValid.
 * \endif
 *
 * \if CHINESE
 * @brief 报告"无颜色"这一选择
 * @details 对应 widgets onNoneColorActionTriggered。widgets 菜单在此报告的正是
 *          无效 QColor，因此接收方用 QColor::isValid 区分"无颜色"与任何真实颜色。
 * \endif
 */
void RibbonColorMenu::selectNoneColor()
{
    emitSelectedColor(QColor());
}

/**
 * \if ENGLISH
 * @brief Ask the QML side to pick a color
 * @details The widgets action runs a QColorDialog inline; this module has no
 *          dialog (see the class note), so the row only raises
 *          customColorRequested() and waits for addCustomColor(). The menu stays
 *          open, exactly as it does while the widgets dialog is modal on top of
 *          it.
 * \endif
 *
 * \if CHINESE
 * @brief 请 QML 侧去取一个颜色
 * @details widgets 的 action 就地弹 QColorDialog；本模块不含对话框（见类注释），
 *          因此该行只发出 customColorRequested()，等 addCustomColor() 回灌。菜单
 *          保持打开——与 widgets 对话框压在其上时的表现一致。
 * \endif
 */
void RibbonColorMenu::requestCustomColor()
{
    Q_EMIT customColorRequested();
}

/**
 * \if ENGLISH
 * @brief Append a color to the custom color record
 * @param c Color to record
 * @details Widgets SAColorMenu::PrivateData::recordCustomColor: append while
 *          there is room, otherwise shift everything one place left and put the
 *          new color last, so the record keeps the newest maxCustomColorCount
 *          entries in pick order. An invalid color is refused — the "no color"
 *          entry is a choice, not a custom color.
 * \endif
 *
 * \if CHINESE
 * @brief 把一个颜色追加进自定义颜色记录
 * @param c 要记录的颜色
 * @details 对应 widgets SAColorMenu::PrivateData::recordCustomColor：还有空位就
 *          追加，满了则整体左移一格、新颜色放在最后，因此记录按选取顺序保留最近
 *          maxCustomColorCount 个。无效色被拒绝——"无颜色"是一个选项，不是自定义颜色。
 * \endif
 */
void RibbonColorMenu::recordCustomColor(const QColor& c)
{
    if (!c.isValid()) {
        return;
    }
    if (mMaxCustomColorCount <= 0) {
        return;
    }
    if (mCustomColors.size() < mMaxCustomColorCount) {
        mCustomColors.append(c);
    } else {
        for (int i = 1; i < mCustomColors.size(); ++i) {
            mCustomColors[ i - 1 ] = mCustomColors.at(i);
        }
        mCustomColors.back() = c;
    }
    Q_EMIT customColorsChanged();
}

/**
 * \if ENGLISH
 * @brief Record a color and report it
 * @param c Picked color
 * @details The dialog-accepted path of widgets onCustomColorActionTriggered,
 *          minus the QColorDialog itself (see the class note): the QML side
 *          picks the color and hands it over here.
 * \endif
 *
 * \if CHINESE
 * @brief 记录一个颜色并把它报告出去
 * @param c 选中的颜色
 * @details 即 widgets onCustomColorActionTriggered 中"对话框接受"的那条路径，去掉
 *          QColorDialog 本身（见类注释）：由 QML 侧取色后交给这里。
 * \endif
 */
void RibbonColorMenu::addCustomColor(const QColor& c)
{
    if (!c.isValid()) {
        return;
    }
    recordCustomColor(c);
    emitSelectedColor(c);
}

/**
 * \if ENGLISH
 * @brief Drop every recorded custom color
 * \endif
 *
 * \if CHINESE
 * @brief 清空已记录的自定义颜色
 * \endif
 */
void RibbonColorMenu::clearCustomColors()
{
    if (mCustomColors.isEmpty()) {
        return;
    }
    mCustomColors.clear();
    Q_EMIT customColorsChanged();
}

QUrl RibbonColorMenu::leafUrl() const
{
    return SARibbonQmlLeafUrls::colorMenuLeaf();
}

void RibbonColorMenu::componentComplete()
{
    RibbonQuickHost::componentComplete();
    ensureQmlLeaf();
}

/**
 * \if ENGLISH
 * @brief Republish the derived shade rows
 * @details Both inputs feed one output, so the shades are recomputed on either
 *          change and reported through a single notify signal — the leaf binds
 *          to paletteColors alone (NOTES B48 single-dependency shape).
 * \endif
 *
 * \if CHINESE
 * @brief 重新发布推导出来的深浅行
 * @details 两个输入喂一个输出，因此任一变化都重算深浅行，并经同一个 notify 信号
 *          报告——叶子只绑定 paletteColors（NOTES B48 的单依赖形状）。
 * \endif
 */
void RibbonColorMenu::updatePaletteColors()
{
    mPaletteColors = SA::colorPaletteShades(mStandardColors, mPaletteFactors);
    Q_EMIT paletteColorsChanged();
}

/**
 * \if ENGLISH
 * @brief Recompute the slash inset of the "no color" mark
 * @details Same core call the grid uses, applied to the mark box of this row.
 *          Widgets createNoneColorIcon paints into a 32x32 pixmap rect adjusted
 *          by 1 on every side and lets QMenu scale the icon down; the 1px inset
 *          is kept here so the mark reads the same at the row's own size, and
 *          the published inset is relative to that inset box — the leaf adds the
 *          box origin back when it strokes the line.
 * \endif
 *
 * \if CHINESE
 * @brief 重算"无颜色"标记的斜线内缩量
 * @details 与网格用的是同一个 core 调用，作用在本行的标记盒上。widgets 的
 *          createNoneColorIcon 是在 32x32 像素矩形四周各内缩 1 后绘制，再交给
 *          QMenu 缩小；这里保留那 1px 内缩，使标记在行自身的尺寸下观感一致，
 *          且发布的内缩量相对于内缩后的盒子——叶子描线时再把盒子原点加回去。
 * \endif
 */
void RibbonColorMenu::updateNoneMarkInset()
{
    const QRect box = QRect(0, 0, mNoneMarkSide, mNoneMarkSide).adjusted(1, 1, -1, -1);
    mNoneMarkSlashInset = SA::noneColorSlashLine(box).x1();
}

}
