#include <SARibbonCore/SARibbonPanelLayoutEngine.h>
#include <QMap>
#include <QList>
#include <algorithm>

// 计划 02 S5 Step B：函数体自 src/widgets/SARibbonPanelLayout.cpp 纯 move。
// 机械替换表（其余一字不改）：
//   SARibbonPanelItem*            -> SARibbonAbstractLayoutItem*
//   item->isEmpty()               -> item->isHidden()
//   panel->panelLayoutMode() 映射  -> input.rowCount
//   contentsMargins()/spacing()   -> input.contentsMargins / input.spacing
//   mTitleHeight/mTitleSpace      -> input.titleHeight / input.titleSpace
//   isEnableShowPanelTitle()      -> input.showPanelTitle
//   mTitleLabel 存在性            -> input.hasTitleLabel
//   isHaveOptionAction()          -> input.hasOptionAction
//   optionActionButtonSize()      -> input.optionBtnSize
//   标题文字宽（fm+panelName+4）  -> input.titleTextWidth
//   mButtonSizeHintCache[key=widget] -> mButtonSizeHintCache[key=item]
//   item->widget()->maximumWidth()-> item->maximumWidth()
//   SARibbonGallery::stretchFactor()-> item->stretchFactor()
//   SARibbonPanelItem::Large 等     -> SARibbonRowProportion::Large 等
//   SA::saIsRTL()/saMirrorX()       -> input.isRTL / 内联镜像算术
//   SARibbonPanel::panelHeightHint  -> 本类静态 panelHeightHint
//   mSizeHint/mColumnCount/mLargeHeight/mTitleLabelGeometry/mOptionActionBtnGeometry
//                                    -> Result 局部（由适配器回写）
//   调试打印宏块不随迁（默认编译关闭，NOTES B24）
namespace SARibbon
{
namespace Core
{

SARibbonPanelLayoutEngine::SARibbonPanelLayoutEngine()
{
}

void SARibbonPanelLayoutEngine::removeFromCache(SARibbonAbstractLayoutItem* item)
{
    mButtonSizeHintCache.remove(item);
}

void SARibbonPanelLayoutEngine::clearCache()
{
    mButtonSizeHintCache.clear();
}

int SARibbonPanelLayoutEngine::panelHeightHint(const QFontMetrics& fm, int rowCount, int panelTitleHeight)
{
    int textH = fm.lineSpacing();  // 这里用linespace，因为在换行的情况下，行距是不可忽略的，ribbon的大按钮默认是2行
    switch (rowCount) {
    case 3: {
        // 5.5=（3*1.6+1） （三行）,1是给paneltitle预留的
        return textH * 4.8 + panelTitleHeight;
    } break;
    case 2: {
        // 3=2*1.6
        return textH * 3.2 + panelTitleHeight;
    } break;
    case 1: {
        // Single row: 1.6x text height, no panel title (hidden by default)
        return textH * 1.6;
    } break;
    default: {
        break;
    }
    }
    return (textH * 4.8 + panelTitleHeight);
}

SARibbonPanelLayoutEngine::Result SARibbonPanelLayoutEngine::layout(QVector< SARibbonAbstractLayoutItem* > items,
                                                                    const QRect& setrect,
                                                                    const Input& input)
{
    Result result;

    const int height     = setrect.height();
    const QMargins& mag  = input.contentsMargins;
    const int spacing    = input.spacing;
    const int spacingRow = 1;  // 高度间距，也就是行间距，不同行之间的距离
    int x                = mag.left();
    const int yBegin     = mag.top();
    int titleH           = (input.titleHeight >= 0) ? input.titleHeight : 0;  // 防止负数影响
    int titleSpace       = (input.titleHeight >= 0) ? input.titleSpace : 0;   // 对于没有标题的情况，spacing就不生效
    if (!input.showPanelTitle) {
        titleH     = 0;
        titleSpace = 0;
    }
    // 获取panel的布局模式 3行或者2行
    //  rowcount 是ribbon的行，有2行和3行两种
    const short rowCount = static_cast< short >(input.rowCount);
    // largeHeight是对应large占比的高度
    const int largeHeight = qMax(height - mag.bottom() - mag.top() - titleH - titleSpace, 2);  // 大按钮高度不小于2

    result.largeHeight = largeHeight;
    // sizeHint缓存和大按钮高度绑定：高度变化（panel首次获得真实几何、调整category高度或
    // panel标题高度等）时必须丢弃缓存，否则按钮宽度会一直沿用旧高度算出来的结果
    if (largeHeight != mButtonSizeHintCacheLargeHeight) {
        mButtonSizeHintCache.clear();
        mButtonSizeHintCacheLargeHeight = largeHeight;
    }
    // 计算smallHeight的高度
    const int smallHeight = qMax((largeHeight - (rowCount - 1) * spacingRow) / rowCount, 1);
    // Medium行的y位置
    const int yMediumRow0 = (2 == rowCount) ? yBegin : (yBegin + ((largeHeight - 2 * smallHeight) / 3));
    const int yMediumRow1 = (2 == rowCount) ? (yBegin + smallHeight + spacingRow)
                                            : (yBegin + ((largeHeight - 2 * smallHeight) / 3) * 2 + smallHeight);
    // Small行的y位置
    const int ySmallRow0 = yBegin;
    const int ySmallRow1 = yBegin + smallHeight + spacingRow;
    const int ySmallRow2 = yBegin + 2 * (smallHeight + spacingRow);
    // row用于记录下个item应该属于第几行，item->rowIndex用于记录当前处于第几行，
    // item->rowIndex主要用于SARibbonPanelItem::Medium
    short row  = 0;
    int column = 0;
    // 记录每列最大的宽度
    int columMaxWidth = 0;
    // 记录总宽度
    int totalWidth = 0;

    int itemCount = items.count();

    // 本列第一、二行占比
    SARibbonRowProportion thisColumnRP0 = SARibbonRowProportion::None;
    SARibbonAbstractLayoutItem* lastGeomItem = nullptr;  // 记录最后一个设置位置的item
    for (int i = 0; i < itemCount; ++i) {
        SARibbonAbstractLayoutItem* item = items.at(i);
        if (item->isHidden()) {
            // 如果是hide就直接跳过
            item->rowIndex    = -1;
            item->columnIndex = -1;
            continue;
        }
        // 当开始新的一列时，重置列宽
        if (row == 0) {
            columMaxWidth = 0;
        }
        // Use cached sizeHint to avoid repeated recalculations during resize
        QSize hint;
        auto cacheIt = mButtonSizeHintCache.find(item);
        if (cacheIt != mButtonSizeHintCache.end()) {
            hint = cacheIt.value();
        } else {
            hint = item->sizeHint();
            mButtonSizeHintCache[ item ] = hint;
        }
        // SingleRowMode：所有item在同一行，横向排列
        if (1 == rowCount) {
            item->rowIndex        = 0;
            item->columnIndex     = column;
            item->resultGeometry  = QRect(x, yBegin, hint.width(), smallHeight);
            columMaxWidth         = hint.width();
            x += (columMaxWidth + spacing);
            row           = 0;
            columMaxWidth = 0;
            ++column;
            lastGeomItem = item;
            continue;
        }
        Qt::Orientations exp = item->expandingDirections();
        SARibbonRowProportion rp = item->rowProportion;
        if (SARibbonRowProportion::None == rp) {
            // 为定义行占比但是垂直扩展，就定义为Large占比，否则就是small占比
            if (exp & Qt::Vertical) {
                rp = SARibbonRowProportion::Large;
            } else {
                rp = SARibbonRowProportion::Small;
            }
        }
        // 开始根据占比和layoutmode来布局
        switch (rp) {
        case SARibbonRowProportion::Large: {
            // ！！在Large，如果不是处于新列的第一行，就需要进行换列处理
            // 把large一直设置在下一列的开始
            if (row != 0) {
                x += (columMaxWidth + spacing);
                ++column;
            }
            //
            item->rowIndex        = 0;
            item->columnIndex     = column;
            item->resultGeometry  = QRect(x, yBegin, hint.width(), largeHeight);
            columMaxWidth         = hint.width();
            // 换列，x自动递增到下个坐标，列数增加，行数归零，最大列宽归零
            x += (columMaxWidth + spacing);
            row           = 0;
            columMaxWidth = 0;
            ++column;
        } break;

        case SARibbonRowProportion::Medium: {
            // 2行模式下Medium和small等价
            if (2 == rowCount) {
                if (0 == row) {
                    item->rowIndex        = 0;
                    item->columnIndex     = column;
                    item->resultGeometry  = QRect(x, yMediumRow0, hint.width(), smallHeight);
                    thisColumnRP0         = SARibbonRowProportion::Medium;
                    columMaxWidth         = hint.width();
                    // 下个row为1
                    row = 1;
                    // x不变
                } else {
                    item->rowIndex        = 1;
                    item->columnIndex     = column;
                    item->resultGeometry  = QRect(x, yMediumRow1, hint.width(), smallHeight);
                    // 和上个进行比较得到最长宽度
                    columMaxWidth = qMax(columMaxWidth, hint.width());
                    // 换列，x自动递增到下个坐标，列数增加，行数归零，最大列宽归零
                    x += (columMaxWidth + spacing);
                    row           = 0;
                    columMaxWidth = 0;
                    ++column;
                }
            } else {
                // 3行模式
                if (0 == row) {
                    item->rowIndex        = 0;
                    item->columnIndex     = column;
                    item->resultGeometry  = QRect(x, yMediumRow0, hint.width(), smallHeight);
                    thisColumnRP0         = SARibbonRowProportion::Medium;
                    columMaxWidth         = hint.width();
                    row                   = 1;
                    // x不变
                } else if (1 == row) {
                    item->rowIndex        = 1;
                    item->columnIndex     = column;
                    item->resultGeometry  = QRect(x, yMediumRow1, hint.width(), smallHeight);
                    columMaxWidth         = qMax(columMaxWidth, hint.width());
                    // 换列，x自动递增到下个坐标，列数增加，行数归零，最大列宽归零
                    x += (columMaxWidth + spacing);
                    row           = 0;
                    columMaxWidth = 0;
                    ++column;
                } else {
                    // 这种模式一般情况会发生在当前列前两行是Small，添加了一个Medium
                    // 这时需要先换列
                    // 换列，x自动递增到下个坐标，列数增加，行数归零，最大列宽归零
                    x += (columMaxWidth + spacing);
                    ++column;
                    // 换列后此时等价于0 == row
                    item->rowIndex        = 0;
                    item->columnIndex     = column;
                    item->resultGeometry  = QRect(x, yMediumRow0, hint.width(), smallHeight);
                    thisColumnRP0         = SARibbonRowProportion::Medium;
                    columMaxWidth         = hint.width();
                    row                   = 1;
                }
            }
        } break;

        case SARibbonRowProportion::Small: {
            if (0 == row) {
                // 第一行
                item->rowIndex        = 0;
                item->columnIndex     = column;
                item->resultGeometry  = QRect(x, ySmallRow0, hint.width(), smallHeight);
                thisColumnRP0         = SARibbonRowProportion::Small;
                columMaxWidth         = hint.width();
                // 下个row为1
                row = 1;
                // x不变
            } else if (1 == row) {
                // 第二行
                item->rowIndex        = 1;
                item->columnIndex     = column;
                item->resultGeometry  = QRect(x, ySmallRow1, hint.width(), smallHeight);
                if ((3 == rowCount) && (SARibbonRowProportion::Medium == thisColumnRP0)) {
                    // 三行模式，并且第一行是Medium
                    item->resultGeometry = QRect(x, yMediumRow1, hint.width(), smallHeight);
                }
                // 和上个进行比较得到最长宽度
                columMaxWidth = qMax(columMaxWidth, hint.width());
                // 这里要看两行还是三行，确定是否要换列
                if (2 == rowCount) {
                    // 两行模式，换列
                    // 换列，x自动递增到下个坐标，列数增加，行数归零，最大列宽归零
                    x += (columMaxWidth + spacing);
                    row           = 0;
                    columMaxWidth = 0;
                    ++column;
                } else {
                    // 三行模式，继续增加行数
                    row = 2;
                    // x不变
                }
                if ((3 == rowCount) && (SARibbonRowProportion::Medium == thisColumnRP0)) {
                    // 三行模式，并且第一行是Medium，换列
                    // 换列，x自动递增到下个坐标，列数增加，行数归零，最大列宽归零
                    x += (columMaxWidth + spacing);
                    row           = 0;
                    columMaxWidth = 0;
                    ++column;
                }
            } else {
                // 第三行
                item->rowIndex        = 2;
                item->columnIndex     = column;
                item->resultGeometry  = QRect(x, ySmallRow2, hint.width(), smallHeight);
                // 和上个进行比较得到最长宽度
                columMaxWidth = qMax(columMaxWidth, hint.width());
                // 换列，x自动递增到下个坐标，列数增加，行数归零，最大列宽归零
                x += (columMaxWidth + spacing);
                row           = 0;
                columMaxWidth = 0;
                ++column;
            }
        } break;

        default:
            // 不可能出现
            break;
        }
        lastGeomItem = item;
    }
    // 最后一个元素，更新列数
    //  2022-06-20 此句本来在循环里面，如果最后一个元素隐藏，会导致无法到达此判断导致异常
    if (lastGeomItem) {  // 最后一个元素，更新totalWidth
        if (lastGeomItem->columnIndex != column) {
            // 说明最后一个元素处于最后位置，触发了换列，此时真实列数需要减1，直接等于column索引
            result.columnCount = column;
            // 由于最后一个元素触发了换列，x值是新一列的位置，直接作为totalWidth要减去已经加入的spacing
            totalWidth = x - spacing + mag.right();
        } else {
            // 说明最后一个元素处于非最后位置，没有触发下一个换列，此时真实列数等于column索引+1
            result.columnCount = column + 1;
            // 由于最后一个元素未触发换列，需要计算totalWidth
            totalWidth = x + columMaxWidth + mag.right();
        }
    }

    // 在设置完所有窗口后，再设置扩展属性的窗口
    if (totalWidth < setrect.width() && (setrect.width() - totalWidth) > 10) {
        // 说明可以设置扩展属性的窗口
        recalcExpandGeomArray(items, setrect, input);
    }
    // 布局label
    const int yTitleBegin      = qMax(height - mag.bottom() - titleH, 1);
    bool isTitleWidthThanPanel = false;
    if (input.showPanelTitle && input.hasTitleLabel) {
        result.titleGeometry.setRect(mag.left(), yTitleBegin, setrect.width() - mag.left() - mag.right(), titleH);
        // 这里要确认标题宽度是否大于totalWidth，如果大于，则要把标题的宽度作为totalwidth
        if (input.titleTextWidth >= 0) {
            int textWidth = input.titleTextWidth;
            if (totalWidth < textWidth) {
                totalWidth            = textWidth;
                isTitleWidthThanPanel = true;  // 说明标题的长度大于按钮布局的长度
            }
        }
    }
    // 布局optionActionButton

    if (input.hasOptionAction) {
        QSize optBtnSize = input.optionBtnSize;
        if (input.showPanelTitle) {
            // 有标题
            result.optionBtnGeometry.setRect(
                result.titleGeometry.right() - result.titleGeometry.height(),
                result.titleGeometry.y(),
                result.titleGeometry.height(),
                result.titleGeometry.height()
            );

            // 特殊情况，如果panel的标题长度大于totalWidth，那么说明totalWidth比较短
            // 这时候，optionActionBtn的宽度要加上到标题宽度上
            if (isTitleWidthThanPanel) {
                // 由于文字是居中对齐，因此要扩展2个按钮的宽度
                totalWidth += (2 * titleH);
            }
        } else {
            // 无标题
            result.optionBtnGeometry.setRect(
                setrect.right() - optBtnSize.width() - mag.right(),
                setrect.bottom() - optBtnSize.height() - mag.bottom(),
                optBtnSize.width(),
                optBtnSize.height()
            );
            totalWidth += optBtnSize.width();
        }
    }

    /**
     * \if ENGLISH
     * @brief Handle RTL mirroring for all layout elements
     * @details In RTL mode, all x coordinates are mirrored horizontally across the panel width.
     *          Button columns are reversed, option button moves to the left, and title aligns right.
     * \endif
     *
     * \if CHINESE
     * @brief 处理所有布局元素的RTL镜像
     * @details 在RTL模式下，所有x坐标沿面板宽度水平镜像。按钮列反转，选项按钮移动到左侧，标题右对齐。
     * \endif
     */
    if (input.isRTL) {
        // Mirror all panel items' x coordinates
        for (SARibbonAbstractLayoutItem* item : items) {
            if (!item->isHidden()) {
                int mirroredX = setrect.width() - item->resultGeometry.x() - item->resultGeometry.width();
                item->resultGeometry.moveLeft(mirroredX);
            }
        }

        // Mirror option button position
        if (input.hasOptionAction) {
            int mirroredOptX = setrect.width() - result.optionBtnGeometry.x() - result.optionBtnGeometry.width();
            result.optionBtnGeometry.moveLeft(mirroredOptX);
        }
    }

    // 刷新sizeHint
    int heightHint     = panelHeightHint(input.fontMetrics, input.rowCount, titleH);
    result.sizeHint    = QSize(totalWidth, heightHint);
    result.totalWidth  = totalWidth;
    return result;
}

void SARibbonPanelLayoutEngine::recalcExpandGeomArray(QVector< SARibbonAbstractLayoutItem* >& items,
                                                       const QRect& setrect,
                                                       const Input& input)
{
    // 计算能扩展的尺寸
    // 2.x reads this->mSizeHint.width() here, which is still the PREVIOUS pass value
    // (mSizeHint is overwritten at the end of updateGeomArray), so this input field
    // must be fed with the previous sizeHint width, not the fresh totalWidth
    int expandwidth = setrect.width() - input.previousSizeHintWidth;

    if (expandwidth <= 1) {
        // 没有必要设置
        return;
    }
    // 列扩展信息
    struct _columnExpandInfo
    {
        int oldColumnWidth      = 0;   ///< 原来的列宽
        int columnMaximumWidth  = -1;  ///< 列的最大宽度
        int columnExpandedWidth = 0;   ///< 扩展后列的宽度
        int columnStretch       = 0;   ///< 列内可扩展 item 的 stretch factor 之和（0 表示参与均分）
        QList< SARibbonAbstractLayoutItem* > expandItems;
    };

    // Step 1: 单次遍历同时收集列宽信息和可扩展item
    // 原代码先收集可扩展item，再对每个可扩展列调用 columnWidthInfo() 全量遍历
    // 优化后合并为一次遍历，消除 O(m*n) 嵌套遍历
    QMap< int, _columnExpandInfo > columnExpandInfo;

    for (SARibbonAbstractLayoutItem* item : items) {
        if (item->isHidden()) {
            continue;
        }
        QMap< int, _columnExpandInfo >::iterator ci = columnExpandInfo.find(item->columnIndex);
        if (ci == columnExpandInfo.end()) {
            ci = columnExpandInfo.insert(item->columnIndex, _columnExpandInfo());
        }
        // 收集列宽信息（替代 columnWidthInfo() 调用）
        ci.value().oldColumnWidth     = qMax(ci.value().oldColumnWidth, item->resultGeometry.width());
        ci.value().columnMaximumWidth = qMax(ci.value().columnMaximumWidth, item->maximumWidth());
        // 收集可扩展item并设置标志位
        if (item->expandingDirections() & Qt::Horizontal) {
            ci.value().expandItems.append(item);
            item->isExpandItem = true;
            // 汇总列内可扩展 item 的拉伸系数（issue #47，原为 SARibbonGallery 专属，
            // 经契约 stretchFactor() 泛化——仅 Gallery 适配器覆写，其余返回 0）
            ci.value().columnStretch += item->stretchFactor();
        }
    }

    // 移除无可扩展item的列，并验证列宽有效性
    for (QMap< int, _columnExpandInfo >::iterator i = columnExpandInfo.begin(); i != columnExpandInfo.end();) {
        if (i.value().expandItems.isEmpty()) {
            i = columnExpandInfo.erase(i);
        } else if ((i.value().oldColumnWidth <= 0) || (i.value().oldColumnWidth > i.value().columnMaximumWidth)) {
            // 重置无效列中item的标志位
            for (SARibbonAbstractLayoutItem* item : i.value().expandItems) {
                item->isExpandItem = false;
            }
            i = columnExpandInfo.erase(i);
        } else {
            ++i;
        }
    }

    if (columnExpandInfo.isEmpty()) {
        // 没有需要扩展的就退出
        return;
    }

    // Step 2: 计算扩展后的列宽（直接使用预收集的列宽信息，无需调用 columnWidthInfo()）
    // 权重分配（issue #47）：全部列的 columnStretch 为 0 时退回均分增量（保持既有行为）；
    // 任一列设置过 stretchFactor 后，按"各列原宽之和 + 增量"的总宽度做加权分配，
    // 使最终宽度比接近权重比；系数为 0 的列退回原宽（不参与增量分配），
    // 整数除法的余数按权重从小到大依次补 1px，避免小权重被饿死
    int totalStretch = 0;
    int oldWidthSum  = 0;
    for (const _columnExpandInfo& info : columnExpandInfo) {
        totalStretch += info.columnStretch;
        oldWidthSum += info.oldColumnWidth;
    }

    QMap< int, int > columnExpandWidth;  // columnIndex -> 本列分得的增量宽度
    if (totalStretch <= 0) {
        // 全部为 0：均分（既有行为），余数从第一列开始依次补 1px
        const int colCount = columnExpandInfo.size();
        const int base     = expandwidth / colCount;
        int remainder      = expandwidth - base * colCount;
        for (auto i = columnExpandInfo.begin(); i != columnExpandInfo.end(); ++i) {
            columnExpandWidth[ i.key() ] = base + (remainder-- > 0 ? 1 : 0);
        }
    } else {
        // 有权重：对总宽度（原宽之和 + 增量）加权，最终宽度 ≈ 总宽 × 权重/权重和
        const int totalDistributable = oldWidthSum + expandwidth;
        int remainder                = totalDistributable;
        for (auto i = columnExpandInfo.begin(); i != columnExpandInfo.end(); ++i) {
            const int target = (i.value().columnStretch > 0)
                                   ? (i.value().columnStretch * totalDistributable) / totalStretch
                                   : i.value().oldColumnWidth;  // 0 权重列保持原宽
            const int share  = qBound(0, target - i.value().oldColumnWidth, expandwidth);
            columnExpandWidth[ i.key() ] = share;
            remainder -= (i.value().oldColumnWidth + share);
        }
        // 余数/超发补偿：按权重从小到大依次调整 1px，保证总宽不超发也不遗漏
        QList< QPair< int, int > > sortable;  // (stretch, columnIndex)
        for (auto i = columnExpandInfo.begin(); i != columnExpandInfo.end(); ++i) {
            if (i.value().columnStretch > 0) {
                sortable.append(qMakePair(i.value().columnStretch, i.key()));
            }
        }
        std::sort(sortable.begin(), sortable.end());
        int idx = 0;
        while (remainder > 0 && !sortable.isEmpty()) {
            ++columnExpandWidth[ sortable.at(idx % sortable.size()).second ];
            --remainder;
            ++idx;
        }
        idx = 0;
        while (remainder < 0 && !sortable.isEmpty()) {
            const int col = sortable.at(idx % sortable.size()).second;
            if (columnExpandWidth[ col ] > 0) {
                --columnExpandWidth[ col ];
                ++remainder;
            }
            ++idx;
            if (idx > 4 * sortable.size() * (expandwidth + oldWidthSum + 1)) {
                break;  // 防御性退出，避免理论上的死循环
            }
        }
    }

    for (QMap< int, _columnExpandInfo >::iterator i = columnExpandInfo.begin(); i != columnExpandInfo.end(); ++i) {
        int colwidth = i.value().oldColumnWidth + columnExpandWidth[ i.key() ];  // 先扩展了
        if (colwidth >= i.value().columnMaximumWidth) {
            // 过最大宽度要求
            i.value().columnExpandedWidth = i.value().columnMaximumWidth;
        } else {
            i.value().columnExpandedWidth = colwidth;
        }
    }

    // Step 3: 重新调整尺寸（使用标志位替代 contains() 调用，O(1) 判断）
    // 由于会涉及其他列的变更，因此需要所有都遍历一下
    for (auto i = columnExpandInfo.begin(); i != columnExpandInfo.end(); ++i) {
        int moveXLen = i.value().columnExpandedWidth - i.value().oldColumnWidth;
        for (SARibbonAbstractLayoutItem* item : items) {
            if (item->isHidden() || (item->columnIndex < i.key())) {
                // 之前的列不用管
                continue;
            }
            if (item->columnIndex == i.key()) {
                // 此列的扩展：使用标志位判断（替代 contains()）
                if (item->isExpandItem) {
                    item->resultGeometry.setWidth(i.value().columnExpandedWidth);
                    item->isExpandItem = false;  // 重置标志位
                } else {
                    // 此列不扩展的模块保持原来的尺寸
                    continue;
                }
            } else {
                // 后面的移动
                item->resultGeometry.moveLeft(item->resultGeometry.x() + moveXLen);
            }
        }
    }
}

}
}
