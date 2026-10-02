#ifndef RIBBONCOLORGRID_H
#define RIBBONCOLORGRID_H
#include "SARibbonQmlGlobal.h"
#include "../host/RibbonQuickHost.h"
#include <QColor>
#include <QList>
#include <QRect>
#include <QSize>
#include <QVariantList>
#include <QVector>

namespace SARibbonQml {

/**
 * \if ENGLISH
 * @brief Color grid host: a rectangular array of pickable color swatches
 * @details The QML counterpart of the widgets SAColorGridWidget. The host owns
 *          every number the leaf needs — column/row counts, the per-cell rect
 *          (published as cellRects) and the implicit size — so the leaf only
 *          paints swatches and reports pointer events back. Cell geometry
 *          reproduces the widgets derivation: a cell is an icon-only
 *          SAColorToolButton, and QGridLayout gives it the larger of its size
 *          hint (icon box plus margins) and its minimum size hint — core
 *          SA::colorGridCellSize resolves that once for both front ends. Cells
 *          are laid out by a QGridLayout with 1px contents margins, so the
 *          swatch is the cell inset by cellMargin on all four sides.
 *          Selection is exclusive (widgets QButtonGroup::setExclusive(true)):
 *          checking a cell unchecks the previous one and reports both halves
 *          through colorToggled in the widgets order (old false, then new
 *          true). A non-checkable grid still reports colorClicked. An invalid
 *          QColor in colorList renders the "no color" mark (widgets
 *          SAColorToolButton::paintNoneColor): a red slash plus a black outline,
 *          with the slash geometry taken from core SA::noneColorSlashLine so
 *          both front ends draw the identical mark.
 * \endif
 *
 * \if CHINESE
 * @brief 颜色网格宿主：可点选的色块矩形阵列
 * @details 对应 widgets 侧 SAColorGridWidget。宿主掌握叶子需要的每一个数字——
 *          列数/行数、每个单元的矩形（以 cellRects 发布）与 implicit 尺寸——
 *          叶子只负责画色块并把指针事件回传。单元几何复现 widgets 的推导：
 *          一个单元即一个 icon-only 的 SAColorToolButton，QGridLayout 取它的
 *          sizeHint（图标盒加边距）与 minimumSizeHint 中较大的那个——这一步由
 *          core 的 SA::colorGridCellSize 为两个前端统一算出。单元再由带 1px
 *          contentsMargins 的 QGridLayout 排布，因此色块就是单元四周内缩
 *          cellMargin 后的区域。
 *          勾选是互斥的（对应 widgets QButtonGroup::setExclusive(true)）：
 *          选中一个单元会取消上一个，并按 widgets 的顺序（先旧 false 再新
 *          true）通过 colorToggled 报告两半。非 checkable 的网格仍会发出
 *          colorClicked。colorList 中的无效 QColor 渲染为"无颜色"标记
 *          （对应 widgets SAColorToolButton::paintNoneColor）：红色斜线加黑色
 *          边框，斜线几何取自 core 的 SA::noneColorSlashLine，两个前端画出
 *          完全相同的标记。
 * \endif
 */
class SA_RIBBON_QML_EXPORT RibbonColorGrid : public RibbonQuickHost
{
    Q_OBJECT
    Q_PROPERTY(QList< QColor > colorList READ colorList WRITE setColorList NOTIFY colorListChanged)
    Q_PROPERTY(int colorCount READ colorCount NOTIFY colorListChanged)
    Q_PROPERTY(int columnCount READ columnCount WRITE setColumnCount NOTIFY columnCountChanged)
    Q_PROPERTY(int spacing READ spacing WRITE setSpacing NOTIFY spacingChanged)
    Q_PROPERTY(int horizontalSpacing READ horizontalSpacing WRITE setHorizontalSpacing NOTIFY spacingChanged)
    Q_PROPERTY(int verticalSpacing READ verticalSpacing WRITE setVerticalSpacing NOTIFY spacingChanged)
    Q_PROPERTY(QSize colorIconSize READ colorIconSize WRITE setColorIconSize NOTIFY colorIconSizeChanged)
    Q_PROPERTY(int cellMargin READ cellMargin WRITE setCellMargin NOTIFY cellMarginChanged)
    Q_PROPERTY(bool colorCheckable READ isColorCheckable WRITE setColorCheckable NOTIFY colorCheckableChanged)
    Q_PROPERTY(int checkedIndex READ checkedIndex WRITE setCheckedIndex NOTIFY checkedIndexChanged)
    Q_PROPERTY(QColor currentCheckedColor READ currentCheckedColor NOTIFY checkedIndexChanged)
    Q_PROPERTY(bool horizontalSpacerToRight READ isHorizontalSpacerToRight WRITE setHorizontalSpacerToRight NOTIFY horizontalSpacerToRightChanged)
    Q_PROPERTY(int spacerWidth READ spacerWidth WRITE setSpacerWidth NOTIFY spacerWidthChanged)
    // grid metrics consumed by the visual leaf (re-published on every relayout)
    Q_PROPERTY(int gridColumns READ gridColumns NOTIFY gridMetricsChanged)
    Q_PROPERTY(int gridRows READ gridRows NOTIFY gridMetricsChanged)
    Q_PROPERTY(int cellWidth READ cellWidth NOTIFY gridMetricsChanged)
    Q_PROPERTY(int cellHeight READ cellHeight NOTIFY gridMetricsChanged)
    Q_PROPERTY(QVariantList cellRects READ cellRects NOTIFY gridMetricsChanged)
    Q_PROPERTY(QVariantList cellNoneColor READ cellNoneColor NOTIFY gridMetricsChanged)
    Q_PROPERTY(int noneColorSlashInset READ noneColorSlashInset NOTIFY gridMetricsChanged)
public:
    explicit RibbonColorGrid(QQuickItem* parent = nullptr);
    ~RibbonColorGrid() override;

