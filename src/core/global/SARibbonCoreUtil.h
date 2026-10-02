#ifndef SARIBBONCOREUTIL_H
#define SARIBBONCOREUTIL_H
#include "SARibbonCoreGlobal.h"
#include <QColor>
#include <QSize>
#include <QIcon>
#include <QPixmap>
#include <QList>
#include <QRect>
#include <QLine>

// 计划 02 S1：SARibbonUtil 中无 widget 依赖的函数下沉 core（函数体纯 move，一字不改）。
// namespace SA 维持不变（2.x 既有 API 拼写，v2 §3.5：仅新增类型入 SARibbon::Core）。
// QSS 渲染（replaceQssTokens/getBuiltInRibbonThemeQss）与 widgetDevicePixelRatio 留 widgets（v2 §3.2）。
namespace SA
{

// 让颜色鲜艳
QColor SA_RIBBON_CORE_EXPORT makeColorVibrant(const QColor& c, int saturationDelta = 150, int valueDelta = 30);

// 按照指定的新高度，保持宽高比缩放 QSize
QSize SA_RIBBON_CORE_EXPORT scaleSizeByHeight(const QSize& originalSize, int newHeight);

// 按照指定的新高度，宽高比为1:factor缩放 QSize。
QSize SA_RIBBON_CORE_EXPORT scaleSizeByHeight(const QSize& originalSize, int newHeight, qreal factor);

// 按照指定的新宽度，保持宽高比缩放 QSize
QSize SA_RIBBON_CORE_EXPORT scaleSizeByWidth(const QSize& originalSize, int newWidth);

// 提供类似QIcon::pixmap(const QSize &size, qreal devicePixelRatio, Mode mode, State state) const（Qt6新增）的兼容函数
QPixmap SA_RIBBON_CORE_EXPORT iconToPixmap(const QIcon& icon,
                                           const QSize& size,
                                           qreal devicePixelRatio,
                                           QIcon::Mode mode   = QIcon::Normal,
                                           QIcon::State state = QIcon::Off);

/**
 * \if ENGLISH
 * @brief Check if the application layout direction is Right-to-Left (RTL)
 * @return true if layout direction is Qt::RightToLeft, false otherwise
 * \endif
 *
 * \if CHINESE
 * @brief 检查应用程序布局方向是否为从右到左（RTL）
 * @note core 版改用 QGuiApplication::layoutDirection()（QtGui，行为与 widgets 版等价）；
 * 三个布局引擎一律经 Input.isRTL 入参取该值，不得直接调用本函数（确定性层禁区）
 * \endif
 */
bool SA_RIBBON_CORE_EXPORT saIsRTL();

/**
 * \if ENGLISH
 * @brief Mirror X coordinate for RTL layout support
 * @param x The original X coordinate
 * @param containerWidth The width of the container
 * @param elementWidth The width of the element
 * @return containerWidth - x - elementWidth when RTL, x unchanged when LTR
 * \endif
 *
 * \if CHINESE
 * @brief 为 RTL 布局支持镜像 X 坐标
 * @param x 原始 X 坐标
 * @param containerWidth 容器宽度
 * @param elementWidth 元素宽度
 * @return RTL 时返回 containerWidth - x - elementWidth，LTR 时返回 x 不变
 * \endif
 */
int SA_RIBBON_CORE_EXPORT saMirrorX(int x, int containerWidth, int elementWidth);

// Check if the operating system uses dark mode (cross-platform)
bool SA_RIBBON_CORE_EXPORT isOperatingSystemInDarkMode();

// Enable or disable automatic switching from the default theme to RibbonThemeDark when the operating system is in dark mode (enabled by default)
void SA_RIBBON_CORE_EXPORT setEnableSystemDarkModeAutoSwitch(bool on);

// Query whether automatic theme switching by operating system dark mode is enabled
bool SA_RIBBON_CORE_EXPORT isEnableSystemDarkModeAutoSwitch();

/**
 * \if ENGLISH
 * @brief Gallery grid cell size shared by both front ends
 * @param galleryHeight The gallery body height (cell height derives from it)
 * @param displayRow Visible grid rows, clamped to [1, 3]
 * @param gridMinimumWidth Minimum cell width (<= 0 disables the lower bound)
 * @param gridMaximumWidth Maximum cell width (<= 0 disables the upper bound)
 * @return (width, height) of one grid cell
 * @details Moved from SARibbonGalleryGroup::recalcGridSize so the QML gallery
 *          derives identical cells from identical inputs.
 * \endif
 *
 * \if CHINESE
 * @brief 画廊网格单元尺寸（双前端共用）
 * @param galleryHeight 画廊主体高度（单元高度由此推导）
 * @param displayRow 可见网格行数，钳制到 [1, 3]
 * @param gridMinimumWidth 单元最小宽度（<= 0 关闭下限）
 * @param gridMaximumWidth 单元最大宽度（<= 0 关闭上限）
 * @return 单个网格单元的 (width, height)
 * @details 自 SARibbonGalleryGroup::recalcGridSize 下沉，QML 画廊以相同
 *          输入推导出相同单元。
 * \endif
 */
QSize SA_RIBBON_CORE_EXPORT calcGalleryGridCellSize(int galleryHeight,
                                                    int displayRow,
                                                    int gridMinimumWidth,
                                                    int gridMaximumWidth);

/**
 * \if ENGLISH
 * @brief Caption style of one gallery grid cell
 * @details Mirrors SARibbonGalleryGroup::GalleryGroupStyle; the enumeration
 *          lives in core so both front ends reserve the same caption band.
 * \endif
 *
 * \if CHINESE
 * @brief 画廊网格单元的标题样式
 * @details 对应 SARibbonGalleryGroup::GalleryGroupStyle；枚举放 core，两个
 *          前端才能预留出相同的标题带高度。
 * \endif
 */
enum class GalleryCaptionStyle
{
    None,        ///< icon only, no caption band
    SingleLine,  ///< one caption line (widgets IconWithText)
    WordWrap     ///< two caption lines (widgets IconWithWordWrapText)
};

/**
 * \if ENGLISH
 * @brief Icon box and caption band of one gallery grid cell
 * \endif
 *
 * \if CHINESE
 * @brief 单个画廊网格单元的图标盒与标题带
 * \endif
 */
struct SA_RIBBON_CORE_EXPORT GalleryCellMetrics
{
    QSize iconSize;      ///< icon box inside the cell (widgets setIconSize parity)
    int captionHeight { 0 };  ///< caption band height reserved at the cell bottom
};

/**
 * \if ENGLISH
 * @brief Split a gallery grid cell into its icon box and caption band
 * @param cellWidth Grid cell width (from calcGalleryGridCellSize)
 * @param cellHeight Grid cell height (from calcGalleryGridCellSize)
 * @param lineSpacing Font line spacing used for the caption
 * @param spacing Cell spacing (widgets SARibbonGalleryGroup::spacing, default 1)
 * @param captionStyle Caption style of the cell
 * @return Icon box size and caption band height
 * @details Moved from SARibbonGalleryGroup::recalcGridSize (icon-size part) so
 *          the QML gallery reserves the very same caption band instead of
 *          inventing one from the panel title height. The hover-shift reserve
 *          (4px, widgets shiftpix) and the per-style fallback when the cell is
 *          too short for the caption are preserved verbatim.
 * \endif
 *
 * \if CHINESE
 * @brief 把一个画廊网格单元切分为图标盒与标题带
 * @param cellWidth 网格单元宽度（来自 calcGalleryGridCellSize）
 * @param cellHeight 网格单元高度（来自 calcGalleryGridCellSize）
 * @param lineSpacing 标题使用的字体行间距
 * @param spacing 单元间距（widgets SARibbonGalleryGroup::spacing，默认 1）
 * @param captionStyle 单元标题样式
 * @return 图标盒尺寸与标题带高度
 * @details 自 SARibbonGalleryGroup::recalcGridSize（图标尺寸部分）下沉，QML
 *          画廊由此预留与 widgets 完全相同的标题带，而不是按面板标题高度自行
 *          拼凑。悬停位移预留（4px，widgets shiftpix）与单元过矮时各样式的
 *          回退分支一字未改。
 * \endif
 */
GalleryCellMetrics SA_RIBBON_CORE_EXPORT calcGalleryCellMetrics(int cellWidth,
                                                                int cellHeight,
                                                                int lineSpacing,
                                                                int spacing,
                                                                GalleryCaptionStyle captionStyle);

/**
 * \if ENGLISH
 * @brief The ten standard colors of the office-like color picker
 * @return Standard color list, row-major, left to right
 * @details Moved verbatim from the colorWidgets module so the QML front end
 *          offers the same palette without linking widgets.
 * \endif
 *
 * \if CHINESE
 * @brief office 风格取色器的十个标准色
 * @return 标准色列表，按行优先、自左向右
 * @details 自 colorWidgets 模块原样下沉，QML 前端由此获得同一套色板而无需
 *          链接 widgets。
 * \endif
 */
QList< QColor > SA_RIBBON_CORE_EXPORT getStandardColorList();

/**
 * \if ENGLISH
 * @brief Default shade factors of a color palette grid
 * @return { 180, 160, 140, 75, 50 } — three light rows then two dark rows
 * \endif
 *
 * \if CHINESE
 * @brief 色板网格的默认深浅因子
 * @return { 180, 160, 140, 75, 50 }，即三行浅色接两行深色
 * \endif
 */
QList< int > SA_RIBBON_CORE_EXPORT defaultColorPaletteFactors();

/**
 * \if ENGLISH
 * @brief Derive the shade rows of a color palette from its base colors
 * @param base Base colors, one per palette column
 * @param factors One factor per palette row; each is fed to QColor::lighter
 * @return factors.size() * base.size() colors, row-major (factor outer loop)
 * @details Moved from SAColorPaletteGridWidget::PrivateData::makeColorPalette;
 *          the loop order is part of the contract because the grid fills its
 *          cells row-major.
 * \endif
 *
 * \if CHINESE
 * @brief 由基准色推导出色板的深浅行
 * @param base 基准色，每列一个
 * @param factors 每行一个因子，逐个交给 QColor::lighter
 * @return factors.size() * base.size() 个颜色，按行优先（因子在外层循环）
 * \details 自 SAColorPaletteGridWidget::PrivateData::makeColorPalette 下沉；
 *          循环次序属于契约的一部分，因为网格是按行优先填充单元的。
 * \endif
 */
QList< QColor > SA_RIBBON_CORE_EXPORT colorPaletteShades(const QList< QColor >& base, const QList< int >& factors);

/**
 * \if ENGLISH
 * @brief Geometry constants of a color tool button
 * @details Moved from the file-local SARibbonColorToolButtonConstants namespace
 *          of the widgets button so both front ends place the color band
 *          identically.
 * \endif
 *
 * \if CHINESE
 * @brief 颜色按钮的几何常量
 * @details 自 widgets 按钮内部的文件级 SARibbonColorToolButtonConstants 下沉，
 *          两个前端由此把色带放在完全相同的位置。
 * \endif
 */
namespace ColorToolButtonConstants
{
constexpr qreal COLOR_BLOCK_RATIO     = 0.25;  ///< band height as a ratio of the icon slot height
constexpr int COLOR_BLOCK_MIN_HEIGHT  = 3;     ///< band height lower bound
constexpr int COLOR_BLOCK_MARGIN      = 1;     ///< gap between the icon box and the band
constexpr int DEFAULT_COLOR_ICON_SIZE = 32;    ///< side of the generated fill-to-icon swatch
constexpr int INVALID_COLOR_PEN_WIDTH = 1;     ///< pen width of the invalid-color outline
constexpr int INVALID_COLOR_LINE_RATIO = 3;    ///< denominator of the invalid-color slash inset
}

/**
 * \if ENGLISH
 * @brief Icon box and color band of a ColorUnderIcon button
 * \endif
 *
 * \if CHINESE
 * @brief ColorUnderIcon 按钮的图标盒与色带
 * \endif
 */
struct SA_RIBBON_CORE_EXPORT ColorUnderIconMetrics
{
    QRect iconRect;   ///< where the icon is drawn: centred horizontally, bottom aligned
    QRect colorRect;  ///< the color band directly under the icon box
};

/**
 * \if ENGLISH
 * @brief Height of the color band under an icon
 * @param iconSlotHeight Height of the whole icon slot
 * @return Band height: slot * COLOR_BLOCK_RATIO, floor COLOR_BLOCK_MIN_HEIGHT,
 *         capped at half the slot; 0 when the slot has no height
 * \endif
 *
 * \if CHINESE
 * @brief 图标下方色带的高度
 * @param iconSlotHeight 整个图标槽的高度
 * @return 色带高度：槽高乘 COLOR_BLOCK_RATIO，下限 COLOR_BLOCK_MIN_HEIGHT，
 *         上限为槽高一半；槽高非正时返回 0
 * \endif
 */
int SA_RIBBON_CORE_EXPORT colorBandHeight(int iconSlotHeight);

/**
 * \if ENGLISH
 * @brief Split an icon slot into the icon box and the color band under it
 * @param iconSlot The whole icon slot of the button (widgets iconSize)
 * @param fittedIconSize Size the icon is actually drawn at; an empty size means
 *        no icon, in which case the band spans the whole slot width
 * @return Icon box and color band rectangles in icon-slot local coordinates;
 *         both are null when the slot leaves no room for the band
 * @details Placement rules moved from
 *          SARibbonColorToolButton::PrivateData::createIconPixmap. Scaling the
 *          icon into the remaining area stays with the caller because the
 *          widgets side derives the fitted size from a device-pixel-ratio aware
 *          QPixmap while the QML side derives it from the published iconSide —
 *          this function only places what it is given.
 * \endif
 *
 * \if CHINESE
 * @brief 把图标槽切分为图标盒与其下方的色带
 * @param iconSlot 按钮的整个图标槽（widgets 的 iconSize）
 * @param fittedIconSize 图标实际绘制的尺寸；空尺寸表示没有图标，此时色带占满
 *        整个槽宽
 * @return 图标槽局部坐标下的图标盒与色带矩形；槽内放不下色带时两者皆为空
 * @details 摆放规则自 SARibbonColorToolButton::PrivateData::createIconPixmap
 *          下沉。把图标缩放进剩余区域这一步留给调用方：widgets 侧的缩放结果来
 *          自带 devicePixelRatio 的 QPixmap，QML 侧来自已发布的 iconSide，本函
 *          数只负责摆放拿到的尺寸。
 * \endif
 */
ColorUnderIconMetrics SA_RIBBON_CORE_EXPORT calcColorUnderIconMetrics(const QSize& iconSlot, const QSize& fittedIconSize);

/**
 * \if ENGLISH
 * @brief Diagonal of the "no color" swatch
 * @param colorRect The swatch rectangle
 * @return The red slash from the lower-left inset to the upper-right inset
 * @details Moved from SAColorToolButton::paintNoneColor; the inset is the
 *          rectangle width over INVALID_COLOR_LINE_RATIO. Only the line is
 *          computed here — the surrounding black outline is the full rectangle.
 * \endif
 *
 * \if CHINESE
 * @brief “无颜色”色块的对角斜线
 * @param colorRect 色块矩形
 * @return 自左下内缩点指向右上内缩点的红色斜线
 * @details 自 SAColorToolButton::paintNoneColor 下沉；内缩量为矩形宽度除以
 *          INVALID_COLOR_LINE_RATIO。此处只算斜线——外框黑线就是整个矩形。
 * \endif
 */
QLine SA_RIBBON_CORE_EXPORT noneColorSlashLine(const QRect& colorRect);

}

#endif  // SARIBBONCOREUTIL_H