    QList< QColor > colorList() const;
    void setColorList(const QList< QColor >& colors);
    int colorCount() const;

    // Columns per row; <= 0 means unlimited, i.e. a single row (widgets parity)
    int columnCount() const;
    void setColumnCount(int c);

    // Grid gaps; spacing()/setSpacing assign the horizontal gap and report it
    // (QGridLayout::setSpacing parity: one value for both directions)
    int spacing() const;
    void setSpacing(int v);
    int horizontalSpacing() const;
    void setHorizontalSpacing(int v);
    int verticalSpacing() const;
    void setVerticalSpacing(int v);

    // Swatch box inside a cell (widgets setColorIconSize parity)
    QSize colorIconSize() const;
    void setColorIconSize(const QSize& s);

    // Inset from the cell edge to the swatch (widgets setMargins parity, 4px)
    int cellMargin() const;
    void setCellMargin(int v);

    bool isColorCheckable() const;
    void setColorCheckable(bool on);

    // Checked cell, -1 for none; ignored while the grid is not checkable
    int checkedIndex() const;
    void setCheckedIndex(int index);
    QColor currentCheckedColor() const;
    Q_INVOKABLE void clearCheckedState();

    // Trailing expanding spring (widgets setHorizontalSpacerToRight parity):
    // widens the implicit size by spacerWidth plus one horizontal gap
    bool isHorizontalSpacerToRight() const;
    void setHorizontalSpacerToRight(bool on);
    int spacerWidth() const;
    void setSpacerWidth(int w);

    // Minimum height of one row, 0 clears it (widgets setRowMinimumHeight parity)
    Q_INVOKABLE void setRowMinimumHeight(int row, int minSize);
    Q_INVOKABLE int rowMinimumHeight(int row) const;

    Q_INVOKABLE QColor colorAt(int index) const;

    int gridColumns() const;
    int gridRows() const;
    int cellWidth() const;
    int cellHeight() const;
    QVariantList cellRects() const;
    /// Per-cell flag: the swatch holds an invalid QColor ("no color" cell)
    QVariantList cellNoneColor() const;
    /// Horizontal inset of the "no color" slash ends (core noneColorSlashLine)
    int noneColorSlashInset() const;

    // Leaf pointer callbacks; every one resolves the index to a color first and
    // ignores out-of-range indices
    Q_INVOKABLE void notifyCellPressed(int index);
    Q_INVOKABLE void notifyCellReleased(int index);
    Q_INVOKABLE void activateCell(int index);

Q_SIGNALS:
    void colorListChanged();
    void columnCountChanged();
    void spacingChanged();
    void colorIconSizeChanged();
    void cellMarginChanged();
    void colorCheckableChanged();
    void checkedIndexChanged();
    void horizontalSpacerToRightChanged();
    void spacerWidthChanged();
    void gridMetricsChanged();

    /**
     * \if ENGLISH
     * @brief A swatch was clicked
     * @param c Its color (invalid for a "no color" cell)
     * \endif
     *
     * \if CHINESE
     * @brief 某个色块被点击
     * @param c 其颜色（"无颜色"单元为无效色）
     * \endif
     */
    void colorClicked(const QColor& c);

    /**
     * \if ENGLISH
     * @brief A swatch was pressed
     * @param c Its color
     * \endif
     *
     * \if CHINESE
     * @brief 某个色块被按下
     * @param c 其颜色
     * \endif
     */
    void colorPressed(const QColor& c);

    /**
     * \if ENGLISH
     * @brief A swatch was released
     * @param c Its color
     * \endif
     *
     * \if CHINESE
     * @brief 某个色块被释放
     * @param c 其颜色
     * \endif
     */
    void colorReleased(const QColor& c);

    /**
     * \if ENGLISH
     * @brief The checked swatch changed (checkable grids only)
     * @param c Its color
     * @param on Whether it became checked
     * \endif
     *
     * \if CHINESE
     * @brief 勾选的色块发生变化（仅 checkable 网格）
     * @param c 其颜色
     * @param on 是否变为勾选
     * \endif
     */
    void colorToggled(const QColor& c, bool on);

protected:
    QUrl leafUrl() const override;
    void componentComplete() override;

private:
    // Recompute columns/rows/cell rects and the implicit size, then publish
    void updateGridMetrics();
    int effectiveRowHeight(int row) const;
    // Exclusive check flip: unmarks the previous cell and reports both halves
    void applyCheckedIndex(int index);

    QList< QColor > mColors;
    QVector< int > mRowMinimumHeights;
    QSize mIconSize { 16, 16 };  ///< widgets SAColorGridWidget::PrivateData::mIconSize default
    int mColumnCount       = 8;  ///< widgets default; <= 0 means a single row
    int mHorizontalSpacing = 0;  ///< widgets QGridLayout::setSpacing(0)
    int mVerticalSpacing   = 0;
    int mCellMargin        = 4;  ///< widgets setMargins(QMargins(4,4,4,4)) in setColorAt
    bool mColorCheckable   = false;
    bool mHorizontalSpacerToRight = false;
    int mSpacerWidth       = 40;  ///< widgets QSpacerItem(40, 20, Expanding, Minimum)
    int mCheckedIndex      = -1;
    int mGridColumns       = 0;
    int mGridRows          = 0;
    int mCellWidth         = 0;
    int mCellHeight        = 0;
    int mNoneColorSlashInset = 0;  ///< core SA::noneColorSlashLine inset of one swatch
    QVariantList mCellRects;
    QVariantList mCellNoneColor;
};

}

#endif  // RIBBONCOLORGRID_H
